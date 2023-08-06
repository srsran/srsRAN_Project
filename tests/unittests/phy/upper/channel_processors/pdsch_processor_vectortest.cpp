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
#include "pdsch_processor_test_data.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "srsran/support/executors/task_worker_pool.h"
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

using PdschProcessorParams = std::tuple<std::string, test_case_t>;

// Number of concurrent threads.
static constexpr unsigned NOF_CONCURRENT_THREADS = 16;

class PdschProcessorFixture : public ::testing::TestWithParam<PdschProcessorParams>
{
private:
  std::shared_ptr<pdsch_processor_factory> create_pdsch_processor_factory(const std::string& type)
  {
    std::shared_ptr<crc_calculator_factory> crc_calculator_factory = create_crc_calculator_factory_sw("auto");
    if (!crc_calculator_factory) {
      return nullptr;
    }

    std::shared_ptr<ldpc_encoder_factory> ldpc_encoder_factory = create_ldpc_encoder_factory_sw("auto");
    if (!ldpc_encoder_factory) {
      return nullptr;
    }

    std::shared_ptr<ldpc_rate_matcher_factory> ldpc_rate_matcher_factory = create_ldpc_rate_matcher_factory_sw();
    if (!ldpc_rate_matcher_factory) {
      return nullptr;
    }

    std::shared_ptr<ldpc_segmenter_tx_factory> ldpc_segmenter_tx_factory =
        create_ldpc_segmenter_tx_factory_sw(crc_calculator_factory);
    if (!ldpc_segmenter_tx_factory) {
      return nullptr;
    }

    pdsch_encoder_factory_sw_configuration pdsch_encoder_factory_config = {};
    pdsch_encoder_factory_config.encoder_factory                        = ldpc_encoder_factory;
    pdsch_encoder_factory_config.rate_matcher_factory                   = ldpc_rate_matcher_factory;
    pdsch_encoder_factory_config.segmenter_factory                      = ldpc_segmenter_tx_factory;
    std::shared_ptr<pdsch_encoder_factory> pdsch_encoder_factory =
        create_pdsch_encoder_factory_sw(pdsch_encoder_factory_config);
    if (!pdsch_encoder_factory) {
      return nullptr;
    }

    std::shared_ptr<channel_modulation_factory> modulator_factory = create_channel_modulation_sw_factory();
    if (!modulator_factory) {
      return nullptr;
    }

    std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
    if (!prg_factory) {
      return nullptr;
    }

    std::shared_ptr<pdsch_modulator_factory> pdsch_modulator_factory =
        create_pdsch_modulator_factory_sw(modulator_factory, prg_factory);
    if (!pdsch_modulator_factory) {
      return nullptr;
    }

    std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_pdsch_factory =
        create_dmrs_pdsch_processor_factory_sw(prg_factory);
    if (!dmrs_pdsch_factory) {
      return nullptr;
    }

    if (type == "generic") {
      return create_pdsch_processor_factory_sw(pdsch_encoder_factory, pdsch_modulator_factory, dmrs_pdsch_factory);
    }

    if (type == "concurrent") {
      return create_pdsch_concurrent_processor_factory_sw(ldpc_segmenter_tx_factory,
                                                          ldpc_encoder_factory,
                                                          ldpc_rate_matcher_factory,
                                                          prg_factory,
                                                          modulator_factory,
                                                          dmrs_pdsch_factory,
                                                          executor,
                                                          NOF_CONCURRENT_THREADS);
    }

    if (type == "lite") {
      return create_pdsch_lite_processor_factory_sw(ldpc_segmenter_tx_factory,
                                                    ldpc_encoder_factory,
                                                    ldpc_rate_matcher_factory,
                                                    prg_factory,
                                                    modulator_factory,
                                                    dmrs_pdsch_factory);
    }

    return nullptr;
  }

protected:
  // PDSCH processor.
  std::unique_ptr<pdsch_processor> pdsch_proc;
  // PDSCH validator.
  std::unique_ptr<pdsch_pdu_validator> pdu_validator;
  // Worker pool.
  static task_worker_pool          worker_pool;
  static task_worker_pool_executor executor;

  void SetUp() override
  {
    const PdschProcessorParams& param        = GetParam();
    const std::string&          factory_type = std::get<0>(param);

    // Create PDSCH processor factory.
    std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory = create_pdsch_processor_factory(factory_type);
    ASSERT_NE(pdsch_proc_factory, nullptr) << "Invalid PDSCH processor factory.";

    // Create actual PDSCH processor.
    pdsch_proc = pdsch_proc_factory->create();
    ASSERT_NE(pdsch_proc, nullptr) << "Cannot create PDSCH processor";

    // Create actual PDSCH processor validator.
    pdu_validator = pdsch_proc_factory->create_validator();
    ASSERT_NE(pdu_validator, nullptr) << "Cannot create PDSCH validator";
  }

  static void TearDownTestSuite() { worker_pool.stop(); }
};

task_worker_pool          PdschProcessorFixture::worker_pool(NOF_CONCURRENT_THREADS, 128, "pdsch_proc");
task_worker_pool_executor PdschProcessorFixture::executor(PdschProcessorFixture::worker_pool);

TEST_P(PdschProcessorFixture, PdschProcessorVectortest)
{
  const PdschProcessorParams&   param     = GetParam();
  const test_case_t&            test_case = std::get<1>(param);
  const test_case_context&      context   = test_case.context;
  const pdsch_processor::pdu_t& config    = context.pdu;

  resource_grid_writer_spy grid_actual(4, context.rg_nof_symb, context.rg_nof_rb, "info");
  resource_grid_mapper_spy mapper(grid_actual);

  // Read input data as a bit-packed transport block.
  std::vector<uint8_t> transport_block = test_case.sch_data.read();
  ASSERT_FALSE(transport_block.empty()) << "Failed to load transport block.";

  // Prepare transport blocks view.
  static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS> transport_blocks;
  transport_blocks.emplace_back(transport_block);

  // Make sure the configuration is valid.
  ASSERT_TRUE(pdu_validator->is_valid(config));

  // Process PDSCH.
  pdsch_proc->process(mapper, transport_blocks, config);

  // Assert results.
  grid_actual.assert_entries(test_case.grid_expected.read());
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PdschProcessorVectortest,
                         PdschProcessorFixture,
                         testing::Combine(testing::Values("generic", "concurrent", "lite"),
                                          ::testing::ValuesIn(pdsch_processor_test_data)));
} // namespace

} // namespace srsran