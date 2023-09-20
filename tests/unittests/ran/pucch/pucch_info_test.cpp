/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/ran/pucch/pucch_info.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace pucch_info_test {

// The struct is used by different test, each parameter can play the role of the input or of the expected output,
// depending on which test it's used in.
struct f2_code_rate_param {
  float    code_rate;
  unsigned nof_prb;
  unsigned nof_symbols;
  unsigned nof_payload_bits;
};

std::ostream& operator<<(std::ostream& os, const f2_code_rate_param& value)
{
  // If we print a float, we get a "." in the file name, which makes the test crash; we need to replace it with a "_";
  // Display 4 decimal digits.
  if (value.code_rate >= 0.1f) {
    fmt::print(os, "code_rate_0_");
  } else {
    fmt::print(os, "code_rate_0_0");
  }
  fmt::print(os,
             "{}_nof_prbs_{}_nof_symb_{}_payload_bits_{}",
             static_cast<unsigned>(value.code_rate * 10000),
             value.nof_prb,
             value.nof_symbols,
             value.nof_payload_bits);
  return os;
}

} // namespace pucch_info_test

using namespace pucch_info_test;

class pucch_f2_code_rate_test : public ::testing::TestWithParam<f2_code_rate_param>
{};

TEST_P(pucch_f2_code_rate_test, test_code_rate_different_parameters)
{
  ASSERT_FLOAT_EQ(GetParam().code_rate,
                  pucch_format2_code_rate(GetParam().nof_prb, GetParam().nof_symbols, GetParam().nof_payload_bits));
}

INSTANTIATE_TEST_SUITE_P(
    test_code_rate_different_parameters,
    pucch_f2_code_rate_test,
    ::testing::Values(
        f2_code_rate_param{.code_rate = 0.15625f, .nof_prb = 1, .nof_symbols = 2, .nof_payload_bits = 5},
        f2_code_rate_param{.code_rate = 0.03125f, .nof_prb = 5, .nof_symbols = 2, .nof_payload_bits = 5},
        f2_code_rate_param{.code_rate = 0.25f, .nof_prb = 1, .nof_symbols = 2, .nof_payload_bits = 8},
        f2_code_rate_param{.code_rate = 0.34375f, .nof_prb = 1, .nof_symbols = 2, .nof_payload_bits = 11},
        f2_code_rate_param{.code_rate = 0.21875f, .nof_prb = 1, .nof_symbols = 2, .nof_payload_bits = 7},
        f2_code_rate_param{.code_rate = 0.5625f, .nof_prb = 1, .nof_symbols = 2, .nof_payload_bits = 12},
        f2_code_rate_param{.code_rate = 0.78125f, .nof_prb = 1, .nof_symbols = 2, .nof_payload_bits = 19},
        f2_code_rate_param{.code_rate = 0.484375f, .nof_prb = 2, .nof_symbols = 2, .nof_payload_bits = 20},
        f2_code_rate_param{.code_rate = 0.34375f, .nof_prb = 4, .nof_symbols = 2, .nof_payload_bits = 33}),
    [](const testing::TestParamInfo<f2_code_rate_param>& params_item) { return fmt::format("{}", params_item.param); });

class pucch_f2_min_rb_test : public ::testing::TestWithParam<f2_code_rate_param>
{};

TEST_P(pucch_f2_min_rb_test, test_min_rbs_different_parameters)
{
  const unsigned max_f2_rbs = 16U;
  ASSERT_FLOAT_EQ(GetParam().nof_prb,
                  get_pucch_format2_nof_prbs(
                      GetParam().nof_payload_bits, max_f2_rbs, GetParam().nof_symbols, GetParam().code_rate));
}

INSTANTIATE_TEST_SUITE_P(
    test_min_rbs_different_parameters,
    pucch_f2_min_rb_test,
    ::testing::Values(f2_code_rate_param{.code_rate = 0.25f, .nof_prb = 1, .nof_symbols = 2, .nof_payload_bits = 8},
                      f2_code_rate_param{.code_rate = 0.25f, .nof_prb = 2, .nof_symbols = 2, .nof_payload_bits = 11},
                      f2_code_rate_param{.code_rate = 0.35f, .nof_prb = 1, .nof_symbols = 2, .nof_payload_bits = 11},
                      f2_code_rate_param{.code_rate = 0.35f, .nof_prb = 2, .nof_symbols = 2, .nof_payload_bits = 12},
                      f2_code_rate_param{.code_rate = 0.35f, .nof_prb = 3, .nof_symbols = 2, .nof_payload_bits = 19},
                      f2_code_rate_param{.code_rate = 0.45f, .nof_prb = 2, .nof_symbols = 2, .nof_payload_bits = 19},
                      f2_code_rate_param{.code_rate = 0.35f, .nof_prb = 3, .nof_symbols = 2, .nof_payload_bits = 20},
                      f2_code_rate_param{.code_rate = 0.45f, .nof_prb = 3, .nof_symbols = 2, .nof_payload_bits = 20},
                      f2_code_rate_param{.code_rate = 0.35f, .nof_prb = 4, .nof_symbols = 2, .nof_payload_bits = 25},
                      f2_code_rate_param{.code_rate = 0.45f, .nof_prb = 3, .nof_symbols = 2, .nof_payload_bits = 25}),
    [](const testing::TestParamInfo<f2_code_rate_param>& params_item) { return fmt::format("{}", params_item.param); });

class pucch_f2_max_payload_test : public ::testing::TestWithParam<f2_code_rate_param>
{};

TEST_P(pucch_f2_max_payload_test, test_max_rbs_different_parameters)
{
  ASSERT_FLOAT_EQ(GetParam().nof_payload_bits,
                  get_pucch_format2_max_payload(GetParam().nof_prb, GetParam().nof_symbols, GetParam().code_rate));
}

INSTANTIATE_TEST_SUITE_P(
    test_max_rbs_different_parameters,
    pucch_f2_max_payload_test,
    ::testing::Values(f2_code_rate_param{.code_rate = 0.25f, .nof_prb = 1, .nof_symbols = 2, .nof_payload_bits = 8},
                      f2_code_rate_param{.code_rate = 0.35f, .nof_prb = 1, .nof_symbols = 2, .nof_payload_bits = 11},
                      f2_code_rate_param{.code_rate = 0.25f, .nof_prb = 2, .nof_symbols = 2, .nof_payload_bits = 11},
                      f2_code_rate_param{.code_rate = 0.25f, .nof_prb = 3, .nof_symbols = 2, .nof_payload_bits = 18},
                      f2_code_rate_param{.code_rate = 0.25f, .nof_prb = 4, .nof_symbols = 2, .nof_payload_bits = 21},
                      f2_code_rate_param{.code_rate = 0.35f, .nof_prb = 2, .nof_symbols = 2, .nof_payload_bits = 16},
                      f2_code_rate_param{.code_rate = 0.35f, .nof_prb = 3, .nof_symbols = 2, .nof_payload_bits = 22}),
    [](const testing::TestParamInfo<f2_code_rate_param>& params_item) { return fmt::format("{}", params_item.param); });
