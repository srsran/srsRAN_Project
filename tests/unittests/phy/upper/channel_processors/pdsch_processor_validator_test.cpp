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

#include "../../support/resource_grid_mapper_test_doubles.h"
#include "../../support/resource_grid_test_doubles.h"
#include "../rx_softbuffer_test_doubles.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "srsran/ran/dmrs.h"
#include "srsran/ran/precoding/precoding_codebooks.h"
#include "fmt/ostream.h"
#include "gtest/gtest.h"

using namespace srsran;

namespace {

// Valid PDSCH configuration used as a base for the test cases.
const pdsch_processor::pdu_t base_pdu = {nullopt,
                                         {0, 19},
                                         1,
                                         52,
                                         0,
                                         cyclic_prefix::NORMAL,
                                         {{modulation_scheme::QPSK, 0}},
                                         1,
                                         pdsch_processor::pdu_t::CRB0,
                                         {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
                                         dmrs_type::TYPE1,
                                         0,
                                         0,
                                         1,
                                         rb_allocation::make_type1(0, 52),
                                         2,
                                         12,
                                         ldpc_base_graph_type::BG1,
                                         3168,
                                         {},
                                         0,
                                         0,
                                         precoding_configuration::make_wideband(make_single_port())};

struct test_case_t {
  std::function<pdsch_processor::pdu_t()> get_pdu;
  std::string                             expr;
};

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os, "{}", test_case.get_pdu());
  return os;
}

const std::vector<test_case_t> pdsch_processor_validator_test_data = {
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.bwp_size_rb            = MAX_RB + 1;
       return pdu;
     },
     R"(Invalid BWP configuration \[0, 276\) for the given frequency allocation \[0\, 52\)\.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.dmrs_symbol_mask       = {1};
       return pdu;
     },
     R"(The DM-RS symbol mask size \(i\.e\., 1\), must be equal to the number of symbols in the slot \(i\.e\., 14\)\.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.dmrs_symbol_mask       = bounded_bitset<MAX_NSYMB_PER_SLOT>(MAX_NSYMB_PER_SLOT);
       return pdu;
     },
     R"(The number of OFDM symbols carrying DM-RS RE must be greater than zero.)"},
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
     R"(The transmission with time allocation \[2, 15\) exceeds the slot boundary of 14 symbols.)"},
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
     R"(Only contiguous allocation is currently supported\.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.tbs_lbrm_bytes         = 0;
       return pdu;
     },
     R"(Invalid LBRM size \(0 bytes\)\. It must be non-zero, less than or equal to 3168 bytes\.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.codewords.clear();
       return pdu;
     },
     R"(Expected 1 codewords and got 0 for 1 layers\.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.bwp_start_rb           = 0;
       pdu.bwp_size_rb            = 52;
       pdu.freq_alloc             = rb_allocation::make_type1(0, 52);
       pdu.freq_alloc = rb_allocation::make_type1(0, 52, vrb_to_prb_mapper::create_non_interleaved_common_ss(1));
       return pdu;
     },
     R"(Invalid BWP configuration \[0, 52\) for the given frequency allocation \[1\, 53\)\.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.bwp_start_rb           = 0;
       pdu.bwp_size_rb            = 52;
       pdu.freq_alloc             = rb_allocation::make_type1(0, 52);
       pdu.freq_alloc = rb_allocation::make_type1(0, 52, vrb_to_prb_mapper::create_interleaved_common(1, 0, 52));
       return pdu;
     },
     R"(Invalid BWP configuration \[0, 52\) for the given frequency allocation non-contiguous.)"},
    {[] {
       pdsch_processor::pdu_t pdu = base_pdu;
       pdu.bwp_start_rb           = 0;
       pdu.bwp_size_rb            = 52;
       pdu.freq_alloc             = rb_allocation::make_type1(0, 52);
       pdu.freq_alloc = rb_allocation::make_type1(0, 52, vrb_to_prb_mapper::create_interleaved_coreset0(1, 52));
       return pdu;
     },
     R"(Invalid BWP configuration \[0, 52\) for the given frequency allocation non-contiguous.)"},
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
    std::shared_ptr<channel_modulation_factory> chan_modulation_factory = create_channel_modulation_sw_factory();
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

    // Create DM-RS for pdsch channel estimator.
    std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_pdsch_proc_factory =
        create_dmrs_pdsch_processor_factory_sw(prg_factory);
    ASSERT_NE(dmrs_pdsch_proc_factory, nullptr);

    // Create PDSCH demodulator factory.
    std::shared_ptr<pdsch_modulator_factory> pdsch_mod_factory =
        create_pdsch_modulator_factory_sw(chan_modulation_factory, prg_factory);
    ASSERT_NE(pdsch_mod_factory, nullptr);

    // Create PDSCH decoder factory.
    pdsch_encoder_factory_sw_configuration pdsch_enc_config;
    pdsch_enc_config.encoder_factory                         = ldpc_enc_factory;
    pdsch_enc_config.rate_matcher_factory                    = ldpc_rm_factory;
    pdsch_enc_config.segmenter_factory                       = ldpc_segm_tx_factory;
    std::shared_ptr<pdsch_encoder_factory> pdsch_enc_factory = create_pdsch_encoder_factory_sw(pdsch_enc_config);
    ASSERT_NE(pdsch_enc_factory, nullptr);

    // Create PDSCH processor.
    std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory =
        create_pdsch_processor_factory_sw(pdsch_enc_factory, pdsch_mod_factory, dmrs_pdsch_proc_factory);
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
  ASSERT_FALSE(pdu_validator->is_valid(param.get_pdu()));

  // Prepare resource grid and resource grid mapper spies.
  resource_grid_writer_spy grid(0, 0, 0);
  resource_grid_mapper_spy mapper(grid);

  // Prepare receive data.
  std::vector<uint8_t> data;

  // Prepare softbuffer.
  rx_softbuffer_spy softbuffer(ldpc::MAX_CODEBLOCK_SIZE, 0);

  // Process pdsch PDU.
#ifdef ASSERTS_ENABLED
  ASSERT_DEATH({ pdsch_proc->process(mapper, {data}, param.get_pdu()); }, param.expr);
#endif // ASSERTS_ENABLED
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(pdschProcessorValidatorDeathTest,
                         pdschProcessorFixture,
                         ::testing::ValuesIn(pdsch_processor_validator_test_data));

} // namespace
