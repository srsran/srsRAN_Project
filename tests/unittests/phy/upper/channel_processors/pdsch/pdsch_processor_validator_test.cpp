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

#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "srsran/phy/upper/channel_processors/pdsch/factories.h"
#include "srsran/phy/upper/channel_processors/pdsch/formatters.h"
#include "srsran/ran/dmrs.h"
#include "srsran/ran/precoding/precoding_codebooks.h"
#include "fmt/ostream.h"
#include "gtest/gtest.h"
#include <regex>

using namespace srsran;

namespace {

// Valid PDSCH configuration used as a base for the test cases.
const pdsch_processor::pdu_t base_pdu = {.context                     = std::nullopt,
                                         .slot                        = {0, 19},
                                         .rnti                        = 1,
                                         .bwp_size_rb                 = 52,
                                         .bwp_start_rb                = 0,
                                         .cp                          = cyclic_prefix::NORMAL,
                                         .codewords                   = {{modulation_scheme::QPSK, 0}},
                                         .n_id                        = 1,
                                         .ref_point                   = pdsch_processor::pdu_t::CRB0,
                                         .dmrs_symbol_mask            = {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
                                         .dmrs                        = dmrs_type::TYPE1,
                                         .scrambling_id               = 0,
                                         .n_scid                      = 0,
                                         .nof_cdm_groups_without_data = 1,
                                         .freq_alloc                  = rb_allocation::make_type1(0, 52),
                                         .start_symbol_index          = 2,
                                         .nof_symbols                 = 12,
                                         .ldpc_base_graph             = ldpc_base_graph_type::BG1,
                                         .tbs_lbrm                    = units::bytes(3168),
                                         .reserved                    = {},
                                         .ptrs                        = std::nullopt,
                                         .ratio_pdsch_dmrs_to_sss_dB  = 0,
                                         .ratio_pdsch_data_to_sss_dB  = 0,
                                         .precoding = precoding_configuration::make_wideband(make_single_port())};

struct test_case_t {
  std::function<pdsch_processor::pdu_t()> get_pdu;
  std::string                             expr;
};

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  pdsch_processor::pdu_t pdu = test_case.get_pdu();
  fmt::print(os, "{}", pdu);
  return os;
}

const std::vector<test_case_t> pdsch_processor_validator_test_data = {
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.bwp_size_rb            = MAX_RB + 1;
       return pdu;
     },
     R"(Invalid BWP configuration, i\.e\., \[0, 276\) for the given RB allocation, i\.e\., \[0, 52\)\.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.dmrs_symbol_mask       = {1};
       return pdu;
     },
     R"(The DM-RS symbol mask size \(i\.e\., 1\) must be equal to the slot size \(i\.e\., 14\)\.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.dmrs_symbol_mask       = bounded_bitset<MAX_NSYMB_PER_SLOT>(MAX_NSYMB_PER_SLOT);
       return pdu;
     },
     R"(The number of OFDM symbols carrying DM-RS must be greater than zero\.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.dmrs_symbol_mask       = bounded_bitset<MAX_NSYMB_PER_SLOT>(MAX_NSYMB_PER_SLOT);
       pdu.dmrs_symbol_mask.set(0);
       pdu.start_symbol_index = 1;
       pdu.nof_symbols        = 10;
       return pdu;
     },
     R"(The index of the first OFDM symbol carrying DM-RS \(i\.e\., 0\) must be equal to or greater than the first symbol allocated to transmission \(i\.e\., 1\)\.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.dmrs_symbol_mask       = bounded_bitset<MAX_NSYMB_PER_SLOT>(MAX_NSYMB_PER_SLOT);
       pdu.dmrs_symbol_mask.set(13);
       pdu.start_symbol_index = 0;
       pdu.nof_symbols        = 10;
       return pdu;
     },
     R"(The index of the last OFDM symbol carrying DM-RS \(i\.e\., 13\) must be less than or equal to the last symbol allocated to transmission \(i\.e\., 9\)\.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.start_symbol_index     = 2;
       pdu.nof_symbols            = 13;
       return pdu;
     },
     R"(The symbol allocation \(i\.e\., \[2, 15\)\) exceeds the slot size \(i\.e\., 14\)\.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.dmrs                   = dmrs_type::TYPE2;
       return pdu;
     },
     R"(Only DM-RS Type 1 is currently supported.)"},
    {[] {
       pdsch_processor::pdu_t pdu      = base_pdu;
       pdu.nof_cdm_groups_without_data = get_max_nof_cdm_groups_without_data(dmrs_config_type::type1) + 1;
       return pdu;
     },
     R"(The number of CDM groups without data \(i\.e\., 3\) must not exceed the maximum supported by the DM-RS type \(i\.e\., 2\)\.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.freq_alloc             = rb_allocation::make_type0({1, 0, 1, 0, 1, 0});
       return pdu;
     },
     R"(Only contiguous VRB mask allocation is currently supported\.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.tbs_lbrm               = units::bytes(0);
       return pdu;
     },
     R"(Invalid LBRM size \(0 bytes\)\.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.codewords.clear();
       return pdu;
     },
     R"(Only one codeword is currently supported\.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.bwp_start_rb           = 0;
       pdu.bwp_size_rb            = 52;
       pdu.freq_alloc             = rb_allocation::make_type1(0, 52);
       pdu.freq_alloc             = rb_allocation::make_type1(0, 52, vrb_to_prb::create_non_interleaved_common_ss(1));
       return pdu;
     },
     R"(Invalid BWP configuration, i\.e\., \[0, 52\) for the given RB allocation, i\.e\., \[0, 52\)\.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.bwp_start_rb           = 0;
       pdu.bwp_size_rb            = 52;
       pdu.freq_alloc             = rb_allocation::make_type1(0, 52);
       pdu.freq_alloc = rb_allocation::make_type1(0, 52, vrb_to_prb::create_interleaved_common_ss(1, 0, 52));
       return pdu;
     },
     R"(Invalid BWP configuration, i\.e\., \[0, 52\) for the given RB allocation, i\.e\., \[0, 52\)\.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.bwp_start_rb           = 0;
       pdu.bwp_size_rb            = 52;
       pdu.freq_alloc             = rb_allocation::make_type1(0, 52);
       pdu.freq_alloc             = rb_allocation::make_type1(0, 52, vrb_to_prb::create_interleaved_coreset0(1, 52));
       return pdu;
     },
     R"(Invalid BWP configuration, i\.e\., \[0, 52\) for the given RB allocation, i\.e\., \[0, 52\)\.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;

       // Create RE pattern that collides with DM-RS.
       re_pattern reserved_pattern;
       reserved_pattern.crb_mask = ~crb_bitmap(MAX_RB);
       reserved_pattern.crb_mask.fill(0, MAX_RB);
       reserved_pattern.symbols = pdu.dmrs_symbol_mask;
       reserved_pattern.re_mask = ~bounded_bitset<NRE>(NRE);
       pdu.reserved.merge(reserved_pattern);

       return pdu;
     },
     R"(Reserved REs collide with DM-RS\.)"},
};

class pdschProcessorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  static std::unique_ptr<pdsch_processor>     pdsch_proc;
  static std::unique_ptr<pdsch_pdu_validator> pdu_validator;

  static void SetUpTestSuite()
  {
    if (pdsch_proc && pdu_validator) {
      return;
    }

    // Create pseudo-random sequence generator.
    std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
    ASSERT_NE(prg_factory, nullptr);

    // Create demodulator mapper factory.
    std::shared_ptr<modulation_mapper_factory> chan_modulation_factory = create_modulation_mapper_factory();
    ASSERT_NE(chan_modulation_factory, nullptr);

    // Create CRC calculator factory.
    std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
    ASSERT_NE(crc_calc_factory, nullptr);

    // Create LDPC decoder factory.
    std::shared_ptr<ldpc_encoder_factory> ldpc_enc_factory = create_ldpc_encoder_factory_sw("auto");
    ASSERT_NE(ldpc_enc_factory, nullptr);

    // Create LDPC rate dematcher factory.
    std::shared_ptr<ldpc_rate_matcher_factory> ldpc_rm_factory = create_ldpc_rate_matcher_factory_sw();
    ASSERT_NE(ldpc_rm_factory, nullptr);

    // Create LDPC desegmenter factory.
    std::shared_ptr<ldpc_segmenter_tx_factory> ldpc_segm_tx_factory =
        create_ldpc_segmenter_tx_factory_sw(crc_calc_factory);
    ASSERT_NE(ldpc_segm_tx_factory, nullptr);

    // Create channel precoding factory.
    std::shared_ptr<channel_precoder_factory> precoding_factory = create_channel_precoder_factory("auto");
    ASSERT_NE(precoding_factory, nullptr);

    // Create resource grid mapper factory.
    std::shared_ptr<resource_grid_mapper_factory> rg_mapper_factory =
        create_resource_grid_mapper_factory(precoding_factory);
    ASSERT_NE(rg_mapper_factory, nullptr);

    // Create DM-RS for pdsch channel estimator.
    std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_pdsch_proc_factory =
        create_dmrs_pdsch_processor_factory_sw(prg_factory, rg_mapper_factory);
    ASSERT_NE(dmrs_pdsch_proc_factory, nullptr);

    // Create PT-RS for pdsch channel estimator.
    std::shared_ptr<ptrs_pdsch_generator_factory> ptrs_pdsch_gen_factory =
        create_ptrs_pdsch_generator_generic_factory(prg_factory, rg_mapper_factory);
    ASSERT_NE(ptrs_pdsch_gen_factory, nullptr);

    // Create PDSCH demodulator factory.
    std::shared_ptr<pdsch_modulator_factory> pdsch_mod_factory =
        create_pdsch_modulator_factory_sw(chan_modulation_factory, prg_factory, rg_mapper_factory);
    ASSERT_NE(pdsch_mod_factory, nullptr);

    // Create PDSCH decoder factory.
    pdsch_encoder_factory_sw_configuration pdsch_enc_config;
    pdsch_enc_config.encoder_factory                         = ldpc_enc_factory;
    pdsch_enc_config.rate_matcher_factory                    = ldpc_rm_factory;
    pdsch_enc_config.segmenter_factory                       = ldpc_segm_tx_factory;
    std::shared_ptr<pdsch_encoder_factory> pdsch_enc_factory = create_pdsch_encoder_factory_sw(pdsch_enc_config);
    ASSERT_NE(pdsch_enc_factory, nullptr);

    // Create PDSCH processor.
    std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory = create_pdsch_processor_factory_sw(
        pdsch_enc_factory, pdsch_mod_factory, dmrs_pdsch_proc_factory, ptrs_pdsch_gen_factory);
    ASSERT_NE(pdsch_proc_factory, nullptr);

    // Create actual PDSCH processor.
    pdsch_proc = pdsch_proc_factory->create();
    ASSERT_NE(pdsch_proc, nullptr);

    // Create actual PDSCH PDU validator.
    pdu_validator = pdsch_proc_factory->create_validator();
    ASSERT_NE(pdu_validator, nullptr);
  }
};

std::unique_ptr<pdsch_processor>     pdschProcessorFixture::pdsch_proc;
std::unique_ptr<pdsch_pdu_validator> pdschProcessorFixture::pdu_validator;

TEST_P(pdschProcessorFixture, pdschProcessorValidatorDeathTest)
{
  // Use thread safe death test.
  ::testing::GTEST_FLAG(death_test_style) = "threadsafe";

  ASSERT_NE(pdsch_proc, nullptr);
  ASSERT_NE(pdu_validator, nullptr);

  const test_case_t& param = GetParam();

  // Make sure the configuration is invalid.
  error_type<std::string> validator_out = pdu_validator->is_valid(param.get_pdu());
  ASSERT_FALSE(validator_out.has_value()) << "Validation should fail.";
  ASSERT_TRUE(std::regex_match(validator_out.error(), std::regex(param.expr))) << fmt::format(
      R"(The assertion message "{}" doesn't match the expected pattern "{}".)", validator_out.error(), param.expr);
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(pdschProcessorValidatorDeathTest,
                         pdschProcessorFixture,
                         ::testing::ValuesIn(pdsch_processor_validator_test_data));

} // namespace
