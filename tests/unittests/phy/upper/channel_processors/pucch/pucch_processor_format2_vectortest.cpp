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

#include "pucch_processor_format2_test_data.h"
#include "pucch_processor_test_fixture.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "srsran/phy/upper/channel_processors/pucch/formatters.h"
#include "srsran/phy/upper/equalization/equalization_factories.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace srsran;

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

} // namespace srsran

using PucchProcessorFormat2Param = test_case_t;
using PucchProcessorF2Fixture    = PucchProcessorTestFixture<PucchProcessorFormat2Param>;
template <>
std::unique_ptr<pucch_processor> PucchProcessorF2Fixture::processor = nullptr;
template <>
std::unique_ptr<pucch_pdu_validator> PucchProcessorF2Fixture::validator = nullptr;

TEST_P(PucchProcessorF2Fixture, PucchProcessorF2VectorTest)
{
  const test_case_t&                            test_case = GetParam();
  const context_t&                              context   = test_case.context;
  const pucch_processor::format2_configuration& config    = context.config;

  // Prepare resource grid.
  resource_grid_reader_spy grid(MAX_PORTS, MAX_NSYMB_PER_SLOT, MAX_NOF_PRBS);
  grid.write(test_case.grid.read());

  // Read expected UCI payload fields.
  std::vector<uint8_t> expected_harq_ack   = test_case.harq_ack.read();
  std::vector<uint8_t> expected_sr         = test_case.sr.read();
  std::vector<uint8_t> expected_csi_part_1 = test_case.csi_part_1.read();
  std::vector<uint8_t> expected_csi_part_2 = test_case.csi_part_2.read();

  // Make sure configuration is valid.
  ASSERT_TRUE(validator->is_valid(config));

  // Process PUCCH.
  pucch_processor_result result = processor->process(grid, config);

  // Assert expected UCI payload.
  ASSERT_EQ(result.message.get_status(), uci_status::valid);
  ASSERT_EQ(result.message.get_harq_ack_bits(), span<uint8_t>(expected_harq_ack));
  ASSERT_EQ(result.message.get_sr_bits(), span<uint8_t>(expected_sr));
  ASSERT_EQ(result.message.get_csi_part1_bits(), span<uint8_t>(expected_csi_part_1));
  ASSERT_EQ(result.message.get_csi_part2_bits(), span<uint8_t>(expected_csi_part_2));
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PucchProcessorF2VectorTest,
                         PucchProcessorF2Fixture,
                         ::testing::ValuesIn(pucch_processor_format2_test_data));
