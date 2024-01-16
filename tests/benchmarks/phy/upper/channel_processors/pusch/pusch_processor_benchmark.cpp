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

#include "../../../lib/phy/upper/rx_buffer_pool_impl.h"
#include "../../../lib/scheduler/support/tbs_calculator.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_processors/pusch/factories.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor_result_notifier.h"
#include "srsran/support/benchmark_utils.h"
#include "srsran/support/complex_normal_random.h"
#include "srsran/support/executors/task_worker_pool.h"
#include "srsran/support/srsran_test.h"
#include "srsran/support/unique_thread.h"
#include <condition_variable>
#include <getopt.h>
#include <mutex>
#include <random>

using namespace srsran;

// A test case consists of a PUSCH PDU configuration and a Transport Block Size.
using test_case_type = std::tuple<pusch_processor::pdu_t, unsigned>;

namespace {

enum class benchmark_modes : unsigned { silent, latency, throughput_total, throughput_thread, all, invalid };

class pusch_processor_result_notifier_adaptor : public pusch_processor_result_notifier
{
public:
  void on_uci(const pusch_processor_result_control& uci) override {}

  void on_sch(const pusch_processor_result_data& sch) override { completed = true; }

  void reset() { completed = false; }

  void wait_for_completion()
  {
    while (!completed.load()) {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }

private:
  std::atomic<bool> completed = {false};
};

} // namespace

const char* to_string(benchmark_modes mode)
{
  switch (mode) {
    case benchmark_modes::silent:
      return "silent";
    case benchmark_modes::latency:
      return "latency";
    case benchmark_modes::throughput_total:
      return "throughput_total";
    case benchmark_modes::throughput_thread:
      return "throughput_thread";
    case benchmark_modes::all:
      return "all";
    case benchmark_modes::invalid:
    default:
      return "invalid";
  }
}

benchmark_modes to_benchmark_mode(const char* string)
{
  for (unsigned mode_i = static_cast<unsigned>(benchmark_modes::silent);
       mode_i != static_cast<unsigned>(benchmark_modes::invalid);
       ++mode_i) {
    benchmark_modes mode = static_cast<benchmark_modes>(mode_i);
    if (strcmp(to_string(mode), string) == 0) {
      return mode;
    }
  }
  return benchmark_modes::invalid;
}

// Maximum number of threads given the CPU hardware.
static const unsigned max_nof_threads = std::thread::hardware_concurrency();

// General test configuration parameters.
static uint64_t                           nof_repetitions             = 10;
static uint64_t                           nof_threads                 = max_nof_threads;
static uint64_t                           batch_size_per_thread       = 100;
static std::string                        selected_profile_name       = "default";
static std::string                        ldpc_decoder_type           = "auto";
static std::string                        rate_dematcher_type         = "auto";
static bool                               enable_evm                  = false;
static benchmark_modes                    benchmark_mode              = benchmark_modes::throughput_total;
static unsigned                           nof_rx_ports                = 1;
static constexpr unsigned                 max_nof_rx_ports            = 4;
static unsigned                           nof_tx_layers               = 1;
static unsigned                           nof_harq_ack                = 0;
static unsigned                           nof_csi_part1               = 0;
static unsigned                           nof_csi_part2               = 0;
static dmrs_type                          dmrs                        = dmrs_type::TYPE1;
static unsigned                           nof_cdm_groups_without_data = 2;
static bounded_bitset<MAX_NSYMB_PER_SLOT> dmrs_symbol_mask =
    {false, false, true, false, false, false, false, false, false, false, false, false, false, false};
static unsigned                                                                          nof_pusch_decoder_threads = 8;
static std::unique_ptr<task_worker_pool<concurrent_queue_policy::locking_mpmc>>          worker_pool = nullptr;
static std::unique_ptr<task_worker_pool_executor<concurrent_queue_policy::locking_mpmc>> executor    = nullptr;

// Thread shared variables.
static std::mutex              mutex_pending_count;
static std::mutex              mutex_finish_count;
static std::condition_variable cvar_count;
static std::atomic<bool>       thread_quit   = {};
static unsigned                pending_count = 0;
static unsigned                finish_count  = 0;

// Test profile structure, initialized with default profile values.
struct test_profile {
  std::string                      name        = "default";
  std::string                      description = "Runs all combinations.";
  subcarrier_spacing               scs         = subcarrier_spacing::kHz15;
  cyclic_prefix                    cp          = cyclic_prefix::NORMAL;
  unsigned                         nof_symbols = get_nsymb_per_slot(cyclic_prefix::NORMAL);
  std::vector<unsigned>            nof_prb_set = {25, 52, 106, 270};
  std::vector<sch_mcs_description> mcs_set     = {{modulation_scheme::QPSK, 120.0F},
                                                  {modulation_scheme::QAM16, 658.0F},
                                                  {modulation_scheme::QAM64, 873.0F},
                                                  {modulation_scheme::QAM256, 948.0F}};
};

// Profile selected during test execution.
static test_profile selected_profile = {};

// Available test profiles.
static const std::vector<test_profile> profile_set = {
    {
        // Use default profile values.
    },

    {"pusch_scs15_5MHz_qpsk_min",
     "Decodes PUSCH with 5 MHz of bandwidth and a 15 kHz SCS, QPSK modulation at minimum code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {25},
     {{modulation_scheme::QPSK, 120.0F}}},

    {"pusch_scs15_5MHz_256qam_max",
     "Decodes PUSCH with 5 MHz of bandwidth and a 15 kHz SCS, 256-QAM modulation at maximum code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {25},
     {{modulation_scheme::QAM256, 948.0F}}},

    {"pusch_scs15_20MHz_qpsk_min",
     "Decodes PUSCH with 20 MHz of bandwidth and a 15 kHz SCS, QPSK modulation at minimum code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {106},
     {{modulation_scheme::QPSK, 120.0F}}},

    {"pusch_scs15_20MHz_16qam_med",
     "Decodes PUSCH with 20 MHz of bandwidth and a 15 kHz SCS, 16-QAM modulation at a medium code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {106},
     {{modulation_scheme::QAM16, 658.0F}}},

    {"pusch_scs15_50MHz_64qam_max",
     "Decodes PUSCH with 50 MHz of bandwidth and a 15 kHz SCS, 64-QAM modulation at a high code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {270},
     {{modulation_scheme::QAM64, 873.0F}}},

    {"pusch_scs15_20MHz_256qam_max",
     "Decodes PUSCH with 20 MHz of bandwidth and a 15 kHz SCS, 256-QAM modulation at maximum code rate",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {106},
     {{modulation_scheme::QAM256, 948.0F}}},

    {"pusch_scs15_50MHz_qpsk_min",
     "Decodes PUSCH with 50 MHz of bandwidth and a 15 kHz SCS, QPSK modulation at minimum code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {270},
     {{modulation_scheme::QPSK, 120.0F}}},

    {"pusch_scs15_50MHz_256qam_max",
     "Decodes PUSCH with 50 MHz of bandwidth and a 15 kHz SCS, 256-QAM modulation at maximum code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {270},
     {{modulation_scheme::QAM256, 948.0F}}},

    {"pusch_scs30_100MHz_256qam_max",
     "Decodes PUSCH with 50 MHz of bandwidth and a 15 kHz SCS, 256-QAM modulation at maximum code rate.",
     subcarrier_spacing::kHz30,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {273},
     {{modulation_scheme::QAM256, 948.0F}}},
};

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-m benchmark mode] [-R repetitions] [-B Batch size per thread] [-T number of threads] [-D "
             "LDPC type] [-M rate "
             "dematcher type] [-P profile]\n",
             prog);
  fmt::print("\t-m Benchmark mode. [Default {}]\n", to_string(benchmark_mode));
  fmt::print("\t\t {:<20}It does not print any result.\n", to_string(benchmark_modes::silent));
  fmt::print("\t\t {:<20}Prints the overall average execution time.\n", to_string(benchmark_modes::latency));
  fmt::print("\t\t {:<20}Prints the total aggregated throughput.\n", to_string(benchmark_modes::throughput_total));
  fmt::print("\t\t {:<20}Prints the average single thread throughput.\n",
             to_string(benchmark_modes::throughput_thread));
  fmt::print("\t\t {:<20}Prints all the previous modes.\n", to_string(benchmark_modes::all));
  fmt::print("\t-R Repetitions [Default {}]\n", nof_repetitions);
  fmt::print("\t-B Batch size [Default {}]\n", batch_size_per_thread);
  fmt::print("\t-T Number of threads [Default {}, max. {}]\n", nof_threads, max_nof_threads);
  fmt::print("\t-t Number of concurrent PUSCH decoder threads. Set to zero for no concurrency. [Default {}, max. {}]\n",
             nof_pusch_decoder_threads,
             max_nof_threads);
  fmt::print("\t-D LDPC decoder type. [Default {}]\n", ldpc_decoder_type);
  fmt::print("\t-p Number of RX ports. [Default {}, max. {}]\n", nof_rx_ports, max_nof_rx_ports);
  fmt::print("\t-M Rate dematcher type. [Default {}]\n", rate_dematcher_type);
  fmt::print("\t-E Toggle EVM enable/disable. [Default {}]\n", enable_evm ? "enable" : "disable");
  fmt::print("\t-P Benchmark profile. [Default {}]\n", selected_profile_name);
  for (const test_profile& profile : profile_set) {
    fmt::print("\t\t {:<30}{}\n", profile.name, profile.description);
  }
  fmt::print("\t-h Show this message\n");
}

static int parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:T:t:p:B:D:M:EP:m:h")) != -1) {
    switch (opt) {
      case 'R':
        nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'T':
        nof_threads = std::min(max_nof_threads, static_cast<unsigned>(std::strtol(optarg, nullptr, 10)));
        break;
      case 't':
        nof_pusch_decoder_threads = std::min(max_nof_threads, static_cast<unsigned>(std::strtol(optarg, nullptr, 10)));
        break;
      case 'p':
        nof_rx_ports = std::min(max_nof_rx_ports, static_cast<unsigned>(std::strtol(optarg, nullptr, 10)));
        break;
      case 'B':
        batch_size_per_thread = std::strtol(optarg, nullptr, 10);
        break;
      case 'D':
        ldpc_decoder_type = std::string(optarg);
        break;
      case 'M':
        rate_dematcher_type = std::string(optarg);
        break;
      case 'E':
        enable_evm = !enable_evm;
        break;
      case 'P':
        selected_profile_name = std::string(optarg);
        break;
      case 'm':
        benchmark_mode = to_benchmark_mode(optarg);
        if (benchmark_mode == benchmark_modes::invalid) {
          fmt::print(stderr, "Invalid benchmark mode '{}'\n", optarg);
          usage(argv[0]);
          return -1;
        }
        break;
      case 'h':
      default:
        usage(argv[0]);
        exit(0);
    }
  }

  // Search profile.
  bool profile_found = false;
  for (const auto& candidate_profile : profile_set) {
    if (selected_profile_name == candidate_profile.name) {
      selected_profile = candidate_profile;
      srslog::fetch_basic_logger("TEST").info("Loading profile: {}", selected_profile.name);
      profile_found = true;
      break;
    }
  }
  if (!profile_found) {
    usage(argv[0]);
    srslog::fetch_basic_logger("TEST").error("Invalid profile: {}.", selected_profile_name);
    fmt::print(stderr, "Invalid profile: {}.\n", selected_profile_name);
    return -1;
  }

  return 0;
}

// Generates a set of test cases given a test profile.
static std::vector<test_case_type> generate_test_cases(const test_profile& profile)
{
  std::vector<test_case_type> test_case_set;

  for (sch_mcs_description mcs : profile.mcs_set) {
    for (unsigned nof_prb : profile.nof_prb_set) {
      // Determine the Transport Block Size.
      tbs_calculator_configuration tbs_config = {};
      tbs_config.mcs_descr                    = mcs;
      tbs_config.n_prb                        = nof_prb;
      tbs_config.nof_layers                   = nof_tx_layers;
      tbs_config.nof_symb_sh                  = profile.nof_symbols;
      tbs_config.nof_dmrs_prb = dmrs.nof_dmrs_per_rb() * dmrs_symbol_mask.count() * nof_cdm_groups_without_data;
      unsigned tbs            = tbs_calculator_calculate(tbs_config);

      // Build the PUSCH PDU configuration.
      pusch_processor::pdu_t config = {};
      config.slot                   = slot_point(to_numerology_value(profile.scs), 0);
      config.rnti                   = 1;
      config.bwp_size_rb            = nof_prb;
      config.bwp_start_rb           = 0;
      config.cp                     = profile.cp;
      config.mcs_descr              = mcs;
      config.codeword.emplace(pusch_processor::codeword_description{
          0, get_ldpc_base_graph(mcs.get_normalised_target_code_rate(), units::bits(tbs)), true});
      config.uci.alpha_scaling         = 1.0;
      config.uci.beta_offset_harq_ack  = 5.0;
      config.uci.beta_offset_csi_part1 = 5.0;
      config.uci.beta_offset_csi_part2 = 5.0;
      config.uci.nof_harq_ack          = nof_harq_ack;
      config.uci.nof_csi_part1         = nof_csi_part1;
      config.uci.csi_part2_size        = uci_part2_size_description(nof_csi_part2);
      config.n_id                      = 0;
      config.nof_tx_layers             = nof_tx_layers;
      config.rx_ports.resize(nof_rx_ports);
      std::iota(config.rx_ports.begin(), config.rx_ports.end(), 0);
      config.dmrs_symbol_mask            = dmrs_symbol_mask;
      config.dmrs                        = dmrs;
      config.scrambling_id               = 0;
      config.nof_cdm_groups_without_data = nof_cdm_groups_without_data;
      config.freq_alloc                  = rb_allocation::make_type1(config.bwp_start_rb, nof_prb);
      config.start_symbol_index          = 0;
      config.nof_symbols                 = profile.nof_symbols;
      config.tbs_lbrm_bytes              = ldpc::MAX_CODEBLOCK_SIZE / 8;

      test_case_set.emplace_back(std::tuple<pusch_processor::pdu_t, unsigned>(config, tbs));
    }
  }
  return test_case_set;
}

static pusch_processor_factory& get_pusch_processor_factory()
{
  static std::shared_ptr<pusch_processor_factory> pusch_proc_factory = nullptr;

  if (pusch_proc_factory) {
    return *pusch_proc_factory;
  }

  // Create pseudo-random sequence generator.
  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  // Create demodulator mapper factory.
  std::shared_ptr<channel_modulation_factory> chan_modulation_factory = create_channel_modulation_sw_factory();
  TESTASSERT(chan_modulation_factory);

  // Create CRC calculator factory.
  std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
  TESTASSERT(crc_calc_factory);

  // Create LDPC decoder factory.
  std::shared_ptr<ldpc_decoder_factory> ldpc_dec_factory = create_ldpc_decoder_factory_sw(ldpc_decoder_type);
  TESTASSERT(ldpc_dec_factory);

  // Create LDPC rate dematcher factory.
  std::shared_ptr<ldpc_rate_dematcher_factory> ldpc_rm_factory =
      create_ldpc_rate_dematcher_factory_sw(rate_dematcher_type);
  TESTASSERT(ldpc_rm_factory);

  // Create LDPC desegmenter factory.
  std::shared_ptr<ldpc_segmenter_rx_factory> ldpc_segm_rx_factory = create_ldpc_segmenter_rx_factory_sw();
  TESTASSERT(ldpc_segm_rx_factory);

  std::shared_ptr<short_block_detector_factory> short_block_det_factory = create_short_block_detector_factory_sw();
  TESTASSERT(short_block_det_factory);

  std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_fast();
  if (!dft_factory) {
    dft_factory = create_dft_processor_factory_generic();
  }
  TESTASSERT(dft_factory, "Cannot create DFT factory.");

  // Create port channel estimator factory.
  std::shared_ptr<port_channel_estimator_factory> port_chan_estimator_factory =
      create_port_channel_estimator_factory_sw(dft_factory);
  TESTASSERT(port_chan_estimator_factory);

  // Create DM-RS for PUSCH channel estimator.
  std::shared_ptr<dmrs_pusch_estimator_factory> dmrs_pusch_chan_estimator_factory =
      create_dmrs_pusch_estimator_factory_sw(prg_factory, port_chan_estimator_factory);
  TESTASSERT(dmrs_pusch_chan_estimator_factory);

  // Create channel equalizer factory.
  std::shared_ptr<channel_equalizer_factory> eq_factory = create_channel_equalizer_factory_zf();
  TESTASSERT(eq_factory);

  // Create PUSCH demodulator factory.
  std::shared_ptr<pusch_demodulator_factory> pusch_demod_factory =
      create_pusch_demodulator_factory_sw(eq_factory, chan_modulation_factory, prg_factory, enable_evm);
  TESTASSERT(pusch_demod_factory);

  // Create PUSCH demultiplexer factory.
  std::shared_ptr<ulsch_demultiplex_factory> demux_factory = create_ulsch_demultiplex_factory_sw();
  TESTASSERT(demux_factory);

  // Create
  if (nof_pusch_decoder_threads != 0) {
    worker_pool = std::make_unique<task_worker_pool<concurrent_queue_policy::locking_mpmc>>(
        nof_pusch_decoder_threads, 1024, "decoder", std::chrono::microseconds{0}, os_thread_realtime_priority::max());
    executor = std::make_unique<task_worker_pool_executor<concurrent_queue_policy::locking_mpmc>>(*worker_pool);
  }

  // Create PUSCH decoder factory.
  pusch_decoder_factory_sw_configuration pusch_dec_config;
  pusch_dec_config.crc_factory                             = crc_calc_factory;
  pusch_dec_config.decoder_factory                         = ldpc_dec_factory;
  pusch_dec_config.dematcher_factory                       = ldpc_rm_factory;
  pusch_dec_config.segmenter_factory                       = ldpc_segm_rx_factory;
  pusch_dec_config.nof_pusch_decoder_threads               = nof_threads + nof_pusch_decoder_threads + 1;
  pusch_dec_config.executor                                = executor.get();
  std::shared_ptr<pusch_decoder_factory> pusch_dec_factory = create_pusch_decoder_factory_sw(pusch_dec_config);
  TESTASSERT(pusch_dec_factory);

  // Create polar decoder factory.
  std::shared_ptr<polar_factory> polar_dec_factory = create_polar_factory_sw();
  report_fatal_error_if_not(polar_dec_factory, "Invalid polar decoder factory.");

  // Create UCI decoder factory.
  std::shared_ptr<uci_decoder_factory> uci_dec_factory =
      create_uci_decoder_factory_generic(short_block_det_factory, polar_dec_factory, crc_calc_factory);
  TESTASSERT(uci_dec_factory);

  // Create PUSCH processor.
  pusch_processor_factory_sw_configuration pusch_proc_factory_config;
  pusch_proc_factory_config.estimator_factory                    = dmrs_pusch_chan_estimator_factory;
  pusch_proc_factory_config.demodulator_factory                  = pusch_demod_factory;
  pusch_proc_factory_config.demux_factory                        = demux_factory;
  pusch_proc_factory_config.decoder_factory                      = pusch_dec_factory;
  pusch_proc_factory_config.uci_dec_factory                      = uci_dec_factory;
  pusch_proc_factory_config.ch_estimate_dimensions.nof_prb       = MAX_RB;
  pusch_proc_factory_config.ch_estimate_dimensions.nof_symbols   = MAX_NSYMB_PER_SLOT;
  pusch_proc_factory_config.ch_estimate_dimensions.nof_rx_ports  = nof_rx_ports;
  pusch_proc_factory_config.ch_estimate_dimensions.nof_tx_layers = nof_tx_layers;
  pusch_proc_factory_config.dec_nof_iterations                   = 2;
  pusch_proc_factory_config.dec_enable_early_stop                = true;
  pusch_proc_factory_config.max_nof_concurrent_threads           = nof_threads;
  pusch_proc_factory = create_pusch_processor_factory_sw(pusch_proc_factory_config);
  TESTASSERT(pusch_proc_factory);

  pusch_proc_factory = create_pusch_processor_pool(std::move(pusch_proc_factory), nof_threads);
  TESTASSERT(pusch_proc_factory);

  return *pusch_proc_factory;
}

// Instantiates the PUSCH processor and validator.
static std::tuple<std::unique_ptr<pusch_processor>, std::unique_ptr<pusch_pdu_validator>> create_processor()
{
  pusch_processor_factory& pusch_proc_factory = get_pusch_processor_factory();

  // Create PUSCH processor.
  std::unique_ptr<pusch_processor> processor = pusch_proc_factory.create();
  TESTASSERT(processor);

  // Create PUSCH processor validator.
  std::unique_ptr<pusch_pdu_validator> validator = pusch_proc_factory.create_validator();
  TESTASSERT(validator);

  return std::make_tuple(std::move(processor), std::move(validator));
}

static void thread_process(pusch_processor&              proc,
                           const pusch_processor::pdu_t& config,
                           unsigned                      tbs,
                           const resource_grid_reader&   grid)
{
  pusch_processor_result_notifier_adaptor result_notifier;

  // Compute the number of codeblocks.
  unsigned nof_codeblocks = ldpc::compute_nof_codeblocks(units::bits(tbs), config.codeword.value().ldpc_base_graph);

  // Buffer pool configuration.
  rx_buffer_pool_config buffer_pool_config = {};
  buffer_pool_config.nof_buffers           = 1;
  buffer_pool_config.max_nof_codeblocks    = nof_codeblocks;
  buffer_pool_config.max_codeblock_size    = ldpc::MAX_CODEBLOCK_SIZE;
  buffer_pool_config.expire_timeout_slots =
      100 * get_nof_slots_per_subframe(to_subcarrier_spacing(config.slot.numerology()));
  buffer_pool_config.external_soft_bits = false;

  trx_buffer_identifier buffer_id = trx_buffer_identifier(config.rnti, 0);

  // Create buffer pool.
  std::unique_ptr<rx_buffer_pool> buffer_pool = create_rx_buffer_pool(buffer_pool_config);

  // Prepare receive data buffer.
  std::vector<uint8_t> data(tbs / 8);

  // Notify finish count.
  {
    std::unique_lock<std::mutex> lock(mutex_finish_count);
    finish_count++;
    cvar_count.notify_all();
  }

  while (!thread_quit) {
    // Wait for pending.
    {
      std::unique_lock<std::mutex> lock(mutex_pending_count);
      while (pending_count == 0) {
        cvar_count.wait_until(lock, std::chrono::system_clock::now() + std::chrono::milliseconds(2));

        // Quit if signaled.
        if (thread_quit) {
          return;
        }
      }
      pending_count--;
    }

    // Reserve buffer.
    unique_rx_buffer rm_buffer = buffer_pool->reserve(config.slot, buffer_id, nof_codeblocks);

    // Reset notifier.
    result_notifier.reset();

    // Process PDU.
    if (executor) {
      if (not executor->execute([&proc, &data, &rm_buffer, &result_notifier, &grid, config]() {
            proc.process(data, std::move(rm_buffer), result_notifier, grid, config);
          })) {
        fmt::print("Failed to enqueue task.\n");
      }
    } else {
      proc.process(data, std::move(rm_buffer), result_notifier, grid, config);
    }

    // Wait for finish the task.
    result_notifier.wait_for_completion();

    // Notify finish count.
    {
      std::unique_lock<std::mutex> lock(mutex_finish_count);
      finish_count++;
      cvar_count.notify_all();
    }
  }
}

// Creates a resource grid.
static std::unique_ptr<resource_grid> create_resource_grid(unsigned nof_ports, unsigned nof_symbols, unsigned nof_subc)
{
  std::shared_ptr<channel_precoder_factory> precoding_factory = create_channel_precoder_factory("generic");
  TESTASSERT(precoding_factory != nullptr, "Invalid channel precoder factory.");
  std::shared_ptr<resource_grid_factory> rg_factory = create_resource_grid_factory(precoding_factory);
  TESTASSERT(rg_factory != nullptr, "Invalid resource grid factory.");

  return rg_factory->create(nof_ports, nof_symbols, nof_subc);
}

int main(int argc, char** argv)
{
  int ret = parse_args(argc, argv);
  if (ret < 0) {
    return ret;
  }

  // Inform of the benchmark configuration.
  if (benchmark_mode != benchmark_modes::silent) {
    fmt::print("Launching benchmark for {} threads, {} times per thread, and {} repetitions. Using {} profile, {} LDPC "
               "decoder, and {} rate dematcher.\n",
               nof_threads,
               batch_size_per_thread,
               nof_repetitions,
               selected_profile_name,
               ldpc_decoder_type,
               rate_dematcher_type);
  }

  benchmarker perf_meas("PUSCH processor", nof_repetitions);

  // Pseudo-random generator.
  std::mt19937 rgen(0);

  // Grid dimensions for all test cases.
  unsigned grid_nof_symbols = get_nsymb_per_slot(selected_profile.cp);
  unsigned grid_nof_subcs   = MAX_RB * NRE;

  // Create resource grid.
  std::unique_ptr<resource_grid> grid = create_resource_grid(nof_rx_ports, grid_nof_symbols, grid_nof_subcs);
  TESTASSERT(grid);

  unsigned nof_grid_re = grid_nof_subcs * grid_nof_symbols * nof_rx_ports;

  // Create a vector to hold the randomly generated RE.
  std::vector<cf_t> random_re(nof_grid_re);

  // Standard complex normal distribution with zero mean.
  complex_normal_distribution<cf_t> c_normal_dist = {};

  // Generate random RE.
  std::generate(random_re.begin(), random_re.end(), [&rgen, &c_normal_dist]() { return c_normal_dist(rgen); });

  // Generate a RE mask and set all elements to true.
  bounded_bitset<NRE* MAX_RB> re_mask = ~bounded_bitset<NRE * MAX_RB>(grid_nof_subcs);

  // Fill the grid with the random RE.
  span<const cf_t> re_view(random_re);
  for (unsigned i_rx_port = 0; i_rx_port != nof_rx_ports; ++i_rx_port) {
    for (unsigned i_symbol = 0; i_symbol != grid_nof_symbols; ++i_symbol) {
      re_view = grid->get_writer().put(i_rx_port, i_symbol, 0, re_mask, re_view);
    }
  }

  // Create processor and validator.
  std::unique_ptr<pusch_processor>     processor;
  std::unique_ptr<pusch_pdu_validator> validator;
  std::tie(processor, validator) = create_processor();

  // Generate the test cases.
  std::vector<test_case_type> test_case_set = generate_test_cases(selected_profile);

  for (const test_case_type& test_case : test_case_set) {
    // Get the PUSCH configuration.
    const pusch_processor::pdu_t& config = std::get<0>(test_case);
    // Get the TBS in bits.
    unsigned tbs = std::get<1>(test_case);

    // Make sure the configuration is valid.
    TESTASSERT(validator->is_valid(config));

    // Reset finish counter.
    finish_count = 0;
    thread_quit  = false;

    // Prepare threads for the current case.
    std::vector<unique_thread> threads(nof_threads);
    for (unsigned thread_id = 0; thread_id != nof_threads; ++thread_id) {
      // Select thread.
      unique_thread& thread = threads[thread_id];

      // Prepare priority.
      os_thread_realtime_priority prio = os_thread_realtime_priority::no_realtime() + 1;

      // Prepare affinity mask.
      os_sched_affinity_bitmask cpuset;
      cpuset.set(thread_id);

      // Create thread.
      thread = unique_thread(
          "thread_" + std::to_string(thread_id), prio, cpuset, [&proc = *processor, &config, &tbs, &grid] {
            thread_process(proc, config, tbs, grid.get()->get_reader());
          });
    }

    // Wait for finish thread init.
    {
      std::unique_lock<std::mutex> lock(mutex_finish_count);
      while (finish_count != nof_threads) {
        cvar_count.wait_until(lock, std::chrono::system_clock::now() + std::chrono::milliseconds(2));
      }
    }

    // Calculate the peak throughput, considering that the number of bits is for a slot.
    double slot_duration_us     = 1e3 / static_cast<double>(pow2(config.slot.numerology()));
    double peak_throughput_Mbps = static_cast<double>(tbs) / slot_duration_us;

    // Measurement description.
    fmt::memory_buffer meas_description;
    fmt::format_to(meas_description,
                   "PUSCH RB={:<3} Mod={:<6} R={:<5.3f} - {:>5.1f} Mbps",
                   config.freq_alloc.get_nof_rb(),
                   to_string(config.mcs_descr.modulation),
                   config.mcs_descr.get_normalised_target_code_rate(),
                   peak_throughput_Mbps);

    // Run the benchmark.
    perf_meas.new_measure(to_string(meas_description), nof_threads * batch_size_per_thread * tbs, []() {
      // Notify start.
      {
        std::unique_lock<std::mutex> lock(mutex_pending_count);
        pending_count = nof_threads * batch_size_per_thread;
        finish_count  = 0;
        cvar_count.notify_all();
      }

      // Wait for finish.
      {
        std::unique_lock<std::mutex> lock(mutex_finish_count);
        while (finish_count != (nof_threads * batch_size_per_thread)) {
          cvar_count.wait_until(lock, std::chrono::system_clock::now() + std::chrono::milliseconds(2));
        }
      }
    });

    thread_quit = true;

    for (unique_thread& thread : threads) {
      thread.join();
    }
  }

  // Print latency.
  if ((benchmark_mode == benchmark_modes::latency) || (benchmark_mode == benchmark_modes::all)) {
    fmt::print("\n--- Average latency ---\n");
    perf_meas.print_percentiles_time("microseconds", 1e-3 / static_cast<double>(nof_threads * batch_size_per_thread));
  }

  // Print total aggregated throughput.
  if ((benchmark_mode == benchmark_modes::throughput_total) || (benchmark_mode == benchmark_modes::all)) {
    fmt::print("\n--- Total throughput ---\n");
    perf_meas.print_percentiles_throughput("bits");
  }

  // Print average throughput per thread.
  if ((benchmark_mode == benchmark_modes::throughput_thread) || (benchmark_mode == benchmark_modes::all)) {
    fmt::print("\n--- Thread throughput ---\n");
    perf_meas.print_percentiles_throughput("bits", 1.0 / static_cast<double>(nof_threads));
  }

  if (worker_pool) {
    worker_pool->stop();
  }

  return 0;
}
