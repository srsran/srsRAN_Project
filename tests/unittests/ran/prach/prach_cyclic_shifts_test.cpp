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

#include "srsran/ran/prach/prach_cyclic_shifts.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

namespace srsran {

std::ostream& operator<<(std::ostream& os, const prach_subcarrier_spacing& ra_scs)
{
  fmt::print(os, "{}", to_string(ra_scs));
  return os;
}

std::ostream& operator<<(std::ostream& os, const restricted_set_config& restricted_set)
{
  switch (restricted_set) {
    case restricted_set_config::UNRESTRICTED:
      fmt::print(os, "Unrestricted");
      break;
    case restricted_set_config::TYPE_A:
      fmt::print(os, "Type A");
      break;
    case restricted_set_config::TYPE_B:
    default:
      fmt::print(os, "Type B");
      break;
  }
  return os;
}
} // namespace srsran

using namespace srsran;

// Combined parameters.
using prach_cyclic_shift_param = std::tuple<prach_subcarrier_spacing, restricted_set_config, unsigned>;

// Combined expected values from TS38.211 Tables 6.3.3.1-5, 6.3.3.1-6 and 6.3.3.1-7.
static const std::map<prach_cyclic_shift_param, uint16_t> prach_cyclic_shift_expected = {
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::UNRESTRICTED, 0}, 0},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::UNRESTRICTED, 1}, 13},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::UNRESTRICTED, 2}, 15},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::UNRESTRICTED, 3}, 18},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::UNRESTRICTED, 4}, 22},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::UNRESTRICTED, 5}, 26},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::UNRESTRICTED, 6}, 32},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::UNRESTRICTED, 7}, 38},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::UNRESTRICTED, 8}, 46},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::UNRESTRICTED, 9}, 59},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::UNRESTRICTED, 10}, 76},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::UNRESTRICTED, 11}, 93},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::UNRESTRICTED, 12}, 119},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::UNRESTRICTED, 13}, 167},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::UNRESTRICTED, 14}, 279},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::UNRESTRICTED, 15}, 419},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_A, 0}, 15},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_A, 1}, 18},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_A, 2}, 22},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_A, 3}, 26},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_A, 4}, 32},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_A, 5}, 38},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_A, 6}, 46},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_A, 7}, 55},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_A, 8}, 68},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_A, 9}, 82},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_A, 10}, 100},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_A, 11}, 128},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_A, 12}, 158},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_A, 13}, 202},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_A, 14}, 237},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_B, 0}, 15},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_B, 1}, 18},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_B, 2}, 22},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_B, 3}, 26},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_B, 4}, 32},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_B, 5}, 38},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_B, 6}, 46},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_B, 7}, 55},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_B, 8}, 68},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_B, 9}, 82},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_B, 10}, 100},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_B, 11}, 118},
    {{prach_subcarrier_spacing::kHz1_25, restricted_set_config::TYPE_B, 12}, 137},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::UNRESTRICTED, 0}, 0},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::UNRESTRICTED, 1}, 13},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::UNRESTRICTED, 2}, 26},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::UNRESTRICTED, 3}, 33},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::UNRESTRICTED, 4}, 38},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::UNRESTRICTED, 5}, 41},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::UNRESTRICTED, 6}, 49},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::UNRESTRICTED, 7}, 55},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::UNRESTRICTED, 8}, 64},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::UNRESTRICTED, 9}, 76},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::UNRESTRICTED, 10}, 93},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::UNRESTRICTED, 11}, 119},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::UNRESTRICTED, 12}, 139},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::UNRESTRICTED, 13}, 209},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::UNRESTRICTED, 14}, 279},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::UNRESTRICTED, 15}, 419},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_A, 0}, 36},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_A, 1}, 57},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_A, 2}, 72},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_A, 3}, 81},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_A, 4}, 89},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_A, 5}, 94},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_A, 6}, 103},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_A, 7}, 112},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_A, 8}, 121},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_A, 9}, 132},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_A, 10}, 137},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_A, 11}, 152},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_A, 12}, 173},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_A, 13}, 195},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_A, 14}, 216},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_A, 15}, 237},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_B, 0}, 36},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_B, 1}, 57},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_B, 2}, 60},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_B, 3}, 63},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_B, 4}, 65},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_B, 5}, 68},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_B, 6}, 71},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_B, 7}, 77},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_B, 8}, 81},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_B, 9}, 85},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_B, 10}, 97},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_B, 11}, 109},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_B, 12}, 122},
    {{prach_subcarrier_spacing::kHz5, restricted_set_config::TYPE_B, 13}, 137},
    {{prach_subcarrier_spacing::kHz15, restricted_set_config::UNRESTRICTED, 0}, 0},
    {{prach_subcarrier_spacing::kHz15, restricted_set_config::UNRESTRICTED, 1}, 2},
    {{prach_subcarrier_spacing::kHz15, restricted_set_config::UNRESTRICTED, 2}, 4},
    {{prach_subcarrier_spacing::kHz15, restricted_set_config::UNRESTRICTED, 3}, 6},
    {{prach_subcarrier_spacing::kHz15, restricted_set_config::UNRESTRICTED, 4}, 8},
    {{prach_subcarrier_spacing::kHz15, restricted_set_config::UNRESTRICTED, 5}, 10},
    {{prach_subcarrier_spacing::kHz15, restricted_set_config::UNRESTRICTED, 6}, 12},
    {{prach_subcarrier_spacing::kHz15, restricted_set_config::UNRESTRICTED, 7}, 13},
    {{prach_subcarrier_spacing::kHz15, restricted_set_config::UNRESTRICTED, 8}, 15},
    {{prach_subcarrier_spacing::kHz15, restricted_set_config::UNRESTRICTED, 9}, 17},
    {{prach_subcarrier_spacing::kHz15, restricted_set_config::UNRESTRICTED, 10}, 19},
    {{prach_subcarrier_spacing::kHz15, restricted_set_config::UNRESTRICTED, 11}, 23},
    {{prach_subcarrier_spacing::kHz15, restricted_set_config::UNRESTRICTED, 12}, 27},
    {{prach_subcarrier_spacing::kHz15, restricted_set_config::UNRESTRICTED, 13}, 34},
    {{prach_subcarrier_spacing::kHz15, restricted_set_config::UNRESTRICTED, 14}, 46},
    {{prach_subcarrier_spacing::kHz15, restricted_set_config::UNRESTRICTED, 15}, 69},
};

class prach_cyclic_shift_test : public ::testing::TestWithParam<prach_cyclic_shift_param>
{
protected:
  static uint16_t get_expected()
  {
    // Get parameter.
    prach_cyclic_shift_param param = GetParam();

    // If the PRACH SCS is not 1.25 kHz nor 5 kHz, set it to 15 KHz.
    if (std::get<0>(param) != prach_subcarrier_spacing::kHz1_25 &&
        std::get<0>(param) != prach_subcarrier_spacing::kHz5) {
      std::get<0>(param) = prach_subcarrier_spacing::kHz15;
    }

    // If there is no entry, it is reserved.
    if (prach_cyclic_shift_expected.count(param) == 0) {
      return PRACH_CYCLIC_SHIFTS_RESERVED;
    }

    // Return the expected value.
    return prach_cyclic_shift_expected.at(param);
  }
};

TEST_P(prach_cyclic_shift_test, check_combinations)
{
  // Get combined parameter.
  prach_cyclic_shift_param params = GetParam();

  // Extract parameters.
  prach_subcarrier_spacing prach_scs             = std::get<0>(params);
  restricted_set_config    restricted_set        = std::get<1>(params);
  unsigned                 zero_correlation_zone = std::get<2>(params);

  // Get number of cyclic shifts.
  uint16_t N_cs = prach_cyclic_shifts_get(prach_scs, restricted_set, zero_correlation_zone);

  // Compare with expected.
  ASSERT_EQ(get_expected(), N_cs);
}

// Creates test suite that combines all possible parameters. Denote zero_correlation_zone exceeds the maximum by one.
INSTANTIATE_TEST_SUITE_P(prach_cyclic_shifts,
                         prach_cyclic_shift_test,
                         ::testing::Combine(::testing::Values(prach_subcarrier_spacing::kHz1_25,
                                                              prach_subcarrier_spacing::kHz5,
                                                              prach_subcarrier_spacing::kHz15,
                                                              prach_subcarrier_spacing::kHz30,
                                                              prach_subcarrier_spacing::kHz60,
                                                              prach_subcarrier_spacing::kHz120),
                                            ::testing::Values(restricted_set_config::UNRESTRICTED,
                                                              restricted_set_config::TYPE_A,
                                                              restricted_set_config::TYPE_B),
                                            ::testing::Range(0U, 17U)));
