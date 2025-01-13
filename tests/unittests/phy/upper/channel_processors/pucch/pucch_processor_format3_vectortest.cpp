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

#include "pucch_processor_format3_test_data.h"
#include "pucch_processor_test_fixture.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "srsran/phy/upper/channel_processors/pucch/factories.h"
#include "srsran/phy/upper/channel_processors/pucch/formatters.h"
#include "srsran/phy/upper/equalization/equalization_factories.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace srsran;

using PucchProcessorF3Params = test_case_t;

namespace srsran {

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os,
             "grid: {} RB x {} symb, PUCCH config: {}",
             test_case.context.grid_nof_prb,
             test_case.context.grid_nof_symbols,
             test_case.context.config);
  return os;
}

std::ostream& operator<<(std::ostream& os, span<const uint8_t> data)
{
  fmt::print(os, "{}", data);
  return os;
}

} // namespace srsran

using PucchProcessorFormat3Param = test_case_t;
using PucchProcessorF3Fixture    = PucchProcessorTestFixture<PucchProcessorFormat3Param>;
template <>
std::unique_ptr<pucch_processor> PucchProcessorF3Fixture::processor = nullptr;
template <>
std::unique_ptr<pucch_pdu_validator> PucchProcessorF3Fixture::validator = nullptr;

TEST_P(PucchProcessorF3Fixture, PucchProcessorF3VectorTest)
{
  const test_case_t&                            test_case = GetParam();
  const context_t&                              context   = test_case.context;
  const pucch_processor::format3_configuration& config    = context.config;
  std::vector<uint8_t>                          uci_bits  = test_case.uci_bits.read();

  // Prepare resource grid.
  resource_grid_reader_spy grid(16, 14, MAX_NOF_PRBS);
  grid.write(test_case.grid.read());

  // Make sure configuration is valid.
  error_type<std::string> validation = validator->is_valid(config);
  ASSERT_TRUE(validation.has_value()) << fmt::format("PUCCH configuration validation failed with message:\n {}",
                                                     validation.error());

  // Process PUCCH.
  pucch_processor_result result = processor->process(grid, config);

  // Assert expected UCI payload.
  ASSERT_EQ(result.message.get_status(), uci_status::valid);
  ASSERT_EQ(span<const uint8_t>(result.message.get_harq_ack_bits()), span<const uint8_t>(uci_bits));
  ASSERT_EQ(span<const uint8_t>(result.message.get_sr_bits()), span<const uint8_t>());
  ASSERT_EQ(span<const uint8_t>(result.message.get_csi_part1_bits()), span<const uint8_t>());
  ASSERT_EQ(span<const uint8_t>(result.message.get_csi_part2_bits()), span<const uint8_t>());
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PucchProcessorF3VectorTest,
                         PucchProcessorF3Fixture,
                         ::testing::ValuesIn(pucch_processor_format3_test_data));
