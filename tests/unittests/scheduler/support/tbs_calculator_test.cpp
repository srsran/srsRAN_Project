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

#include "tbs_calculator_test_data.h"
#include "srsran/support/srsran_test.h"

using namespace srsran;

static unsigned expected_tbs_calculator_table_find_smallest_not_less_than(unsigned nof_info_prime)
{
  // TS38.214 Table 5.1.3.2-1.
  static const std::array<unsigned, 93> table_valid_tbs = {
      24,   32,   40,   48,   56,   64,   72,   80,   88,   96,   104,  112,  120,  128,  136,  144,  152,  160,  168,
      176,  184,  192,  208,  224,  240,  256,  272,  288,  304,  320,  336,  352,  368,  384,  408,  432,  456,  480,
      504,  528,  552,  576,  608,  640,  672,  704,  736,  768,  808,  848,  888,  928,  984,  1032, 1064, 1128, 1160,
      1192, 1224, 1256, 1288, 1320, 1352, 1416, 1480, 1544, 1608, 1672, 1736, 1800, 1864, 1928, 2024, 2088, 2152, 2216,
      2280, 2408, 2472, 2536, 2600, 2664, 2728, 2792, 2856, 2976, 3104, 3240, 3368, 3496, 3624, 3752, 3824};

  // Finds the smallest but not less than the number of information bits.
  for (unsigned tbs : table_valid_tbs) {
    if (!(tbs < nof_info_prime)) {
      return tbs;
    }
  }

  // Invalid value.
  return 0;
}

int main()
{
  // Tests the TBS calculator TS38.214 Table 5.1.3.2-1.
  for (unsigned nof_info_prime = 1; nof_info_prime != 3824; ++nof_info_prime) {
    TESTASSERT_EQ(expected_tbs_calculator_table_find_smallest_not_less_than(nof_info_prime),
                  tbs_calculator_table_find_smallest_not_less_than(nof_info_prime),
                  "nof_info_prime={}",
                  nof_info_prime);
  }

  // Tests the TBS calculation matches with the expected data.
  for (const test_case_t& test_case : tbs_calculator_test_data) {
    unsigned tbs = tbs_calculator_calculate(test_case.config);

    TESTASSERT_EQ(test_case.tbs, tbs);
  }

  return 0;
}