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

#include "lib/scheduler/support/pdsch/pdsch_default_time_allocation.h"
#include <gtest/gtest.h>

namespace srsran {

std::ostream& operator<<(std::ostream& os, dmrs_typeA_position dmrs_pos)
{
  switch (dmrs_pos) {
    case dmrs_typeA_position::pos2:
      return os << "pos2";
    case dmrs_typeA_position::pos3:
    default:
      return os << "pos3";
  }
}

std::ostream& operator<<(std::ostream& os, sch_mapping_type mapping)
{
  switch (mapping) {
    case sch_mapping_type::typeA:
      return os << "typeA";
    case sch_mapping_type::typeB:
    default:
      return os << "typeB";
  }
}

} // namespace srsran

using namespace srsran;

// Combined parameters.
using pdsch_default_time_allocation_params = std::tuple<unsigned, dmrs_typeA_position>;

static ofdm_symbol_range s_and_len(unsigned start, unsigned dur)
{
  return ofdm_symbol_range(start, start + dur);
}

// Expected values from TS38.214 Table 5.1.2.1.1-2.
static const std::map<pdsch_default_time_allocation_params, pdsch_time_domain_resource_allocation>
    pdsch_default_time_allocation_A_normal = {
        {{0, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeA, s_and_len(2, 12)}},
        {{0, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeA, s_and_len(3, 11)}},
        {{1, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeA, s_and_len(2, 10)}},
        {{1, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeA, s_and_len(3, 9)}},
        {{2, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeA, s_and_len(2, 9)}},
        {{2, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeA, s_and_len(3, 8)}},
        {{3, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeA, s_and_len(2, 7)}},
        {{3, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeA, s_and_len(3, 6)}},
        {{4, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeA, s_and_len(2, 5)}},
        {{4, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeA, s_and_len(3, 4)}},
        {{5, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeB, s_and_len(9, 4)}},
        {{5, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeB, s_and_len(10, 4)}},
        {{6, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeB, s_and_len(4, 4)}},
        {{6, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeB, s_and_len(6, 4)}},
        {{7, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeB, s_and_len(5, 7)}},
        {{7, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeB, s_and_len(5, 7)}},
        {{8, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeB, s_and_len(5, 2)}},
        {{8, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeB, s_and_len(5, 2)}},
        {{9, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeB, s_and_len(9, 2)}},
        {{9, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeB, s_and_len(9, 2)}},
        {{10, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeB, s_and_len(12, 2)}},
        {{10, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeB, s_and_len(12, 2)}},
        {{11, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeA, s_and_len(1, 13)}},
        {{11, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeA, s_and_len(1, 13)}},
        {{12, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeA, s_and_len(1, 6)}},
        {{12, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeA, s_and_len(1, 6)}},
        {{13, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeA, s_and_len(2, 4)}},
        {{13, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeA, s_and_len(2, 4)}},
        {{14, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeB, s_and_len(4, 7)}},
        {{14, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeB, s_and_len(4, 7)}},
        {{15, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeB, s_and_len(8, 4)}},
        {{15, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeB, s_and_len(8, 4)}},
};

// Expected values from TS38.214 Table 5.1.2.1.1-3.
static const std::map<pdsch_default_time_allocation_params, pdsch_time_domain_resource_allocation>
    pdsch_default_time_allocation_A_extended = {
        {{0, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeA, s_and_len(2, 6)}},
        {{0, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeA, s_and_len(3, 5)}},
        {{1, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeA, s_and_len(2, 10)}},
        {{1, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeA, s_and_len(3, 9)}},
        {{2, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeA, s_and_len(2, 9)}},
        {{2, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeA, s_and_len(3, 8)}},
        {{3, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeA, s_and_len(2, 7)}},
        {{3, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeA, s_and_len(3, 6)}},
        {{4, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeA, s_and_len(2, 5)}},
        {{4, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeA, s_and_len(3, 4)}},
        {{5, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeB, s_and_len(6, 4)}},
        {{5, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeB, s_and_len(8, 2)}},
        {{6, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeB, s_and_len(4, 4)}},
        {{6, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeB, s_and_len(6, 4)}},
        {{7, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeB, s_and_len(5, 6)}},
        {{7, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeB, s_and_len(5, 6)}},
        {{8, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeB, s_and_len(5, 2)}},
        {{8, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeB, s_and_len(5, 2)}},
        {{9, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeB, s_and_len(9, 2)}},
        {{9, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeB, s_and_len(9, 2)}},
        {{10, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeB, s_and_len(10, 2)}},
        {{10, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeB, s_and_len(10, 2)}},
        {{11, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeA, s_and_len(1, 11)}},
        {{11, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeA, s_and_len(1, 11)}},
        {{12, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeA, s_and_len(1, 6)}},
        {{12, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeA, s_and_len(1, 6)}},
        {{13, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeA, s_and_len(2, 4)}},
        {{13, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeA, s_and_len(2, 4)}},
        {{14, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeB, s_and_len(4, 6)}},
        {{14, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeB, s_and_len(4, 6)}},
        {{15, dmrs_typeA_position::pos2}, {0, sch_mapping_type::typeB, s_and_len(8, 4)}},
        {{15, dmrs_typeA_position::pos3}, {0, sch_mapping_type::typeB, s_and_len(8, 4)}},
};

static pdsch_time_domain_resource_allocation INVALID_PDSCH_TIME_RESOURCE_ALLOCATION = {};

class pdsch_default_time_allocation_test : public ::testing::TestWithParam<pdsch_default_time_allocation_params>
{
protected:
  static pdsch_time_domain_resource_allocation get_expected_A_normal()
  {
    // Get parameter.
    pdsch_default_time_allocation_params param = GetParam();

    // If there is no entry, it is reserved.
    if (pdsch_default_time_allocation_A_normal.count(param) == 0) {
      return INVALID_PDSCH_TIME_RESOURCE_ALLOCATION;
    }

    // Return the expected value.
    return pdsch_default_time_allocation_A_normal.at(param);
  }
  static pdsch_time_domain_resource_allocation get_expected_A_extended()
  {
    // Get parameter.
    pdsch_default_time_allocation_params param = GetParam();

    // If there is no entry, it is reserved.
    if (pdsch_default_time_allocation_A_extended.count(param) == 0) {
      return INVALID_PDSCH_TIME_RESOURCE_ALLOCATION;
    }

    // Return the expected value.
    return pdsch_default_time_allocation_A_extended.at(param);
  }
};

TEST_P(pdsch_default_time_allocation_test, A_normal)
{
  // Get combined parameter.
  pdsch_default_time_allocation_params params = GetParam();

  // Extract parameters.
  unsigned            row_index = std::get<0>(params);
  dmrs_typeA_position dmrs_pos  = std::get<1>(params);

  // Get result.
  pdsch_time_domain_resource_allocation result =
      pdsch_default_time_allocation_default_A_get(cyclic_prefix::NORMAL, row_index, dmrs_pos);

  // Get expected.
  pdsch_time_domain_resource_allocation expected = get_expected_A_normal();

  // Compare with expected.
  ASSERT_EQ(expected, result);
}

TEST_P(pdsch_default_time_allocation_test, A_extended)
{
  // Get combined parameter.
  pdsch_default_time_allocation_params params = GetParam();

  // Extract parameters.
  unsigned            row_index = std::get<0>(params);
  dmrs_typeA_position dmrs_pos  = std::get<1>(params);

  // Get result.
  pdsch_time_domain_resource_allocation result =
      pdsch_default_time_allocation_default_A_get(cyclic_prefix::EXTENDED, row_index, dmrs_pos);

  // Get expected.
  pdsch_time_domain_resource_allocation expected = get_expected_A_extended();

  // Compare with expected.
  ASSERT_EQ(expected, result);
}

// Creates test suite that combines all possible parameters. Denote zero_correlation_zone exceeds the maximum by one.
INSTANTIATE_TEST_SUITE_P(pdsch_default_time_allocation,
                         pdsch_default_time_allocation_test,
                         ::testing::Combine(::testing::Range(0U, 16U),
                                            ::testing::Values(dmrs_typeA_position::pos2, dmrs_typeA_position::pos3)));
