/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../support/resource_grid_test_doubles.h"
#include "nzp_csi_rs_generator_test_data.h"
#include "srsgnb/phy/upper/signal_processors/signal_processor_factories.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace {
class NzpCsiRsGeneratorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  static std::unique_ptr<nzp_csi_rs_generator>               generator;
  static std::unique_ptr<nzp_csi_rs_configuration_validator> validator;

  static void SetUpTestSuite()
  {
    if ((generator == nullptr) || (validator == nullptr)) {
      std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
      ASSERT_NE(prg_factory, nullptr);

      std::shared_ptr<nzp_csi_rs_generator_factory> nzp_csi_rs_generator_fact =
          create_nzp_csi_rs_generator_factory_sw(prg_factory);
      ASSERT_NE(nzp_csi_rs_generator_fact, nullptr);

      generator = nzp_csi_rs_generator_fact->create();
      validator = nzp_csi_rs_generator_fact->create_validator();
    }
  }

  void SetUp() override
  {
    // Do nothing.
  }
};

std::unique_ptr<nzp_csi_rs_generator>               NzpCsiRsGeneratorFixture::generator = nullptr;
std::unique_ptr<nzp_csi_rs_configuration_validator> NzpCsiRsGeneratorFixture::validator = nullptr;
} // namespace

TEST_P(NzpCsiRsGeneratorFixture, Vector)
{
  const test_case_t& test_case = GetParam();

  ASSERT_NE(generator, nullptr);
  ASSERT_NE(validator, nullptr);

  // Create resource grid.
  resource_grid_writer_spy grid(MAX_PORTS, MAX_NSYMB_PER_SLOT, test_case.config.start_rb + test_case.config.nof_rb);

  // The configuration must be valid.
  ASSERT_TRUE(validator->is_valid(test_case.config));

  // Map NZP-CSI-RS using the test case arguments.
  generator->map(grid, test_case.config);

  // Load output golden data.
  const std::vector<resource_grid_writer_spy::expected_entry_t> testvector_symbols = test_case.symbols.read();

  grid.assert_entries(testvector_symbols);
}

// Creates test suite with al the test cases.
INSTANTIATE_TEST_SUITE_P(NzpCsiRsGenerator,
                         NzpCsiRsGeneratorFixture,
                         ::testing::ValuesIn(nzp_csi_rs_generator_test_data));
