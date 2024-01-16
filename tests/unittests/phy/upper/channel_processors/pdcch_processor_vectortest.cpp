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

#include "../../support/resource_grid_mapper_test_doubles.h"
#include "pdcch_processor_test_data.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, test_case_t test_case)
{
  fmt::print(os, "{}", test_case.config);
  return os;
}

} // namespace srsran

namespace {
class PdcchProcessorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  static std::unique_ptr<pdcch_processor>     processor;
  static std::unique_ptr<pdcch_pdu_validator> validator;

  static void SetUpTestSuite()
  {
    if (!(processor && validator)) {
      std::shared_ptr<crc_calculator_factory> crc_factory = create_crc_calculator_factory_sw("auto");
      ASSERT_NE(crc_factory, nullptr);

      std::shared_ptr<polar_factory> polar_factory = create_polar_factory_sw();
      ASSERT_NE(polar_factory, nullptr);

      std::shared_ptr<pdcch_encoder_factory> encoder_factory =
          create_pdcch_encoder_factory_sw(crc_factory, polar_factory);
      ASSERT_NE(encoder_factory, nullptr);

      std::shared_ptr<channel_modulation_factory> mod_factory = create_channel_modulation_sw_factory();
      ASSERT_NE(mod_factory, nullptr);

      std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
      ASSERT_NE(prg_factory, nullptr);

      std::shared_ptr<pdcch_modulator_factory> modulator_factory =
          create_pdcch_modulator_factory_sw(mod_factory, prg_factory);
      ASSERT_NE(modulator_factory, nullptr);

      std::shared_ptr<dmrs_pdcch_processor_factory> dmrs_pdcch_factory =
          create_dmrs_pdcch_processor_factory_sw(prg_factory);
      ASSERT_NE(dmrs_pdcch_factory, nullptr);

      std::shared_ptr<pdcch_processor_factory> factory =
          create_pdcch_processor_factory_sw(encoder_factory, modulator_factory, dmrs_pdcch_factory);
      ASSERT_NE(factory, nullptr);

      processor = factory->create();
      validator = factory->create_validator();
    }
  }
};

std::unique_ptr<pdcch_processor>     PdcchProcessorFixture::processor = nullptr;
std::unique_ptr<pdcch_pdu_validator> PdcchProcessorFixture::validator = nullptr;

} // namespace

TEST_P(PdcchProcessorFixture, FromVector)
{
  ASSERT_NE(processor, nullptr);
  ASSERT_NE(validator, nullptr);

  const test_case_t& test_case = GetParam();

  unsigned max_prb   = MAX_RB;
  unsigned max_symb  = test_case.config.coreset.start_symbol_index + test_case.config.coreset.duration;
  unsigned max_ports = test_case.config.dci.precoding.get_nof_ports();

  ASSERT_TRUE(validator->is_valid(test_case.config));

  // Prepare resource grid and resource grid mapper spies.
  resource_grid_writer_spy              grid(max_ports, max_symb, max_prb);
  std::unique_ptr<resource_grid_mapper> mapper = create_resource_grid_mapper(max_ports, NRE * max_prb, grid);

  // Process.
  processor->process(*mapper, test_case.config);

  // Load output golden data
  const std::vector<resource_grid_writer_spy::expected_entry_t> expected = test_case.data.read();

  // Assert resource grid entries.
  grid.assert_entries(expected);
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PdcchProcessorSw, PdcchProcessorFixture, ::testing::ValuesIn(pdcch_processor_test_data));
