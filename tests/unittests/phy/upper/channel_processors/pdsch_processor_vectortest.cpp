/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdsch_processor_test_data.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "srsran/srsvec/bit.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os, "{}", test_case.context.pdu);
  return os;
}

namespace {

using PdschProcessorParams = test_case_t;

class PdschProcessorFixture : public ::testing::TestWithParam<PdschProcessorParams>
{
protected:
  std::unique_ptr<pdsch_processor>     pdsch_proc;
  std::unique_ptr<pdsch_pdu_validator> pdu_validator;

  void SetUp() override
  {
    std::shared_ptr<crc_calculator_factory> crc_calculator_factory = create_crc_calculator_factory_sw("auto");
    ASSERT_NE(crc_calculator_factory, nullptr);

    std::shared_ptr<ldpc_encoder_factory> ldpc_encoder_factory = create_ldpc_encoder_factory_sw("auto");
    ASSERT_NE(ldpc_encoder_factory, nullptr);

    std::shared_ptr<ldpc_rate_matcher_factory> ldpc_rate_matcher_factory = create_ldpc_rate_matcher_factory_sw();
    ASSERT_NE(ldpc_rate_matcher_factory, nullptr);

    std::shared_ptr<ldpc_segmenter_tx_factory> ldpc_segmenter_tx_factory =
        create_ldpc_segmenter_tx_factory_sw(crc_calculator_factory);
    ASSERT_NE(ldpc_segmenter_tx_factory, nullptr);

    pdsch_encoder_factory_sw_configuration pdsch_encoder_factory_config = {};
    pdsch_encoder_factory_config.encoder_factory                        = ldpc_encoder_factory;
    pdsch_encoder_factory_config.rate_matcher_factory                   = ldpc_rate_matcher_factory;
    pdsch_encoder_factory_config.segmenter_factory                      = ldpc_segmenter_tx_factory;
    std::shared_ptr<pdsch_encoder_factory> pdsch_encoder_factory =
        create_pdsch_encoder_factory_sw(pdsch_encoder_factory_config);
    ASSERT_NE(ldpc_encoder_factory, nullptr);

    std::shared_ptr<polar_factory> polar_code_factory = create_polar_factory_sw();
    ASSERT_NE(polar_code_factory, nullptr);

    std::shared_ptr<channel_modulation_factory> modulator_factory = create_channel_modulation_sw_factory();
    ASSERT_NE(modulator_factory, nullptr);

    std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
    ASSERT_NE(prg_factory, nullptr);

    std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_pdsch_factory =
        create_dmrs_pdsch_processor_factory_sw(prg_factory);
    ASSERT_NE(dmrs_pdsch_factory, nullptr);

    std::shared_ptr<dmrs_pdcch_processor_factory> dmrs_pdcch_factory =
        create_dmrs_pdcch_processor_factory_sw(prg_factory);
    ASSERT_NE(dmrs_pdcch_factory, nullptr);

    std::shared_ptr<pdcch_modulator_factory> pdcch_modulator_factory =
        create_pdcch_modulator_factory_sw(modulator_factory, prg_factory);
    ASSERT_NE(pdcch_modulator_factory, nullptr);

    std::shared_ptr<pdsch_modulator_factory> pdsch_modulator_factory =
        create_pdsch_modulator_factory_sw(modulator_factory, prg_factory);
    ASSERT_NE(pdsch_modulator_factory, nullptr);

    std::shared_ptr<pdcch_encoder_factory> pdcch_encoder_factory =
        create_pdcch_encoder_factory_sw(crc_calculator_factory, polar_code_factory);
    ASSERT_NE(pdcch_encoder_factory, nullptr);

    std::shared_ptr<pdcch_processor_factory> pdcch_proc_factory =
        create_pdcch_processor_factory_sw(pdcch_encoder_factory, pdcch_modulator_factory, dmrs_pdcch_factory);
    ASSERT_NE(pdcch_proc_factory, nullptr);

    std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory =
        create_pdsch_processor_factory_sw(pdsch_encoder_factory, pdsch_modulator_factory, dmrs_pdsch_factory);
    ASSERT_NE(pdsch_proc_factory, nullptr);

    // Create actual PDSCH processor.
    pdsch_proc = pdsch_proc_factory->create();
    ASSERT_NE(pdsch_proc, nullptr);

    // Create actual PDSCH processor validator.
    pdu_validator = pdsch_proc_factory->create_validator();
    ASSERT_NE(pdu_validator, nullptr);
  }
};

TEST_P(PdschProcessorFixture, PdschProcessorVectortest)
{
  const test_case_t&            test_case = GetParam();
  const test_case_context&      context   = test_case.context;
  const pdsch_processor::pdu_t& config    = context.pdu;

  resource_grid_writer_spy grid_actual(1, context.rg_nof_symb, context.rg_nof_rb, "info");

  // Read input data as a bit-packed transport block.
  std::vector<uint8_t> transport_block = test_case.sch_data.read();
  TESTASSERT(!transport_block.empty(), "Failed to load transport block.");

  // Prepare transport blocks view.
  static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS> transport_blocks;
  transport_blocks.emplace_back(transport_block);

  // Make sure the configuration is valid.
  ASSERT_TRUE(pdu_validator->is_valid(config));

  // Process PDSCH.
  pdsch_proc->process(grid_actual, transport_blocks, config);

  // Assert results.
  grid_actual.assert_entries(test_case.grid_expected.read());
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PdschProcessorVectortest,
                         PdschProcessorFixture,
                         ::testing::ValuesIn(pdsch_processor_test_data));
} // namespace

} // namespace srsran