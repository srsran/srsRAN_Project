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

#include "srsran/ran/prach/prach_frequency_mapping.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

namespace srsran {

std::ostream& operator<<(std::ostream& os, const prach_subcarrier_spacing& ra_scs)
{
  fmt::print(os, "{}", to_string(ra_scs));
  return os;
}

std::ostream& operator<<(std::ostream& os, const subcarrier_spacing& scs)
{
  fmt::print(os, "{} kHz", scs_to_khz(scs));

  return os;
}
} // namespace srsran

using namespace srsran;

// Combined parameters.
using prach_frequency_mapping_param = std::tuple<prach_subcarrier_spacing, subcarrier_spacing>;

// Expected values from TS38.211 Table 6.3.3.2-1.
static const std::map<prach_frequency_mapping_param, prach_frequency_mapping_information>
    prach_frequency_mapping_expected = {
        {{prach_subcarrier_spacing::kHz1_25, subcarrier_spacing::kHz15}, {6, 7}},
        {{prach_subcarrier_spacing::kHz1_25, subcarrier_spacing::kHz30}, {3, 1}},
        {{prach_subcarrier_spacing::kHz1_25, subcarrier_spacing::kHz60}, {2, 133}},
        {{prach_subcarrier_spacing::kHz5, subcarrier_spacing::kHz15}, {24, 12}},
        {{prach_subcarrier_spacing::kHz5, subcarrier_spacing::kHz30}, {12, 10}},
        {{prach_subcarrier_spacing::kHz5, subcarrier_spacing::kHz60}, {6, 7}},
        {{prach_subcarrier_spacing::kHz15, subcarrier_spacing::kHz15}, {12, 2}},
        {{prach_subcarrier_spacing::kHz15, subcarrier_spacing::kHz30}, {6, 2}},
        {{prach_subcarrier_spacing::kHz15, subcarrier_spacing::kHz60}, {3, 2}},
        {{prach_subcarrier_spacing::kHz30, subcarrier_spacing::kHz15}, {24, 2}},
        {{prach_subcarrier_spacing::kHz30, subcarrier_spacing::kHz30}, {12, 2}},
        {{prach_subcarrier_spacing::kHz30, subcarrier_spacing::kHz60}, {6, 2}},
        {{prach_subcarrier_spacing::kHz60, subcarrier_spacing::kHz60}, {12, 2}},
        {{prach_subcarrier_spacing::kHz60, subcarrier_spacing::kHz120}, {6, 2}},
        {{prach_subcarrier_spacing::kHz120, subcarrier_spacing::kHz60}, {24, 2}},
        {{prach_subcarrier_spacing::kHz120, subcarrier_spacing::kHz120}, {12, 2}},
};

class prach_frequency_mapping_test : public ::testing::TestWithParam<prach_frequency_mapping_param>
{
protected:
  static prach_frequency_mapping_information get_expected()
  {
    // Get parameter.
    prach_frequency_mapping_param param = GetParam();

    // If there is no entry, it is reserved.
    if (prach_frequency_mapping_expected.count(param) == 0) {
      return PRACH_FREQUENCY_MAPPING_INFORMATION_RESERVED;
    }

    // Return the expected value.
    return prach_frequency_mapping_expected.at(param);
  }
};

TEST_P(prach_frequency_mapping_test, check_combinations)
{
  // Get combined parameter.
  prach_frequency_mapping_param params = GetParam();

  // Get result.
  prach_frequency_mapping_information result = prach_frequency_mapping_get(std::get<0>(params), std::get<1>(params));

  // Get expected value.
  prach_frequency_mapping_information expected = get_expected();

  // Compare with expected.
  ASSERT_EQ(expected.nof_rb_ra, result.nof_rb_ra);
  ASSERT_EQ(expected.k_bar, result.k_bar);
}

// Creates test suite that combines all possible parameters. Denote zero_correlation_zone exceeds the maximum by one.
INSTANTIATE_TEST_SUITE_P(prach_cyclic_shifts,
                         prach_frequency_mapping_test,
                         ::testing::Combine(::testing::Values(prach_subcarrier_spacing::kHz1_25,
                                                              prach_subcarrier_spacing::kHz5,
                                                              prach_subcarrier_spacing::kHz15,
                                                              prach_subcarrier_spacing::kHz30,
                                                              prach_subcarrier_spacing::kHz60,
                                                              prach_subcarrier_spacing::kHz120),
                                            ::testing::Values(subcarrier_spacing::kHz15,
                                                              subcarrier_spacing::kHz30,
                                                              subcarrier_spacing::kHz60,
                                                              subcarrier_spacing::kHz120)));
