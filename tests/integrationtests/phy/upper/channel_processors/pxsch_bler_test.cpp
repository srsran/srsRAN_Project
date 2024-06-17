/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "pxsch_bler_test_channel_emulator.h"
#include "pxsch_bler_test_factories.h"
#include "srsran/adt/concurrent_queue.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_result.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor_result_notifier.h"
#include "srsran/phy/upper/rx_buffer_pool.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/ran/pdsch/dlsch_info.h"
#include "srsran/ran/precoding/precoding_codebooks.h"
#include "srsran/ran/pusch/pusch_mcs.h"
#include "srsran/ran/sch/sch_dmrs_power.h"
#include "srsran/ran/sch/sch_mcs.h"
#include "srsran/ran/sch/tbs_calculator.h"
#include "srsran/support/executors/task_worker_pool.h"
#include "fmt/ostream.h"
#include <condition_variable>
#include <gtest/gtest.h>
#include <mutex>
#include <random>
#include <thread>
#include <tuple>

using namespace srsran;

static constexpr subcarrier_spacing scs                         = subcarrier_spacing::kHz30;
static constexpr pusch_mcs_table    mcs_table                   = pusch_mcs_table::qam256;
static constexpr uint16_t           rnti                        = 0x1234;
static constexpr unsigned           bwp_size_rb                 = 273;
static constexpr unsigned           bwp_start_rb                = 0;
static constexpr unsigned           nof_layers                  = 1;
static constexpr unsigned           nof_ofdm_symbols            = 14;
static const symbol_slot_mask       dmrs_symbol_mask            = {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0};
static constexpr unsigned           nof_ldpc_iterations         = 6;
static constexpr dmrs_type          dmrs                        = dmrs_type::TYPE1;
static constexpr unsigned           nof_cdm_groups_without_data = 2;
static constexpr cyclic_prefix      cp                          = cyclic_prefix::NORMAL;
static constexpr unsigned           rv                          = 0;
static constexpr unsigned           n_id                        = 0;
static constexpr unsigned           scrambling_id               = 0;
static constexpr bool               n_scid                      = false;
static constexpr bool               use_early_stop              = true;
static unsigned                     max_nof_threads             = std::thread::hardware_concurrency();
static bool                         show_stats                  = true;
static unsigned                     nof_repetitions             = 10 * 10240 * pow2(to_numerology_value(scs));

namespace {

// Test parameters.
struct pxsch_bler_params {
  // Fading delay profile: TDLA, TDLB, or TDLC.
  std::string channel_delay_profile;
  // Signal to Interference and Noise Ratio in decibels.
  float sinr_dB;
  // Number of receive ports.
  unsigned nof_rx_ports;
  // Modulation and code scheme index.
  sch_mcs_index mcs_index;
  // Frequency allocation.
  prb_interval freq_allocation;
};

std::ostream& operator<<(std::ostream& os, const pxsch_bler_params& params)
{
  fmt::print(os,
             "channel={} sinr={}dB nof_rx_ports={} mcs={} f_alloc=from{}to{}",
             params.channel_delay_profile,
             params.sinr_dB,
             params.nof_rx_ports,
             params.mcs_index.to_uint(),
             params.freq_allocation.start(),
             params.freq_allocation.stop());
  return os;
}

class PxschBlerTestFixture : public ::testing::TestWithParam<pxsch_bler_params>
{
private:
  std::shared_ptr<resource_grid_factory> create_grid_factory()
  {
    std::shared_ptr<channel_precoder_factory> precod_factory = create_channel_precoder_factory("auto");
    report_fatal_error_if_not(precod_factory, "Failed to create channel precoding factory.");

    return create_resource_grid_factory(precod_factory);
  }

protected:
  class pdsch_processor_notifier_adaptor : public pdsch_processor_notifier
  {
  public:
    void on_finish_processing() override { completed = true; }

    void wait_for_completion()
    {
      while (!completed.load()) {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
      }
    }

  private:
    std::atomic<bool> completed = {false};
  };

  /// Implements a PUSCH decoder notifier adaptor for synchronizing the decoder results.
  class pusch_processor_notifier_adaptor : public pusch_processor_result_notifier
  {
  public:
    void on_uci(const pusch_processor_result_control& uci_) override
    {
      std::unique_lock<std::mutex> lock(mutex);
      uci = uci_;
    }

    void on_sch(const pusch_processor_result_data& sch_) override
    {
      std::unique_lock<std::mutex> lock(mutex);
      completed = true;
      sch       = sch_;
      cvar.notify_all();
    }

    const pusch_processor_result_data& wait_for_completion()
    {
      std::unique_lock<std::mutex> lock(mutex);
      while (!completed) {
        cvar.wait(lock);
      }

      return sch;
    }

  private:
    bool                           completed = false;
    pusch_processor_result_control uci;
    pusch_processor_result_data    sch;
    std::mutex                     mutex;
    std::condition_variable        cvar;
  };

  void SetUp() override
  {
    // Prepare executors.
    worker_pool =
        std::make_unique<task_worker_pool<concurrent_queue_policy::locking_mpmc>>("thread", max_nof_threads, 1024);
    executor = std::make_unique<task_worker_pool_executor<concurrent_queue_policy::locking_mpmc>>(*worker_pool);

    // Prepare logging.
    srslog::fetch_basic_logger("ALL").set_level(srslog::basic_levels::warning);

    // Extract test parameters.
    const std::string& channel    = GetParam().channel_delay_profile;
    sch_mcs_index      mcs_index  = GetParam().mcs_index;
    prb_interval       rb_mapping = GetParam().freq_allocation;

    // Compute modulation and code scheme.
    sch_mcs_description mcs_descr = pusch_mcs_get_config(mcs_table, mcs_index, false);

    // Calculate transport block size.
    tbs_calculator_configuration tbs_config = {};
    tbs_config.mcs_descr                    = mcs_descr;
    tbs_config.n_prb                        = rb_mapping.length();
    tbs_config.nof_layers                   = nof_layers;
    tbs_config.nof_symb_sh                  = nof_ofdm_symbols;
    tbs_config.nof_dmrs_prb = dmrs.nof_dmrs_per_rb() * dmrs_symbol_mask.count() * nof_cdm_groups_without_data;
    unsigned tbs            = tbs_calculator_calculate(tbs_config);

    // Select LDPC base graph.
    ldpc_base_graph_type ldpc_base_graph =
        get_ldpc_base_graph(mcs_descr.get_normalised_target_code_rate(), units::bits(tbs));

    // Generate frequency allocation.
    rb_allocation freq_alloc = rb_allocation::make_type1(rb_mapping.start(), rb_mapping.length(), std::nullopt);

    // Create PDSCH processor factory.
    std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory =
        create_sw_pdsch_processor_factory(*executor, max_nof_threads + 1);
    report_fatal_error_if_not(pdsch_proc_factory, "Failted to create PDSCH processor factory.");

    // Create PUSCH processor factory.
    std::shared_ptr<pusch_processor_factory> pusch_proc_factory =
        create_sw_pusch_processor_factory(*executor, max_nof_threads + 1, nof_ldpc_iterations, use_early_stop);
    report_fatal_error_if_not(pusch_proc_factory, "Failted to create PUSCH processor factory.");

    // Create resource grid factory.
    std::shared_ptr<resource_grid_factory> grid_factory = create_grid_factory();
    report_fatal_error_if_not(grid_factory, "Failed to create resource grid factory.");

    // Create PDSCH processor.
    transmitter = pdsch_proc_factory->create();
    report_fatal_error_if_not(transmitter, "Failed to create PDSCH processor.");

    // Create PUSCH processor.
    receiver = pusch_proc_factory->create();
    report_fatal_error_if_not(receiver, "Failed to create PUSCH processor.");

    // Create resource grids.
    tx_grid = grid_factory->create(nof_layers, MAX_NSYMB_PER_SLOT, NRE * MAX_RB);
    rx_grid = grid_factory->create(GetParam().nof_rx_ports, MAX_NSYMB_PER_SLOT, NRE * MAX_RB);

    // Calculate number of codeblocks.
    nof_codeblocks = ldpc::compute_nof_codeblocks(units::bits(tbs), ldpc_base_graph);

    // Prepare receive soft buffer pool.
    rx_buffer_pool_config buffer_pool_config;
    buffer_pool_config.max_codeblock_size   = ldpc::MAX_CODEBLOCK_SIZE;
    buffer_pool_config.nof_buffers          = 1;
    buffer_pool_config.nof_codeblocks       = nof_codeblocks;
    buffer_pool_config.expire_timeout_slots = 1;
    buffer_pool_config.external_soft_bits   = false;

    // Create buffer pool.
    buffer_pool = create_rx_buffer_pool(buffer_pool_config);
    report_error_if_not(buffer_pool, "Failed to create buffer pool.");

    // Prepare PDSCH processor configuration.
    pdsch_config.context                     = std::nullopt;
    pdsch_config.slot                        = slot_point(to_numerology_value(scs), 0);
    pdsch_config.rnti                        = rnti;
    pdsch_config.bwp_size_rb                 = bwp_size_rb;
    pdsch_config.bwp_start_rb                = bwp_start_rb;
    pdsch_config.cp                          = cp;
    pdsch_config.codewords                   = {{mcs_descr.modulation, rv}};
    pdsch_config.n_id                        = n_id;
    pdsch_config.ref_point                   = pdsch_processor::pdu_t::PRB0;
    pdsch_config.dmrs_symbol_mask            = dmrs_symbol_mask;
    pdsch_config.dmrs                        = dmrs;
    pdsch_config.scrambling_id               = scrambling_id;
    pdsch_config.n_scid                      = n_scid;
    pdsch_config.nof_cdm_groups_without_data = nof_cdm_groups_without_data;
    pdsch_config.freq_alloc                  = freq_alloc;
    pdsch_config.start_symbol_index          = 0;
    pdsch_config.nof_symbols                 = nof_ofdm_symbols;
    pdsch_config.ldpc_base_graph             = ldpc_base_graph;
    pdsch_config.tbs_lbrm                    = tbs_lbrm_default;
    pdsch_config.reserved                    = {};
    pdsch_config.ratio_pdsch_data_to_sss_dB  = 0.0F;
    pdsch_config.ratio_pdsch_dmrs_to_sss_dB  = get_sch_to_dmrs_ratio_dB(nof_cdm_groups_without_data);
    pdsch_config.precoding                   = precoding_configuration::make_wideband(make_identity(nof_layers));

    static_vector<uint8_t, MAX_PORTS> rx_ports(GetParam().nof_rx_ports);
    std::iota(rx_ports.begin(), rx_ports.end(), 0U);

    // Prepare PUSCH processor configuration.
    pusch_config.context                     = std::nullopt;
    pusch_config.slot                        = slot_point(to_numerology_value(scs), 0);
    pusch_config.rnti                        = rnti;
    pusch_config.bwp_size_rb                 = bwp_size_rb;
    pusch_config.bwp_start_rb                = bwp_start_rb;
    pusch_config.cp                          = cp;
    pusch_config.mcs_descr                   = mcs_descr;
    pusch_config.codeword                    = {rv, ldpc_base_graph, true};
    pusch_config.uci                         = {};
    pusch_config.n_id                        = n_id;
    pusch_config.nof_tx_layers               = nof_layers;
    pusch_config.rx_ports                    = rx_ports;
    pusch_config.dmrs_symbol_mask            = dmrs_symbol_mask;
    pusch_config.dmrs                        = dmrs;
    pusch_config.scrambling_id               = scrambling_id;
    pusch_config.n_scid                      = n_scid;
    pusch_config.nof_cdm_groups_without_data = nof_cdm_groups_without_data;
    pusch_config.freq_alloc                  = freq_alloc;
    pusch_config.start_symbol_index          = 0;
    pusch_config.nof_symbols                 = nof_ofdm_symbols;
    pusch_config.tbs_lbrm                    = tbs_lbrm_default;
    pusch_config.dc_position                 = {}; //{bwp_size_rb * NRE / 2};

    // Resize data to accomodate the transport block.
    tx_data.resize(tbs / 8);
    rx_data.resize(tbs / 8);

    emulator = std::make_unique<channel_emulator>(channel,
                                                  GetParam().sinr_dB,
                                                  GetParam().nof_rx_ports,
                                                  MAX_RB * NRE,
                                                  nof_ofdm_symbols,
                                                  max_nof_threads,
                                                  scs,
                                                  *executor);
  }

  void TearDown() override
  {
    worker_pool->stop();
    srslog::flush();
  }

  unsigned nof_codeblocks;

  std::unique_ptr<pdsch_processor>           transmitter;
  std::unique_ptr<pusch_processor>           receiver;
  std::unique_ptr<resource_grid>             tx_grid;
  std::unique_ptr<resource_grid>             rx_grid;
  std::unique_ptr<rx_buffer_pool_controller> buffer_pool;

  pdsch_processor::pdu_t pdsch_config;
  pusch_processor::pdu_t pusch_config;

  std::vector<uint8_t> tx_data;
  std::vector<uint8_t> rx_data;

  std::unique_ptr<task_worker_pool<concurrent_queue_policy::locking_mpmc>>          worker_pool;
  std::unique_ptr<task_worker_pool_executor<concurrent_queue_policy::locking_mpmc>> executor;

  std::unique_ptr<channel_emulator> emulator;
};
} // namespace

TEST_P(PxschBlerTestFixture, Fading)
{
  uint64_t                 count            = 0;
  uint64_t                 error_count      = 0;
  unsigned                 max_iterations   = std::numeric_limits<unsigned>::min();
  unsigned                 min_iterations   = std::numeric_limits<unsigned>::max();
  uint64_t                 count_iterations = 0;
  sample_statistics<float> sinr_stats;
  sample_statistics<float> evm_stats;
  sample_statistics<float> ta_stats_us;

  std::mt19937 rgen(0);

  // Iterate different seeds.
  for (unsigned n = 0; n != nof_repetitions; ++n) {
    // Generate random data.
    std::generate(tx_data.begin(), tx_data.end(), [&rgen]() { return static_cast<uint8_t>(rgen() & 0xff); });

    // Process PDSCH.
    pdsch_processor_notifier_adaptor tx_notifier;
    transmitter->process(tx_grid->get_mapper(), tx_notifier, {tx_data}, pdsch_config);
    tx_notifier.wait_for_completion();

    emulator->run(rx_grid->get_writer(), tx_grid->get_reader());

    // Get a receive buffer.
    unique_rx_buffer buffer =
        buffer_pool->get_pool().reserve(pusch_config.slot, trx_buffer_identifier(rnti, 0), nof_codeblocks, true);
    ASSERT_TRUE(buffer.is_valid());

    // Process PUSCH.
    pusch_processor_notifier_adaptor rx_notifier;
    receiver->process(rx_data, std::move(buffer), rx_notifier, rx_grid->get_reader(), pusch_config);

    const pusch_processor_result_data& sch_result = rx_notifier.wait_for_completion();

    // Accumulate counters.
    ++count;
    if (!sch_result.data.tb_crc_ok) {
      ++error_count;
    }
    max_iterations = std::max(sch_result.data.ldpc_decoder_stats.get_max(), max_iterations);
    min_iterations = std::min(sch_result.data.ldpc_decoder_stats.get_min(), min_iterations);
    count_iterations += static_cast<uint64_t>(sch_result.data.ldpc_decoder_stats.get_nof_observations() *
                                              sch_result.data.ldpc_decoder_stats.get_mean());
    if (sch_result.csi.get_evm().has_value()) {
      evm_stats.update(sch_result.csi.get_evm().value());
    }
    if (sch_result.csi.get_sinr_dB().has_value()) {
      sinr_stats.update(sch_result.csi.get_sinr_dB().value());
    }
    if (sch_result.csi.get_time_alignment().has_value()) {
      ta_stats_us.update(sch_result.csi.get_time_alignment()->to_seconds() * 1e6);
    }

    // Increment slots.
    ++pdsch_config.slot;
    ++pusch_config.slot;

    // Set following line to 1 for printing partial results.
    if (show_stats && (n % 100 == 0)) {
      // Calculate resultant metrics.
      double bler            = static_cast<double>(error_count) / static_cast<double>(count);
      double mean_iterations = static_cast<double>(count_iterations) / static_cast<double>(count * nof_codeblocks);

      fmt::print("[{:>5.1f}%] "
                 "Iterations={{{:<2} {:<2} {:<3.1f}}}; "
                 "BLER={:.10f}; "
                 "SINR={{{:+.2f} {:+.2f} {:+.2f}}}; "
                 "EVM={{{:.3f} {:.3f} {:.3f}}}; "
                 "TA={{{:.2f} {:.2f} {:.2f}}}us\r",
                 static_cast<double>(n) / static_cast<double>(nof_repetitions) * 100.0,
                 min_iterations,
                 max_iterations,
                 mean_iterations,
                 bler,
                 sinr_stats.get_min(),
                 sinr_stats.get_max(),
                 sinr_stats.get_mean(),
                 evm_stats.get_min(),
                 evm_stats.get_max(),
                 evm_stats.get_mean(),
                 ta_stats_us.get_min(),
                 ta_stats_us.get_max(),
                 ta_stats_us.get_mean());
    }
  }

  // Calculate resultant metrics.
  double bler            = static_cast<double>(error_count) / static_cast<double>(count);
  double mean_iterations = static_cast<double>(count_iterations) / static_cast<double>(count * nof_codeblocks);

  // Print results.
  if (show_stats) {
    fmt::print("Iterations={{{:<2} {:<2} {:<3.1f}}}; "
               "BLER={:.10f}; "
               "SINR={{{:+.2f} {:+.2f} {:+.2f}}}; "
               "EVM={{{:.3f} {:.3f} {:.3f}}}; "
               "TA={{{:.2f} {:.2f} {:.2f}}}us;\n",
               min_iterations,
               max_iterations,
               mean_iterations,
               bler,
               sinr_stats.get_min(),
               sinr_stats.get_max(),
               sinr_stats.get_mean(),
               evm_stats.get_min(),
               evm_stats.get_max(),
               evm_stats.get_mean(),
               ta_stats_us.get_min(),
               ta_stats_us.get_max(),
               ta_stats_us.get_mean());
  }
}

static const std::vector<pxsch_bler_params> test_cases = {
    {"TDLA", 60.0, 1, 27, prb_interval{bwp_start_rb, bwp_size_rb}},
    {"TDLA", 60.0, 2, 27, prb_interval{bwp_start_rb, bwp_size_rb}},
    {"TDLA", 60.0, 4, 27, prb_interval{bwp_start_rb, bwp_size_rb}},
    {"TDLB", 60.0, 1, 10, prb_interval{bwp_start_rb, bwp_size_rb}},
    {"TDLB", 60.0, 2, 27, prb_interval{bwp_start_rb, bwp_size_rb}},
    {"TDLB", 60.0, 4, 27, prb_interval{bwp_start_rb, bwp_size_rb}},
    {"TDLC", 60.0, 1, 8, prb_interval{bwp_start_rb, bwp_size_rb}},
    {"TDLC", 60.0, 2, 12, prb_interval{bwp_start_rb, bwp_size_rb}},
    {"TDLC", 60.0, 4, 19, prb_interval{bwp_start_rb, bwp_size_rb}}};

INSTANTIATE_TEST_SUITE_P(PxschBlertest, PxschBlerTestFixture, ::testing::ValuesIn(test_cases));

int main(int argc, char** argv)
{
  // If the test is called from CTest, it might have more than one command line argument. In this case, disable the
  // partial results and reduce the number of repetitions.
  if (argc > 1) {
    show_stats      = false;
    nof_repetitions = 4;
    max_nof_threads = 2;
  }

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}