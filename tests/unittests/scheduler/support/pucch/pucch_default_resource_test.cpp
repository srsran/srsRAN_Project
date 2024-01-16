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

#include "lib/scheduler/support/pucch/pucch_default_resource.h"
#include <gtest/gtest.h>

namespace srsran {

std::ostream& operator<<(std::ostream& os, pucch_format format)
{
  return os << "Format" << std::to_string(static_cast<unsigned>(format));
}

} // namespace srsran

using namespace srsran;

// Combined parameters. First row index and second BWP size.
using PucchDefaultResourceParam = std::tuple<unsigned, unsigned>;

class PucchDefaultResourceFixture : public ::testing::TestWithParam<PucchDefaultResourceParam>
{
protected:
  static pucch_default_resource get_expected(unsigned row_index, unsigned N_bwp_size)
  {
    pucch_default_resource result;

    // Select format.
    if (row_index < 3) {
      result.format = pucch_format::FORMAT_0;
    } else {
      result.format = pucch_format::FORMAT_1;
    }

    // Select first symbol.
    if (row_index < 3) {
      result.first_symbol_index = 12;
    } else if (row_index < 7) {
      result.first_symbol_index = 10;
    } else if (row_index < 11) {
      result.first_symbol_index = 4;
    } else {
      result.first_symbol_index = 0;
    }

    // Select number of symbols.
    if (row_index < 3) {
      result.nof_symbols = 2;
    } else if (row_index < 7) {
      result.nof_symbols = 4;
    } else if (row_index < 11) {
      result.nof_symbols = 10;
    } else {
      result.nof_symbols = 14;
    }

    // Select RB offset.
    if (row_index == 2) {
      result.rb_bwp_offset = 3;
    } else if (row_index == 5 || row_index == 9 || row_index == 13) {
      result.rb_bwp_offset = 2;
    } else if (row_index == 6 || row_index == 10 || row_index == 14) {
      result.rb_bwp_offset = 4;
    } else if (row_index == 15) {
      result.rb_bwp_offset = N_bwp_size / 4;
    } else {
      result.rb_bwp_offset = 0;
    }

    // Select set of initial cyclic shift indexes.
    if (row_index == 0) {
      result.cs_indexes = {0, 3};
    } else if (row_index == 1 || row_index == 2) {
      result.cs_indexes = {0, 4, 8};
    } else if (row_index == 3 || row_index == 7 || row_index == 11) {
      result.cs_indexes = {0, 6};
    } else {
      result.cs_indexes = {0, 3, 6, 9};
    }

    return result;
  }
};

TEST_P(PucchDefaultResourceFixture, Table_9_2_1_1)
{
  // Get combined parameter.
  PucchDefaultResourceParam param = GetParam();

  // Extract parameters.
  unsigned row_index  = std::get<0>(param);
  unsigned N_bwp_size = std::get<1>(param);

  // Get result.
  pucch_default_resource result = get_pucch_default_resource(row_index, N_bwp_size);

  // Get expected.
  pucch_default_resource expected = get_expected(row_index, N_bwp_size);

  // Compare with expected.
  ASSERT_EQ(expected.format, result.format);
  ASSERT_EQ(expected.first_symbol_index, result.first_symbol_index);
  ASSERT_EQ(expected.nof_symbols, result.nof_symbols);
  ASSERT_EQ(expected.rb_bwp_offset, result.rb_bwp_offset);
  ASSERT_EQ(expected.cs_indexes, result.cs_indexes);
}

// Creates test suite that combines all possible parameters. Denote zero_correlation_zone exceeds the maximum by one.
INSTANTIATE_TEST_SUITE_P(PucchDefaultResourceFixture,
                         PucchDefaultResourceFixture,
                         ::testing::Combine(::testing::Range(0U, 16U), ::testing::Values(52, 106)));