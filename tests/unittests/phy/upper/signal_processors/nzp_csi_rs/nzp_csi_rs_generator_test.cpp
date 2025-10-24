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

#include "nzp_csi_rs_generator_test_data.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/signal_processors/nzp_csi_rs/factories.h"
#include "srsran/phy/upper/signal_processors/nzp_csi_rs/nzp_csi_rs_formatter.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, test_case_t tcase)
{
  fmt::print(os, "{}", tcase.config);
  return os;
}

} // namespace srsran

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

      std::shared_ptr<channel_precoder_factory> precoding_factory = create_channel_precoder_factory("auto");
      ASSERT_NE(precoding_factory, nullptr);

      std::shared_ptr<resource_grid_mapper_factory> rg_mapper_factory =
          create_resource_grid_mapper_factory(precoding_factory);
      ASSERT_NE(rg_mapper_factory, nullptr);

      std::shared_ptr<nzp_csi_rs_generator_factory> nzp_csi_rs_generator_fact =
          create_nzp_csi_rs_generator_factory_sw(prg_factory, rg_mapper_factory);
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

  unsigned max_symb  = MAX_NSYMB_PER_SLOT;
  unsigned max_prb   = test_case.config.start_rb + test_case.config.nof_rb;
  unsigned max_ports = test_case.config.precoding.get_nof_ports();

  // Prepare resource grid and resource grid mapper spies.
  resource_grid_writer_spy grid(max_ports, max_symb, max_prb);

  // The configuration must be valid.
  ASSERT_TRUE(validator->is_valid(test_case.config).has_value());

  // Map NZP-CSI-RS using the test case arguments.
  generator->map(grid, test_case.config);

  // Load output golden data.
  const std::vector<resource_grid_writer_spy::expected_entry_t> testvector_symbols = test_case.symbols.read();

  grid.assert_entries(testvector_symbols, std::sqrt(max_ports));
}

// Creates test suite with al the test cases.
INSTANTIATE_TEST_SUITE_P(NzpCsiRsGenerator,
                         NzpCsiRsGeneratorFixture,
                         ::testing::ValuesIn(nzp_csi_rs_generator_test_data));
