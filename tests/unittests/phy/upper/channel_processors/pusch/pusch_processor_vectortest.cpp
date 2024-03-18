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

#include "../../rx_buffer_test_doubles.h"
#include "pusch_processor_result_test_doubles.h"
#include "pusch_processor_test_data.h"
#include "srsran/phy/upper/channel_processors/pusch/factories.h"
#include "srsran/phy/upper/channel_processors/pusch/formatters.h"
#include "srsran/phy/upper/equalization/equalization_factories.h"
#include "srsran/support/math_utils.h"
#ifdef HWACC_PUSCH_ENABLED
#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#include "srsran/hal/dpdk/bbdev/bbdev_acc_factory.h"
#include "srsran/hal/dpdk/dpdk_eal_factory.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/ext_harq_buffer_context_repository_factory.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_factories.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec_factory.h"
#endif // HWACC_PUSCH_ENABLED
#include "fmt/ostream.h"
#include "gtest/gtest.h"

using namespace srsran;

#ifdef HWACC_PUSCH_ENABLED
static bool skip_hwacc_test = false;
#endif // HWACC_PUSCH_ENABLED

namespace srsran {

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os, "{}", test_case.context.config);
  return os;
}

std::ostream& operator<<(std::ostream& os, const span<const uint8_t>& data)
{
  fmt::print(os, "[{}]", data);
  return os;
}

} // namespace srsran

namespace {

using PuschProcessorParams = std::tuple<std::string, test_case_t>;

class PuschProcessorFixture : public ::testing::TestWithParam<PuschProcessorParams>
{
private:
  static std::shared_ptr<pusch_decoder_factory>
  create_generic_pusch_decoder_factory(std::shared_ptr<crc_calculator_factory> crc_calculator_factory)
  {
    std::shared_ptr<ldpc_decoder_factory> ldpc_decoder_factory = create_ldpc_decoder_factory_sw("auto");
    if (!ldpc_decoder_factory) {
      return nullptr;
    }

    std::shared_ptr<ldpc_rate_dematcher_factory> ldpc_rate_dematcher_factory =
        create_ldpc_rate_dematcher_factory_sw("auto");
    if (!ldpc_rate_dematcher_factory) {
      return nullptr;
    }

    std::shared_ptr<ldpc_segmenter_rx_factory> segmenter_rx_factory = create_ldpc_segmenter_rx_factory_sw();
    if (!segmenter_rx_factory) {
      return nullptr;
    }

    pusch_decoder_factory_sw_configuration pusch_decoder_factory_sw_config;
    pusch_decoder_factory_sw_config.crc_factory       = crc_calculator_factory;
    pusch_decoder_factory_sw_config.decoder_factory   = ldpc_decoder_factory;
    pusch_decoder_factory_sw_config.dematcher_factory = ldpc_rate_dematcher_factory;
    pusch_decoder_factory_sw_config.segmenter_factory = segmenter_rx_factory;
    pusch_decoder_factory_sw_config.nof_prb           = MAX_RB;
    pusch_decoder_factory_sw_config.nof_layers        = pusch_constants::MAX_NOF_LAYERS;

    return create_pusch_decoder_factory_sw(pusch_decoder_factory_sw_config);
  }

  static std::shared_ptr<hal::hw_accelerator_pusch_dec_factory> create_hw_accelerator_pusch_dec_factory()
  {
#ifdef HWACC_PUSCH_ENABLED
    // Hardcoded stdout and error logging.
    srslog::sink* log_sink = srslog::create_stdout_sink();
    srslog::set_default_sink(*log_sink);
    srslog::init();
    srslog::basic_logger& logger = srslog::fetch_basic_logger("HAL", false);
    logger.set_level(srslog::str_to_basic_level("error"));

    // Pointer to a dpdk-based hardware-accelerator interface.
    static std::unique_ptr<dpdk::dpdk_eal> dpdk_interface = nullptr;
    if (!dpdk_interface && !skip_hwacc_test) {
      // :TODO: Enable passing EAL arguments.
      dpdk_interface = dpdk::create_dpdk_eal("pusch_processor_vectortest", logger);
      if (!dpdk_interface) {
        skip_hwacc_test = true;
        return nullptr;
      }
    } else if (skip_hwacc_test) {
      return nullptr;
    }

    // Intefacing to the bbdev-based hardware-accelerator.
    dpdk::bbdev_acc_configuration bbdev_config;
    bbdev_config.id                                    = 0;
    bbdev_config.nof_ldpc_enc_lcores                   = 0;
    bbdev_config.nof_ldpc_dec_lcores                   = 1;
    bbdev_config.nof_fft_lcores                        = 0;
    bbdev_config.nof_mbuf                              = static_cast<unsigned>(pow2(log2_ceil(MAX_NOF_SEGMENTS)));
    std::shared_ptr<dpdk::bbdev_acc> bbdev_accelerator = create_bbdev_acc(bbdev_config, logger);
    if (!bbdev_accelerator || skip_hwacc_test) {
      skip_hwacc_test = true;
      return nullptr;
    }

    // Interfacing to a shared external HARQ buffer context repository.
    unsigned nof_cbs                   = MAX_NOF_SEGMENTS;
    uint64_t acc100_ext_harq_buff_size = bbdev_accelerator->get_harq_buff_size_bytes();
    std::shared_ptr<ext_harq_buffer_context_repository> harq_buffer_context =
        create_ext_harq_buffer_context_repository(nof_cbs, acc100_ext_harq_buff_size, false);
    if (!harq_buffer_context) {
      skip_hwacc_test = true;
      return nullptr;
    }

    // Set the hardware-accelerator configuration.
    hw_accelerator_pusch_dec_configuration hw_decoder_config;
    hw_decoder_config.acc_type            = "acc100";
    hw_decoder_config.bbdev_accelerator   = bbdev_accelerator;
    hw_decoder_config.ext_softbuffer      = true;
    hw_decoder_config.harq_buffer_context = harq_buffer_context;

    // ACC100 hardware-accelerator implementation.
    return hal::create_hw_accelerator_pusch_dec_factory(hw_decoder_config);
#else  // HWACC_PUSCH_ENABLED
    return nullptr;
#endif // HWACC_PUSCH_ENABLED
  }

  static std::shared_ptr<pusch_decoder_factory>
  create_acc100_pusch_decoder_factory(std::shared_ptr<crc_calculator_factory> crc_calculator_factory)
  {
    std::shared_ptr<ldpc_segmenter_rx_factory> segmenter_rx_factory = create_ldpc_segmenter_rx_factory_sw();
    if (!segmenter_rx_factory) {
      return nullptr;
    }

    std::shared_ptr<hal::hw_accelerator_pusch_dec_factory> hw_decoder_factory =
        create_hw_accelerator_pusch_dec_factory();
    if (!hw_decoder_factory) {
      return nullptr;
    }

    // Set the hardware-accelerated PUSCH decoder configuration.
    pusch_decoder_factory_hw_configuration decoder_hw_factory_config;
    decoder_hw_factory_config.segmenter_factory  = segmenter_rx_factory;
    decoder_hw_factory_config.crc_factory        = crc_calculator_factory;
    decoder_hw_factory_config.hw_decoder_factory = hw_decoder_factory;
    return create_pusch_decoder_factory_hw(decoder_hw_factory_config);
  }

  static std::shared_ptr<pusch_decoder_factory>
  create_pusch_decoder_factory(std::shared_ptr<crc_calculator_factory> crc_calculator_factory,
                               const std::string&                      decoder_type)
  {
    if (decoder_type == "generic") {
      return create_generic_pusch_decoder_factory(crc_calculator_factory);
    }

    if (decoder_type == "acc100") {
      return create_acc100_pusch_decoder_factory(crc_calculator_factory);
    }

    return nullptr;
  }

  std::shared_ptr<pusch_processor_factory> create_pusch_processor_factory(const test_case_context& context,
                                                                          const std::string&       decoder_type)
  {
    // Create pseudo-random sequence generator.
    std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
    if (!prg_factory) {
      return nullptr;
    }

    // Create demodulator mapper factory.
    std::shared_ptr<channel_modulation_factory> chan_modulation_factory = create_channel_modulation_sw_factory();
    if (!chan_modulation_factory) {
      return nullptr;
    }

    // Create CRC calculator factory.
    std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
    if (!crc_calc_factory) {
      return nullptr;
    }

    // Create short block detector factory.
    std::shared_ptr<short_block_detector_factory> short_block_det_factory = create_short_block_detector_factory_sw();
    if (!short_block_det_factory) {
      return nullptr;
    }

    std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_slow();
    if (!dft_factory) {
      dft_factory = create_dft_processor_factory_generic();
    }
    if (!dft_factory) {
      return nullptr;
    }

    std::shared_ptr<time_alignment_estimator_factory> ta_estimator_factory =
        create_time_alignment_estimator_dft_factory(dft_factory);
    if (!ta_estimator_factory) {
      return nullptr;
    }

    // Create port channel estimator factory.
    std::shared_ptr<port_channel_estimator_factory> port_chan_estimator_factory =
        create_port_channel_estimator_factory_sw(ta_estimator_factory);
    if (!port_chan_estimator_factory) {
      return nullptr;
    }

    // Create DM-RS for PUSCH channel estimator.
    std::shared_ptr<dmrs_pusch_estimator_factory> dmrs_pusch_chan_estimator_factory =
        create_dmrs_pusch_estimator_factory_sw(prg_factory, port_chan_estimator_factory);
    if (!dmrs_pusch_chan_estimator_factory) {
      return nullptr;
    }

    // Create channel equalizer factory.
    std::shared_ptr<channel_equalizer_factory> eq_factory = create_channel_equalizer_factory_zf();
    if (!eq_factory) {
      return nullptr;
    }

    // Create PUSCH demodulator factory.
    std::shared_ptr<pusch_demodulator_factory> pusch_demod_factory =
        create_pusch_demodulator_factory_sw(eq_factory, chan_modulation_factory, prg_factory, true, true);
    if (!pusch_demod_factory) {
      return nullptr;
    }

    // Create PUSCH demultiplexer factory.
    std::shared_ptr<ulsch_demultiplex_factory> demux_factory = create_ulsch_demultiplex_factory_sw();
    if (!demux_factory) {
      return nullptr;
    }

    // Create PUSCH decoder factory.
    std::shared_ptr<pusch_decoder_factory> pusch_dec_factory =
        create_pusch_decoder_factory(crc_calc_factory, decoder_type);
    if (!pusch_dec_factory) {
      return nullptr;
    }

    // Create polar decoder factory.
    std::shared_ptr<polar_factory> polar_dec_factory = create_polar_factory_sw();
    if (!polar_dec_factory) {
      return nullptr;
    }

    // Create UCI decoder factory.
    std::shared_ptr<uci_decoder_factory> uci_dec_factory =
        create_uci_decoder_factory_generic(short_block_det_factory, polar_dec_factory, crc_calc_factory);
    if (!uci_dec_factory) {
      return nullptr;
    }

    // Create PUSCH processor.
    pusch_processor_factory_sw_configuration pusch_proc_factory_config;
    pusch_proc_factory_config.estimator_factory                    = dmrs_pusch_chan_estimator_factory;
    pusch_proc_factory_config.demodulator_factory                  = pusch_demod_factory;
    pusch_proc_factory_config.demux_factory                        = demux_factory;
    pusch_proc_factory_config.decoder_factory                      = pusch_dec_factory;
    pusch_proc_factory_config.uci_dec_factory                      = uci_dec_factory;
    pusch_proc_factory_config.ch_estimate_dimensions.nof_prb       = context.rg_nof_rb;
    pusch_proc_factory_config.ch_estimate_dimensions.nof_symbols   = context.rg_nof_symb;
    pusch_proc_factory_config.ch_estimate_dimensions.nof_rx_ports  = context.config.rx_ports.size();
    pusch_proc_factory_config.ch_estimate_dimensions.nof_tx_layers = context.config.nof_tx_layers;
    pusch_proc_factory_config.csi_sinr_calc_method       = channel_state_information::sinr_type::post_equalization;
    pusch_proc_factory_config.max_nof_concurrent_threads = 1;
    return create_pusch_processor_factory_sw(pusch_proc_factory_config);
  }

protected:
  std::unique_ptr<pusch_processor>     pusch_proc;
  std::unique_ptr<pusch_pdu_validator> pdu_validator;

  void SetUp() override
  {
    const PuschProcessorParams& param        = GetParam();
    const std::string&          decoder_type = std::get<0>(param);
    const test_case_t&          test_case    = std::get<1>(param);
    const test_case_context&    context      = test_case.context;

    // Create PDSCH processor factory.
    std::shared_ptr<pusch_processor_factory> pusch_proc_factory = create_pusch_processor_factory(context, decoder_type);
#ifdef HWACC_PUSCH_ENABLED
    if (decoder_type == "acc100" && skip_hwacc_test) {
      GTEST_SKIP() << "[WARNING] ACC100 not found. Skipping test.";
    }
#endif // HWACC_PUSCH_ENABLED
    ASSERT_NE(pusch_proc_factory, nullptr) << "Invalid PUSCH processor factory.";

    // Create actual PUSCH processor.
    pusch_proc = pusch_proc_factory->create();
    ASSERT_NE(pusch_proc, nullptr);

    // Create actual PUSCH processor validator.
    pdu_validator = pusch_proc_factory->create_validator();
    ASSERT_NE(pdu_validator, nullptr);
  }
};

TEST_P(PuschProcessorFixture, PuschProcessorVectortest)
{
  const PuschProcessorParams&   param     = GetParam();
  const test_case_t&            test_case = std::get<1>(param);
  const test_case_context&      context   = test_case.context;
  const pusch_processor::pdu_t& config    = context.config;

  // Prepare resource grid.
  resource_grid_reader_spy grid;
  grid.write(test_case.grid.read());

  // Read expected data.
  std::vector<uint8_t> expected_data = test_case.sch_data.read();

  // Prepare receive data.
  std::vector<uint8_t> data(expected_data.size());

  // Prepare buffer.
  rx_buffer_spy    rm_buffer_spy(ldpc::MAX_CODEBLOCK_SIZE,
                              ldpc::compute_nof_codeblocks(units::bytes(expected_data.size()).to_bits(),
                                                           config.codeword.value().ldpc_base_graph));
  unique_rx_buffer rm_buffer(rm_buffer_spy);

  // Make sure the configuration is valid.
  ASSERT_TRUE(pdu_validator->is_valid(config));

  // Process PUSCH PDU.
  pusch_processor_result_notifier_spy results_notifier;
  pusch_proc->process(data, std::move(rm_buffer), results_notifier, grid, config);

  // Verify UL-SCH decode results.
  const auto& sch_entries = results_notifier.get_sch_entries();
  ASSERT_FALSE(sch_entries.empty());
  const auto& sch_entry = sch_entries.front();
  ASSERT_TRUE(sch_entry.data.tb_crc_ok);
  ASSERT_EQ(expected_data, data);

  // Make sure SINR is normal.
  ASSERT_TRUE(std::isnormal(results_notifier.get_sch_entries().front().csi.get_sinr_dB()));

  // Skip the rest of the assertions if UCI is not present.
  if ((config.uci.nof_harq_ack == 0) && (config.uci.nof_csi_part1 == 0) && config.uci.csi_part2_size.entries.empty()) {
    return;
  }

  // Extract UCI result.
  const auto& uci_entries = results_notifier.get_uci_entries();
  ASSERT_FALSE(uci_entries.empty());
  const auto& uci_entry = uci_entries.front();

  // Make sure SINR reported in UCI is normal.
  ASSERT_TRUE(std::isnormal(uci_entry.csi.get_sinr_dB()));

  // Verify HARQ-ACK result.
  if (config.uci.nof_harq_ack > 0) {
    std::vector<uint8_t> expected_harq_ack_unpacked = test_case.harq_ack.read();
    uci_payload_type     expected_harq_ack(expected_harq_ack_unpacked.begin(), expected_harq_ack_unpacked.end());

    ASSERT_EQ(uci_entry.harq_ack.payload, expected_harq_ack);
    ASSERT_EQ(uci_entry.harq_ack.status, uci_status::valid);
  } else {
    ASSERT_TRUE(uci_entry.harq_ack.payload.empty());
    ASSERT_EQ(uci_entry.harq_ack.status, uci_status::unknown);
  }

  // Verify CSI Part 1 result.
  if (config.uci.nof_csi_part1 > 0) {
    std::vector<uint8_t> expected_csi_part1_unpacked = test_case.csi_part1.read();
    uci_payload_type     expected_csi_part1(expected_csi_part1_unpacked.begin(), expected_csi_part1_unpacked.end());

    ASSERT_EQ(uci_entry.csi_part1.payload, expected_csi_part1);
    ASSERT_EQ(uci_entry.csi_part1.status, uci_status::valid);
  } else {
    ASSERT_TRUE(uci_entry.csi_part1.payload.empty());
    ASSERT_EQ(uci_entry.csi_part1.status, uci_status::unknown);
  }
}

TEST_P(PuschProcessorFixture, PuschProcessorVectortestZero)
{
  // Reuses the configurations from the vector test.
  const PuschProcessorParams&   param     = GetParam();
  const test_case_t&            test_case = std::get<1>(param);
  const test_case_context&      context   = test_case.context;
  const pusch_processor::pdu_t& config    = context.config;

  // Read resource grid data and overwrite the RE with zeros.
  std::vector<resource_grid_reader_spy::expected_entry_t> grid_data = test_case.grid.read();
  std::for_each(grid_data.begin(), grid_data.end(), [](auto& e) { e.value = 0; });

  // Prepare resource grid.
  resource_grid_reader_spy grid;
  grid.write(grid_data);

  // Prepare receive data.
  std::vector<uint8_t> data(test_case.sch_data.read().size());

  // Prepare buffer.
  rx_buffer_spy rm_buffer_spy(
      ldpc::MAX_CODEBLOCK_SIZE,
      ldpc::compute_nof_codeblocks(units::bytes(data.size()).to_bits(), config.codeword.value().ldpc_base_graph));
  unique_rx_buffer rm_buffer(rm_buffer_spy);

  // Make sure the configuration is valid.
  ASSERT_TRUE(pdu_validator->is_valid(config));

  // Process PUSCH PDU.
  pusch_processor_result_notifier_spy results_notifier;
  pusch_proc->process(data, std::move(rm_buffer), results_notifier, grid, config);

  // Verify UL-SCH decode results are invalid.
  const auto& sch_entries = results_notifier.get_sch_entries();
  ASSERT_FALSE(sch_entries.empty());
  const auto& sch_entry = sch_entries.front();
  ASSERT_FALSE(sch_entry.data.tb_crc_ok);

  // Make sure SINR is infinity.
  ASSERT_TRUE(std::isinf(results_notifier.get_sch_entries().front().csi.get_sinr_dB()));

  // Skip the rest of the assertions if UCI is not present.
  if ((config.uci.nof_harq_ack == 0) && (config.uci.nof_csi_part1 == 0) && config.uci.csi_part2_size.entries.empty()) {
    return;
  }

  // Extract UCI result.
  const auto& uci_entries = results_notifier.get_uci_entries();
  ASSERT_FALSE(uci_entries.empty());
  const auto& uci_entry = uci_entries.front();

  // Make sure SINR reported in UCI is normal.
  ASSERT_TRUE(std::isinf(uci_entry.csi.get_sinr_dB()));

  // Verify HARQ-ACK result is invalid.
  if (config.uci.nof_harq_ack > 0) {
    uci_payload_type expected_payload = ~uci_payload_type(config.uci.nof_harq_ack);
    ASSERT_EQ(uci_entry.harq_ack.status, uci_status::invalid);
    ASSERT_EQ(uci_entry.harq_ack.payload, expected_payload);
  } else {
    ASSERT_TRUE(uci_entry.harq_ack.payload.empty());
    ASSERT_EQ(uci_entry.harq_ack.status, uci_status::unknown);
  }

  // Verify CSI Part 1 result is invalid.
  if (config.uci.nof_csi_part1 > 0) {
    uci_payload_type expected_payload = ~uci_payload_type(config.uci.nof_csi_part1);
    ASSERT_EQ(uci_entry.csi_part1.status, uci_status::invalid);
    ASSERT_EQ(uci_entry.csi_part1.payload, expected_payload);
  } else {
    ASSERT_TRUE(uci_entry.csi_part1.payload.empty());
    ASSERT_EQ(uci_entry.csi_part1.status, uci_status::unknown);
  }
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PuschProcessorVectortest,
                         PuschProcessorFixture,
#ifdef HWACC_PUSCH_ENABLED
                         testing::Combine(testing::Values("generic", "acc100"),
#else  // HWACC_PUSCH_ENABLED
                         testing::Combine(testing::Values("generic"),
#endif // HWACC_PUSCH_ENABLED
                                          ::testing::ValuesIn(pusch_processor_test_data)));
} // namespace
