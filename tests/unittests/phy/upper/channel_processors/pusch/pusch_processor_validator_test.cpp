/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/phy/upper/channel_processors/pusch/factories.h"
#include "srsran/phy/upper/channel_processors/pusch/formatters.h"
#include "srsran/phy/upper/equalization/equalization_factories.h"
#include "srsran/support/executors/inline_task_executor.h"
#include "fmt/ostream.h"
#include "gtest/gtest.h"
#include <regex>

using namespace srsran;

namespace {

/// Maximum number of layers the PUSCH processor supports.
constexpr unsigned max_supported_nof_layers = 2;
/// Default DM-RS symbol mask within the slot.
const symbol_slot_mask dmrs_symbol_mask =
    {false, false, true, false, false, false, false, false, false, false, false, true, false, false};
/// Default valid PUSCH processor configuration.
const pusch_processor::pdu_t base_pdu = {.context          = std::nullopt,
                                         .slot             = {0, 9},
                                         .rnti             = 8323,
                                         .bwp_size_rb      = 25,
                                         .bwp_start_rb     = 0,
                                         .cp               = cyclic_prefix::NORMAL,
                                         .mcs_descr        = {modulation_scheme::PI_2_BPSK, 0.1},
                                         .codeword         = {{0, ldpc_base_graph_type::BG2, true}},
                                         .uci              = {0, 1, uci_part2_size_description(1), 1, 20, 6.25, 6.25},
                                         .n_id             = 935,
                                         .nof_tx_layers    = 1,
                                         .rx_ports         = {0, 1, 2, 3},
                                         .dmrs_symbol_mask = dmrs_symbol_mask,
                                         .dmrs       = pusch_processor::dmrs_configuration{.dmrs          = dmrs_type::TYPE1,
                                                                                           .scrambling_id = 0,
                                                                                           .n_scid        = false,
                                                                                           .nof_cdm_groups_without_data = 2},
                                         .freq_alloc = rb_allocation::make_type1(15, 1),
                                         .start_symbol_index = 0,
                                         .nof_symbols        = 14,
                                         .tbs_lbrm           = units::bytes(ldpc::MAX_CODEBLOCK_SIZE / 8),
                                         .dc_position        = std::nullopt};

struct test_case_t {
  std::function<pusch_processor::pdu_t()> get_pdu;
  std::string                             expr;
};

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os, "{}", test_case.get_pdu());
  return os;
}

const std::vector<test_case_t> pusch_processor_validator_test_data = {
    {[] {
       pusch_processor::pdu_t pdu = base_pdu;
       pdu.bwp_size_rb            = MAX_RB + 1;
       return pdu;
     },
     R"(The sum of the BWP start \(i\.e\., 0\) and size \(i\.e\., 276\) exceeds the maximum grid size \(i\.e\., 275 PRB\)\.)"},
    {[] {
       pusch_processor::pdu_t pdu = base_pdu;
       pdu.nof_tx_layers          = max_supported_nof_layers + 1;
       return pdu;
     },
     R"(The number of transmit layers \(i\.e\., 3\) is out of the range \[1\.\.2\]\.)"},
    {[] {
       pusch_processor::pdu_t pdu = base_pdu;
       pdu.rx_ports.emplace_back(pusch_constants::MAX_NOF_RX_PORTS);
       return pdu;
     },
     R"(The number of receive ports \(i\.e\., 5\) exceeds the maximum number of receive ports \(i\.e\., 4\)\.)"},
    {[] {
       pusch_processor::pdu_t pdu = base_pdu;
       pdu.bwp_size_rb            = 1;
       return pdu;
     },
     R"(Invalid BWP configuration, i\.e\., \[0, 1\) for the given RB allocation, i\.e\., \[15, 16\)\.)"},
    {[] {
       pusch_processor::pdu_t pdu                                      = base_pdu;
       pdu.uci.nof_csi_part1                                           = 0;
       pdu.uci.csi_part2_size.entries.front().parameters.front().width = 10;
       return pdu;
     },
     R"(CSI Part 1 UCI field length \(i\.e\., 0\) does not correspond with the CSI Part 2 \(i\.e\., entries=\[params=\[offset=0 width=10\] map=\[1\]\]\)\.)"},
    {[] {
       pusch_processor::pdu_t pdu = base_pdu;
       pdu.dmrs_symbol_mask       = {true};
       return pdu;
     },
     R"(The DM-RS symbol mask size \(i\.e\., 1\) must be equal to the slot size \(i\.e\., 14\)\.)"},

    {[] {
       pusch_processor::pdu_t pdu = base_pdu;
       pdu.dmrs_symbol_mask       = symbol_slot_mask(get_nsymb_per_slot(pdu.cp));
       return pdu;
     },
     R"(The number of OFDM symbols carrying DM-RS must be greater than zero\.)"},
    {[] {
       pusch_processor::pdu_t pdu = base_pdu;
       pdu.dmrs_symbol_mask       = symbol_slot_mask(get_nsymb_per_slot(pdu.cp));
       pdu.dmrs_symbol_mask.set(0);
       pdu.start_symbol_index = 1;
       pdu.nof_symbols        = 13;
       return pdu;
     },
     R"(The index of the first OFDM symbol carrying DM-RS \(i\.e\., 0\) must be equal to or greater than the first symbol allocated to transmission \(i\.e\., 1\)\.)"},
    {[] {
       pusch_processor::pdu_t pdu = base_pdu;
       pdu.dmrs_symbol_mask       = symbol_slot_mask(get_nsymb_per_slot(pdu.cp));
       pdu.dmrs_symbol_mask.set(13);
       pdu.dmrs_symbol_mask.set(0);
       pdu.start_symbol_index = 0;
       pdu.nof_symbols        = 13;
       return pdu;
     },
     R"(The index of the last OFDM symbol carrying DM-RS \(i\.e\., 13\) must be less than or equal to the last symbol allocated to transmission \(i\.e\., 12\)\.)"},
    {[] {
       pusch_processor::pdu_t pdu = base_pdu;
       pdu.start_symbol_index     = 0;
       pdu.nof_symbols            = 15;
       return pdu;
     },
     R"(The symbol allocation \(i\.e\., \[0, 15\)\) exceeds the slot size \(i\.e\., 14\)\.)"},
    {[] {
       pusch_processor::pdu_t pdu                                   = base_pdu;
       std::get<pusch_processor::dmrs_configuration>(pdu.dmrs).dmrs = dmrs_type::TYPE2;
       return pdu;
     },
     R"(Only DM-RS Type 1 is currently supported\.)"},
    {[] {
       pusch_processor::pdu_t pdu                                                          = base_pdu;
       std::get<pusch_processor::dmrs_configuration>(pdu.dmrs).nof_cdm_groups_without_data = 1;
       return pdu;
     },
     R"(Only two CDM groups without data are currently supported\.)"},
    {[] {
       pusch_processor::pdu_t pdu = base_pdu;
       pdu.tbs_lbrm               = units::bytes(0);
       return pdu;
     },
     R"(Invalid LBRM size \(0 bytes\)\.)"},
    {[] {
       pusch_processor::pdu_t pdu = base_pdu;
       pdu.dc_position            = MAX_RB * NRE;
       return pdu;
     },
     R"(DC position \(i\.e\., 3300\) is out of range \[0\.\.3300\)\.)"},
    {[] {
       pusch_processor::pdu_t pdu = base_pdu;
       pdu.dmrs                   = pusch_processor::dmrs_transform_precoding_configuration{.n_rs_id = 0};
       pdu.nof_tx_layers          = 2;
       return pdu;
     },
     R"(Transform precoding is only possible with one layer\.)"},
    {[] {
       pusch_processor::pdu_t pdu = base_pdu;
       pdu.dmrs                   = pusch_processor::dmrs_transform_precoding_configuration{.n_rs_id = 0};
       pdu.freq_alloc             = rb_allocation::make_custom({0, 2});
       return pdu;
     },
     R"(Transform precoding is only possible with contiguous allocations\.)"},
    {[] {
       pusch_processor::pdu_t pdu = base_pdu;
       pdu.dmrs                   = pusch_processor::dmrs_transform_precoding_configuration{.n_rs_id = 0};
       pdu.freq_alloc             = rb_allocation::make_type1(0, 7);
       pdu.nof_tx_layers          = 1;
       return pdu;
     },
     R"(Transform precoding is only possible with a valid number of PRB\.)"},
    {[] {
       pusch_processor::pdu_t pdu = base_pdu;
       pdu.uci.nof_harq_ack       = 12;
       return pdu;
     },
     R"(UCI field sizes in bits \(12, 1\), exceed the maximum bit size, i\.e\., 11\.)"},
    {[] {
       pusch_processor::pdu_t pdu = base_pdu;
       pdu.uci.nof_csi_part1      = 12;
       return pdu;
     },
     R"(UCI field sizes in bits \(0, 12\), exceed the maximum bit size, i\.e\., 11\.)"},
};

class PuschProcessorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  static std::unique_ptr<pusch_processor>     pusch_proc;
  static std::unique_ptr<pusch_pdu_validator> pdu_validator;
  static inline_task_executor                 ch_est_executor;

  static void SetUpTestSuite()
  {
    if (pusch_proc && pdu_validator) {
      return;
    }

    // Create pseudo-random sequence generator.
    std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
    ASSERT_NE(prg_factory, nullptr);

    // Create low-PAPR sequence generator.
    std::shared_ptr<low_papr_sequence_generator_factory> low_papr_sequence_gen_factory =
        create_low_papr_sequence_generator_sw_factory();
    ASSERT_NE(low_papr_sequence_gen_factory, nullptr);

    // Create demodulator mapper factory.
    std::shared_ptr<demodulation_mapper_factory> chan_demodulation_factory = create_demodulation_mapper_factory();
    ASSERT_NE(chan_demodulation_factory, nullptr);

    // Create CRC calculator factory.
    std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
    ASSERT_NE(crc_calc_factory, nullptr) << "Cannot create CRC calculator factory.";

    // Create LDPC decoder factory.
    std::shared_ptr<ldpc_decoder_factory> ldpc_dec_factory = create_ldpc_decoder_factory_sw("generic");
    ASSERT_NE(ldpc_dec_factory, nullptr);

    // Create LDPC rate dematcher factory.
    std::shared_ptr<ldpc_rate_dematcher_factory> ldpc_rm_factory = create_ldpc_rate_dematcher_factory_sw("auto");
    ASSERT_NE(ldpc_rm_factory, nullptr);

    // Create LDPC desegmenter factory.
    std::shared_ptr<ldpc_segmenter_rx_factory> ldpc_segm_rx_factory = create_ldpc_segmenter_rx_factory_sw();
    ASSERT_NE(ldpc_segm_rx_factory, nullptr);

    // Create short block detector factory.
    std::shared_ptr<short_block_detector_factory> short_block_det_factory = create_short_block_detector_factory_sw();
    ASSERT_NE(short_block_det_factory, nullptr) << "Cannot create short block detector factory.";

    std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_slow();
    if (!dft_factory) {
      dft_factory = create_dft_processor_factory_generic();
    }
    ASSERT_NE(dft_factory, nullptr) << "Cannot create DFT factory.";

    std::shared_ptr<time_alignment_estimator_factory> ta_estimator_factory =
        create_time_alignment_estimator_dft_factory(dft_factory);
    ASSERT_NE(ta_estimator_factory, nullptr) << "Cannot create TA estimator factory.";

    // Create port channel estimator factory.
    std::shared_ptr<port_channel_estimator_factory> port_chan_estimator_factory =
        create_port_channel_estimator_factory_sw(ta_estimator_factory);
    ASSERT_NE(port_chan_estimator_factory, nullptr);

    // Create DM-RS for PUSCH channel estimator.
    std::shared_ptr<dmrs_pusch_estimator_factory> dmrs_pusch_chan_estimator_factory =
        create_dmrs_pusch_estimator_factory_sw(prg_factory,
                                               low_papr_sequence_gen_factory,
                                               port_chan_estimator_factory,
                                               ch_est_executor,
                                               port_channel_estimator_fd_smoothing_strategy::filter,
                                               port_channel_estimator_td_interpolation_strategy::average,
                                               true);
    ASSERT_NE(dmrs_pusch_chan_estimator_factory, nullptr);

    // Create channel equalizer factory.
    std::shared_ptr<channel_equalizer_factory> eq_factory = create_channel_equalizer_generic_factory();
    ASSERT_NE(eq_factory, nullptr);

    std::shared_ptr<transform_precoder_factory> precoding_factory =
        create_dft_transform_precoder_factory(dft_factory, MAX_RB);
    ASSERT_NE(precoding_factory, nullptr);

    // Create PUSCH demodulator factory.
    std::shared_ptr<pusch_demodulator_factory> pusch_demod_factory = create_pusch_demodulator_factory_sw(
        eq_factory, precoding_factory, chan_demodulation_factory, nullptr, prg_factory, MAX_RB, false);
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
    pusch_dec_config.nof_prb                                 = MAX_RB;
    pusch_dec_config.nof_layers                              = pusch_constants::MAX_NOF_LAYERS;
    std::shared_ptr<pusch_decoder_factory> pusch_dec_factory = create_pusch_decoder_factory_sw(pusch_dec_config);
    ASSERT_NE(pusch_dec_factory, nullptr);

    // Create polar decoder factory.
    std::shared_ptr<polar_factory> polar_dec_factory = create_polar_factory_sw();
    ASSERT_NE(polar_dec_factory, nullptr) << "Invalid polar decoder factory.";

    // Create UCI decoder factory.
    std::shared_ptr<uci_decoder_factory> uci_dec_factory =
        create_uci_decoder_factory_generic(short_block_det_factory, polar_dec_factory, crc_calc_factory);
    ASSERT_NE(uci_dec_factory, nullptr) << "Cannot create UCI decoder factory.";

    // Create PUSCH processor.
    pusch_processor_factory_sw_configuration pusch_proc_factory_config;
    pusch_proc_factory_config.estimator_factory                    = dmrs_pusch_chan_estimator_factory;
    pusch_proc_factory_config.demodulator_factory                  = pusch_demod_factory;
    pusch_proc_factory_config.demux_factory                        = demux_factory;
    pusch_proc_factory_config.decoder_factory                      = pusch_dec_factory;
    pusch_proc_factory_config.uci_dec_factory                      = uci_dec_factory;
    pusch_proc_factory_config.ch_estimate_dimensions.nof_prb       = MAX_RB;
    pusch_proc_factory_config.ch_estimate_dimensions.nof_symbols   = MAX_NSYMB_PER_SLOT;
    pusch_proc_factory_config.ch_estimate_dimensions.nof_rx_ports  = pusch_constants::MAX_NOF_RX_PORTS;
    pusch_proc_factory_config.ch_estimate_dimensions.nof_tx_layers = max_supported_nof_layers;
    pusch_proc_factory_config.max_nof_concurrent_threads           = 1;
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

  static void TearDownTestSuite()
  {
    pdu_validator.reset();
    pusch_proc.reset();
  }
};

std::unique_ptr<pusch_processor>     PuschProcessorFixture::pusch_proc;
std::unique_ptr<pusch_pdu_validator> PuschProcessorFixture::pdu_validator;
inline_task_executor                 PuschProcessorFixture::ch_est_executor;

TEST_P(PuschProcessorFixture, PuschProcessorValidatortest)
{
  ASSERT_NE(pusch_proc, nullptr);
  ASSERT_NE(pdu_validator, nullptr);

  const test_case_t& param = GetParam();

  // Make sure the configuration is invalid.
  error_type<std::string> validator_out = pdu_validator->is_valid(param.get_pdu());
  ASSERT_FALSE(validator_out.has_value()) << "Validation should fail.";
  ASSERT_TRUE(std::regex_match(validator_out.error(), std::regex(param.expr)))
      << "The assertion message doesn't match the expected pattern.";
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PuschProcessorValidatortest,
                         PuschProcessorFixture,
                         ::testing::ValuesIn(pusch_processor_validator_test_data));

} // namespace
