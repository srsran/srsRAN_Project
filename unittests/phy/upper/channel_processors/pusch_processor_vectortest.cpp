/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../rx_softbuffer_test_doubles.h"
#include "pusch_processor_test_data.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/phy/upper/equalization/equalization_factories.h"
#include "fmt/ostream.h"
#include "gtest/gtest.h"
#include <random>

using namespace srsgnb;

namespace srsgnb {

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os, "rg_nof_rb={} rg_nof_symb={}", test_case.context.rg_nof_rb, test_case.context.rg_nof_symb);
  return os;
}

} // namespace srsgnb

namespace {

using PuschProcessorParams = test_case_t;

class PuschProcessorFixture : public ::testing::TestWithParam<PuschProcessorParams>
{
protected:
  std::unique_ptr<pusch_processor> pusch_proc;

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
    std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw();
    ASSERT_NE(crc_calc_factory, nullptr);

    // Create LDPC decoder factory.
    std::shared_ptr<ldpc_decoder_factory> ldpc_dec_factory = create_ldpc_decoder_factory_sw("generic");
    ASSERT_NE(ldpc_dec_factory, nullptr);

    // Create LDPC rate dematcher factory.
    std::shared_ptr<ldpc_rate_dematcher_factory> ldpc_rm_factory = create_ldpc_rate_dematcher_factory_sw();
    ASSERT_NE(ldpc_rm_factory, nullptr);

    // Create LDPC desegmenter factory.
    std::shared_ptr<ldpc_segmenter_rx_factory> ldpc_segm_rx_factory = create_ldpc_segmenter_rx_factory_sw();
    ASSERT_NE(ldpc_segm_rx_factory, nullptr);

    std::shared_ptr<short_block_detector_factory> short_block_det_factory = create_short_block_detector_factory_sw();
    ASSERT_NE(short_block_det_factory, nullptr);

    // Create port channel estimator factory.
    std::shared_ptr<port_channel_estimator_factory> port_chan_estimator_factory =
        create_port_channel_estimator_factory_sw();
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

    // Create PUSCH demux factory.
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
  }
};

TEST_P(PuschProcessorFixture, PuschProcessorUnittest)
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
  rx_softbuffer_spy softbuffer(
      ldpc::MAX_CODEBLOCK_SIZE,
      ldpc::compute_nof_codeblocks(expected_data.size() * 8, config.codeword.value().ldpc_base_graph));

  // Process PUSCH PDU.
  pusch_processor_result result = pusch_proc->process(data, softbuffer, grid, config);

  // Verify UL-SCH decode results.
  ASSERT_TRUE(result.data.has_value());
  ASSERT_TRUE(result.data.value().tb_crc_ok);
  ASSERT_EQ(expected_data, data);

  // Verify HARQ-ACK result.
  if (config.uci.nof_harq_ack > 0) {
    std::vector<uint8_t> expected_harq_ack = test_case.harq_ack.read();

    ASSERT_EQ(span<const uint8_t>(result.harq_ack.payload), span<const uint8_t>(expected_harq_ack));
    ASSERT_EQ(result.harq_ack.status, uci_status::valid);
  } else {
    ASSERT_TRUE(result.harq_ack.payload.empty());
    ASSERT_EQ(result.harq_ack.status, uci_status::unknown);
  }
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PuschProcessorUnittest, PuschProcessorFixture, ::testing::ValuesIn(pusch_processor_test_data));

} // namespace
