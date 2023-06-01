/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "../rx_softbuffer_test_doubles.h"
#include "pusch_processor_result_test_doubles.h"
#include "pusch_processor_test_data.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "srsran/phy/upper/equalization/equalization_factories.h"
#include "fmt/ostream.h"
#include "gtest/gtest.h"

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os, "{}", test_case.context.config);
  return os;
}

} // namespace srsran

namespace {

using PuschProcessorParams = test_case_t;

class PuschProcessorFixture : public ::testing::TestWithParam<PuschProcessorParams>
{
protected:
  std::unique_ptr<pusch_processor>     pusch_proc;
  std::unique_ptr<pusch_pdu_validator> pdu_validator;

  void SetUp() override
  {
    const test_case_context& context = GetParam().context;

    // Create pseudo-random sequence generator.
    std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
    ASSERT_NE(prg_factory, nullptr);

    // Create demodulator mapper factory.
    std::shared_ptr<channel_modulation_factory> chan_modulation_factory = create_channel_modulation_sw_factory();
    ASSERT_NE(chan_modulation_factory, nullptr);

    // Create CRC calculator factory.
    std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
    ASSERT_NE(crc_calc_factory, nullptr);

    // Create LDPC decoder factory.
    std::shared_ptr<ldpc_decoder_factory> ldpc_dec_factory = create_ldpc_decoder_factory_sw("generic");
    ASSERT_NE(ldpc_dec_factory, nullptr);

    // Create LDPC rate dematcher factory.
    std::shared_ptr<ldpc_rate_dematcher_factory> ldpc_rm_factory = create_ldpc_rate_dematcher_factory_sw("auto");
    ASSERT_NE(ldpc_rm_factory, nullptr);

    // Create LDPC desegmenter factory.
    std::shared_ptr<ldpc_segmenter_rx_factory> ldpc_segm_rx_factory = create_ldpc_segmenter_rx_factory_sw();
    ASSERT_NE(ldpc_segm_rx_factory, nullptr);

    std::shared_ptr<short_block_detector_factory> short_block_det_factory = create_short_block_detector_factory_sw();
    ASSERT_NE(short_block_det_factory, nullptr);

    std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_slow();
    if (!dft_factory) {
      dft_factory = create_dft_processor_factory_generic();
    }
    ASSERT_NE(dft_factory, nullptr) << "Cannot create DFT factory.";

    // Create port channel estimator factory.
    std::shared_ptr<port_channel_estimator_factory> port_chan_estimator_factory =
        create_port_channel_estimator_factory_sw(dft_factory);
    ASSERT_NE(port_chan_estimator_factory, nullptr);

    // Create DM-RS for PUSCH channel estimator.
    std::shared_ptr<dmrs_pusch_estimator_factory> dmrs_pusch_chan_estimator_factory =
        create_dmrs_pusch_estimator_factory_sw(prg_factory, port_chan_estimator_factory);
    ASSERT_NE(dmrs_pusch_chan_estimator_factory, nullptr);

    // Create channel equalizer factory.
    std::shared_ptr<channel_equalizer_factory> eq_factory = create_channel_equalizer_factory_zf();
    ASSERT_NE(eq_factory, nullptr);

    // Create PUSCH demodulator factory.
    std::shared_ptr<pusch_demodulator_factory> pusch_demod_factory =
        create_pusch_demodulator_factory_sw(eq_factory, chan_modulation_factory, prg_factory);
    ASSERT_NE(pusch_demod_factory, nullptr);

    // Create PUSCH demultiplexer factory.
    std::shared_ptr<ulsch_demultiplex_factory> demux_factory = create_ulsch_demultiplex_factory_sw();
    ASSERT_NE(demux_factory, nullptr);

    // Create PUSCH decoder factory.
    pusch_decoder_factory_sw_configuration pusch_dec_config;
    pusch_dec_config.crc_factory                             = crc_calc_factory;
    pusch_dec_config.decoder_factory                         = ldpc_dec_factory;
    pusch_dec_config.dematcher_factory                       = ldpc_rm_factory;
    pusch_dec_config.segmenter_factory                       = ldpc_segm_rx_factory;
    std::shared_ptr<pusch_decoder_factory> pusch_dec_factory = create_pusch_decoder_factory_sw(pusch_dec_config);
    ASSERT_NE(pusch_dec_factory, nullptr);

    // Create UCI decoder factory.
    uci_decoder_factory_sw_configuration uci_dec_factory_config;
    uci_dec_factory_config.decoder_factory               = short_block_det_factory;
    std::shared_ptr<uci_decoder_factory> uci_dec_factory = create_uci_decoder_factory_sw(uci_dec_factory_config);
    ASSERT_NE(uci_dec_factory, nullptr);

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
    std::shared_ptr<pusch_processor_factory> pusch_proc_factory =
        create_pusch_processor_factory_sw(pusch_proc_factory_config);
    ASSERT_NE(pusch_proc_factory, nullptr);

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
  const test_case_t&            test_case = GetParam();
  const test_case_context&      context   = test_case.context;
  const pusch_processor::pdu_t& config    = context.config;

  // Prepare resource grid.
  resource_grid_reader_spy grid;
  grid.write(test_case.grid.read());

  // Read expected data.
  std::vector<uint8_t> expected_data = test_case.sch_data.read();

  // Prepare receive data.
  std::vector<uint8_t> data(expected_data.size());

  // Prepare softbuffer.
  rx_softbuffer_spy softbuffer_spy(ldpc::MAX_CODEBLOCK_SIZE,
                                   ldpc::compute_nof_codeblocks(units::bytes(expected_data.size()).to_bits(),
                                                                config.codeword.value().ldpc_base_graph));

  // Make sure the configuration is valid.
  ASSERT_TRUE(pdu_validator->is_valid(config));

  // Process PUSCH PDU.
  pusch_processor_result_notifier_spy results_notifier;
  pusch_proc->process(data, softbuffer_spy, results_notifier, grid, config);

  // Verify UL-SCH decode results.
  const auto& sch_entries = results_notifier.get_sch_entries();
  ASSERT_FALSE(sch_entries.empty());
  const auto& sch_entry = sch_entries.front();
  ASSERT_TRUE(sch_entry.data.tb_crc_ok);
  ASSERT_EQ(expected_data, data);

  // Skip the rest of the assertions if UCI is not present.
  if ((config.uci.nof_harq_ack == 0) && (config.uci.nof_csi_part1 == 0) && (config.uci.nof_csi_part2 == 0)) {
    return;
  }

  // Extract UCI result.
  const auto& uci_entries = results_notifier.get_uci_entries();
  ASSERT_FALSE(uci_entries.empty());
  const auto& uci_entry = uci_entries.front();

  // Verify HARQ-ACK result.
  if (config.uci.nof_harq_ack > 0) {
    std::vector<uint8_t> expected_harq_ack = test_case.harq_ack.read();

    ASSERT_EQ(span<const uint8_t>(uci_entry.harq_ack.payload), span<const uint8_t>(expected_harq_ack));
    ASSERT_EQ(uci_entry.harq_ack.status, uci_status::valid);
  } else {
    ASSERT_TRUE(uci_entry.harq_ack.payload.empty());
    ASSERT_EQ(uci_entry.harq_ack.status, uci_status::unknown);
  }

  // Verify CSI Part 1 result.
  if (config.uci.nof_csi_part1 > 0) {
    std::vector<uint8_t> expected_csi_part1 = test_case.csi_part1.read();

    ASSERT_EQ(span<const uint8_t>(uci_entry.csi_part1.payload), span<const uint8_t>(expected_csi_part1));
    ASSERT_EQ(uci_entry.csi_part1.status, uci_status::valid);
  } else {
    ASSERT_TRUE(uci_entry.csi_part1.payload.empty());
    ASSERT_EQ(uci_entry.csi_part1.status, uci_status::unknown);
  }
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PuschProcessorVectortest,
                         PuschProcessorFixture,
                         ::testing::ValuesIn(pusch_processor_test_data));

} // namespace
