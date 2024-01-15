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

#include "lib/scheduler/support/pusch/pusch_default_time_allocation.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

namespace srsran {

std::ostream& operator<<(std::ostream& os, subcarrier_spacing scs)
{
  fmt::print(os, "{}kHz", scs_to_khz(scs));
  return os;
}

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
using pusch_default_time_allocation_params = std::tuple<unsigned, subcarrier_spacing>;

// Expected values from TS38.214 Table 6.1.2.1.1-2.
static const std::map<pusch_default_time_allocation_params, pusch_default_time_allocation_config>
    pusch_default_time_allocation_A_normal = {
        {{0, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1, 0, 14}},
        {{1, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1, 0, 12}},
        {{2, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1, 0, 10}},
        {{3, subcarrier_spacing::kHz15}, {sch_mapping_type::typeB, 1, 2, 10}},
        {{4, subcarrier_spacing::kHz15}, {sch_mapping_type::typeB, 1, 4, 10}},
        {{5, subcarrier_spacing::kHz15}, {sch_mapping_type::typeB, 1, 4, 8}},
        {{6, subcarrier_spacing::kHz15}, {sch_mapping_type::typeB, 1, 4, 6}},
        {{7, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1 + 1, 0, 14}},
        {{8, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1 + 1, 0, 12}},
        {{9, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1 + 1, 0, 10}},
        {{10, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1 + 2, 0, 14}},
        {{11, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1 + 2, 0, 12}},
        {{12, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1 + 2, 0, 10}},
        {{13, subcarrier_spacing::kHz15}, {sch_mapping_type::typeB, 1, 8, 6}},
        {{14, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1 + 3, 0, 14}},
        {{15, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1 + 3, 0, 10}},
        {{0, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1, 0, 14}},
        {{1, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1, 0, 12}},
        {{2, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1, 0, 10}},
        {{3, subcarrier_spacing::kHz30}, {sch_mapping_type::typeB, 1, 2, 10}},
        {{4, subcarrier_spacing::kHz30}, {sch_mapping_type::typeB, 1, 4, 10}},
        {{5, subcarrier_spacing::kHz30}, {sch_mapping_type::typeB, 1, 4, 8}},
        {{6, subcarrier_spacing::kHz30}, {sch_mapping_type::typeB, 1, 4, 6}},
        {{7, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1 + 1, 0, 14}},
        {{8, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1 + 1, 0, 12}},
        {{9, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1 + 1, 0, 10}},
        {{10, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1 + 2, 0, 14}},
        {{11, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1 + 2, 0, 12}},
        {{12, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1 + 2, 0, 10}},
        {{13, subcarrier_spacing::kHz30}, {sch_mapping_type::typeB, 1, 8, 6}},
        {{14, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1 + 3, 0, 14}},
        {{15, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1 + 3, 0, 10}},
        {{0, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2, 0, 14}},
        {{1, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2, 0, 12}},
        {{2, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2, 0, 10}},
        {{3, subcarrier_spacing::kHz60}, {sch_mapping_type::typeB, 2, 2, 10}},
        {{4, subcarrier_spacing::kHz60}, {sch_mapping_type::typeB, 2, 4, 10}},
        {{5, subcarrier_spacing::kHz60}, {sch_mapping_type::typeB, 2, 4, 8}},
        {{6, subcarrier_spacing::kHz60}, {sch_mapping_type::typeB, 2, 4, 6}},
        {{7, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2 + 1, 0, 14}},
        {{8, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2 + 1, 0, 12}},
        {{9, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2 + 1, 0, 10}},
        {{10, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2 + 2, 0, 14}},
        {{11, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2 + 2, 0, 12}},
        {{12, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2 + 2, 0, 10}},
        {{13, subcarrier_spacing::kHz60}, {sch_mapping_type::typeB, 2, 8, 6}},
        {{14, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2 + 3, 0, 14}},
        {{15, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2 + 3, 0, 10}},
        {{0, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3, 0, 14}},
        {{1, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3, 0, 12}},
        {{2, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3, 0, 10}},
        {{3, subcarrier_spacing::kHz120}, {sch_mapping_type::typeB, 3, 2, 10}},
        {{4, subcarrier_spacing::kHz120}, {sch_mapping_type::typeB, 3, 4, 10}},
        {{5, subcarrier_spacing::kHz120}, {sch_mapping_type::typeB, 3, 4, 8}},
        {{6, subcarrier_spacing::kHz120}, {sch_mapping_type::typeB, 3, 4, 6}},
        {{7, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3 + 1, 0, 14}},
        {{8, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3 + 1, 0, 12}},
        {{9, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3 + 1, 0, 10}},
        {{10, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3 + 2, 0, 14}},
        {{11, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3 + 2, 0, 12}},
        {{12, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3 + 2, 0, 10}},
        {{13, subcarrier_spacing::kHz120}, {sch_mapping_type::typeB, 3, 8, 6}},
        {{14, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3 + 3, 0, 14}},
        {{15, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3 + 3, 0, 10}},
};

// Expected values from TS38.214 Table 6.1.2.1.1-3.
static const std::map<pusch_default_time_allocation_params, pusch_default_time_allocation_config>
    pusch_default_time_allocation_A_extended = {
        {{0, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1, 0, 8}},
        {{1, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1, 0, 12}},
        {{2, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1, 0, 10}},
        {{3, subcarrier_spacing::kHz15}, {sch_mapping_type::typeB, 1, 2, 10}},
        {{4, subcarrier_spacing::kHz15}, {sch_mapping_type::typeB, 1, 4, 4}},
        {{5, subcarrier_spacing::kHz15}, {sch_mapping_type::typeB, 1, 4, 8}},
        {{6, subcarrier_spacing::kHz15}, {sch_mapping_type::typeB, 1, 4, 6}},
        {{7, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1 + 1, 0, 8}},
        {{8, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1 + 1, 0, 12}},
        {{9, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1 + 1, 0, 10}},
        {{10, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1 + 2, 0, 6}},
        {{11, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1 + 2, 0, 12}},
        {{12, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1 + 2, 0, 10}},
        {{13, subcarrier_spacing::kHz15}, {sch_mapping_type::typeB, 1, 8, 4}},
        {{14, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1 + 3, 0, 8}},
        {{15, subcarrier_spacing::kHz15}, {sch_mapping_type::typeA, 1 + 3, 0, 10}},
        {{0, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1, 0, 8}},
        {{1, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1, 0, 12}},
        {{2, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1, 0, 10}},
        {{3, subcarrier_spacing::kHz30}, {sch_mapping_type::typeB, 1, 2, 10}},
        {{4, subcarrier_spacing::kHz30}, {sch_mapping_type::typeB, 1, 4, 4}},
        {{5, subcarrier_spacing::kHz30}, {sch_mapping_type::typeB, 1, 4, 8}},
        {{6, subcarrier_spacing::kHz30}, {sch_mapping_type::typeB, 1, 4, 6}},
        {{7, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1 + 1, 0, 8}},
        {{8, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1 + 1, 0, 12}},
        {{9, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1 + 1, 0, 10}},
        {{10, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1 + 2, 0, 6}},
        {{11, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1 + 2, 0, 12}},
        {{12, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1 + 2, 0, 10}},
        {{13, subcarrier_spacing::kHz30}, {sch_mapping_type::typeB, 1, 8, 4}},
        {{14, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1 + 3, 0, 8}},
        {{15, subcarrier_spacing::kHz30}, {sch_mapping_type::typeA, 1 + 3, 0, 10}},
        {{0, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2, 0, 8}},
        {{1, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2, 0, 12}},
        {{2, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2, 0, 10}},
        {{3, subcarrier_spacing::kHz60}, {sch_mapping_type::typeB, 2, 2, 10}},
        {{4, subcarrier_spacing::kHz60}, {sch_mapping_type::typeB, 2, 4, 4}},
        {{5, subcarrier_spacing::kHz60}, {sch_mapping_type::typeB, 2, 4, 8}},
        {{6, subcarrier_spacing::kHz60}, {sch_mapping_type::typeB, 2, 4, 6}},
        {{7, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2 + 1, 0, 8}},
        {{8, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2 + 1, 0, 12}},
        {{9, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2 + 1, 0, 10}},
        {{10, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2 + 2, 0, 6}},
        {{11, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2 + 2, 0, 12}},
        {{12, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2 + 2, 0, 10}},
        {{13, subcarrier_spacing::kHz60}, {sch_mapping_type::typeB, 2, 8, 4}},
        {{14, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2 + 3, 0, 8}},
        {{15, subcarrier_spacing::kHz60}, {sch_mapping_type::typeA, 2 + 3, 0, 10}},
        {{0, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3, 0, 8}},
        {{1, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3, 0, 12}},
        {{2, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3, 0, 10}},
        {{3, subcarrier_spacing::kHz120}, {sch_mapping_type::typeB, 3, 2, 10}},
        {{4, subcarrier_spacing::kHz120}, {sch_mapping_type::typeB, 3, 4, 4}},
        {{5, subcarrier_spacing::kHz120}, {sch_mapping_type::typeB, 3, 4, 8}},
        {{6, subcarrier_spacing::kHz120}, {sch_mapping_type::typeB, 3, 4, 6}},
        {{7, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3 + 1, 0, 8}},
        {{8, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3 + 1, 0, 12}},
        {{9, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3 + 1, 0, 10}},
        {{10, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3 + 2, 0, 6}},
        {{11, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3 + 2, 0, 12}},
        {{12, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3 + 2, 0, 10}},
        {{13, subcarrier_spacing::kHz120}, {sch_mapping_type::typeB, 3, 8, 4}},
        {{14, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3 + 3, 0, 8}},
        {{15, subcarrier_spacing::kHz120}, {sch_mapping_type::typeA, 3 + 3, 0, 10}},
};

class pusch_default_time_allocation_test : public ::testing::TestWithParam<pusch_default_time_allocation_params>
{
protected:
  static pusch_default_time_allocation_config get_expected_A_normal()
  {
    // Get parameter.
    pusch_default_time_allocation_params param = GetParam();

    // If there is no entry, it is reserved.
    if (pusch_default_time_allocation_A_normal.count(param) == 0) {
      return PUSCH_DEFAULT_TIME_ALLOCATION_RESERVED;
    }

    // Return the expected value.
    return pusch_default_time_allocation_A_normal.at(param);
  }
  static pusch_default_time_allocation_config get_expected_A_extended()
  {
    // Get parameter.
    pusch_default_time_allocation_params param = GetParam();

    // If there is no entry, it is reserved.
    if (pusch_default_time_allocation_A_extended.count(param) == 0) {
      return PUSCH_DEFAULT_TIME_ALLOCATION_RESERVED;
    }

    // Return the expected value.
    return pusch_default_time_allocation_A_extended.at(param);
  }
};

TEST_P(pusch_default_time_allocation_test, A_normal)
{
  // Get combined parameter.
  pusch_default_time_allocation_params params = GetParam();

  // Extract parameters.
  unsigned           row_index = std::get<0>(params);
  subcarrier_spacing scs       = std::get<1>(params);

  // Get result.
  pusch_default_time_allocation_config result =
      pusch_default_time_allocation_default_A_get(cyclic_prefix::NORMAL, row_index, scs);

  // Get expected.
  pusch_default_time_allocation_config expected = get_expected_A_normal();

  // Compare with expected.
  ASSERT_EQ(expected.mapping_type, result.mapping_type);
  ASSERT_EQ(expected.k2, result.k2);
  ASSERT_EQ(expected.start_symbol, result.start_symbol);
  ASSERT_EQ(expected.duration, result.duration);
}

TEST_P(pusch_default_time_allocation_test, A_extended)
{
  // Get combined parameter.
  pusch_default_time_allocation_params params = GetParam();

  // Extract parameters.
  unsigned           row_index = std::get<0>(params);
  subcarrier_spacing scs       = std::get<1>(params);

  // Get result.
  pusch_default_time_allocation_config result =
      pusch_default_time_allocation_default_A_get(cyclic_prefix::EXTENDED, row_index, scs);

  // Get expected.
  pusch_default_time_allocation_config expected = get_expected_A_extended();

  // Compare with expected.
  ASSERT_EQ(expected.mapping_type, result.mapping_type);
  ASSERT_EQ(expected.k2, result.k2);
  ASSERT_EQ(expected.start_symbol, result.start_symbol);
  ASSERT_EQ(expected.duration, result.duration);
}

// Creates test suite that combines all possible parameters. Denote zero_correlation_zone exceeds the maximum by one.
INSTANTIATE_TEST_SUITE_P(pusch_default_time_allocation,
                         pusch_default_time_allocation_test,
                         ::testing::Combine(::testing::Range(0U, 16U),
                                            ::testing::Values(subcarrier_spacing::kHz15,
                                                              subcarrier_spacing::kHz30,
                                                              subcarrier_spacing::kHz60,
                                                              subcarrier_spacing::kHz120)));
