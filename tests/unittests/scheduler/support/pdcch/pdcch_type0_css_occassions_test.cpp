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

#include "srsran/ran/pdcch/pdcch_type0_css_occasions.h"
#include "srsran/support/srsran_test.h"
#include <gtest/gtest.h>

using namespace srsran;

using test_params = std::tuple<unsigned, unsigned>;

class pdcch_type0_css_occasions_fixture : public ::testing::TestWithParam<test_params>
{};

TEST_P(pdcch_type0_css_occasions_fixture, fr1)
{
  unsigned ss0_index        = std::get<0>(GetParam());
  unsigned nof_symb_coreset = std::get<1>(GetParam());

  pdcch_type0_css_occasion_pattern1_configuration config;
  config.is_fr2           = false;
  config.ss_zero_index    = ss0_index;
  config.nof_symb_coreset = nof_symb_coreset;

  if (ss0_index > 15) {
#if ASSERTS_ENABLED
    ASSERT_DEATH({ pdcch_type0_css_occasions_get_pattern1(config); },
                 R"(SearchSpaceZero \([0-9]{1,}\) must be \[0\.\.15\] for FR1.)");
#endif // ASSERTS_ENABLED
    return;
  }

  pdcch_type0_css_occasion_pattern1_description occasion = pdcch_type0_css_occasions_get_pattern1(config);

  // Assert offset.
  if (ss0_index == 2 || ss0_index == 3 || ss0_index == 12 || ss0_index == 13) {
    ASSERT_EQ(occasion.offset, 2);
  } else if (ss0_index == 4 || ss0_index == 5 || ss0_index == 9 || ss0_index == 14 || ss0_index == 15) {
    ASSERT_EQ(occasion.offset, 5);
  } else if (ss0_index == 6 || ss0_index == 7) {
    ASSERT_EQ(occasion.offset, 7);
  } else {
    ASSERT_EQ(occasion.offset, 0);
  }
  // Assert M
  if ((ss0_index == 1) || (ss0_index == 3) || (ss0_index == 5) || (ss0_index == 7)) {
    ASSERT_EQ(occasion.M, 0.5);
  } else if ((ss0_index == 8) || (ss0_index == 9)) {
    ASSERT_EQ(occasion.M, 2.0);
  } else {
    ASSERT_EQ(occasion.M, 1.0);
  }
  // Assert start symbol.
  if (ss0_index == 1 || ss0_index == 3 || ss0_index == 5 || ss0_index == 7) {
    ASSERT_EQ(occasion.start_symbol.size(), 2);
    ASSERT_EQ(occasion.start_symbol[0], 0);
    ASSERT_EQ(occasion.start_symbol[1], nof_symb_coreset);
  } else if (ss0_index == 10 || ss0_index == 12 || ss0_index == 14) {
    ASSERT_EQ(occasion.start_symbol.size(), 1);
    ASSERT_EQ(occasion.start_symbol[0], 1);
  } else if (ss0_index == 11 || ss0_index == 13 || ss0_index == 15) {
    ASSERT_EQ(occasion.start_symbol.size(), 1);
    ASSERT_EQ(occasion.start_symbol[0], 2);
  } else {
    ASSERT_EQ(occasion.start_symbol.size(), 1);
    ASSERT_EQ(occasion.start_symbol[0], 0);
  }
}

TEST_P(pdcch_type0_css_occasions_fixture, fr2)
{
  unsigned ss0_index        = std::get<0>(GetParam());
  unsigned nof_symb_coreset = std::get<1>(GetParam());

  pdcch_type0_css_occasion_pattern1_configuration config;
  config.is_fr2           = true;
  config.ss_zero_index    = ss0_index;
  config.nof_symb_coreset = nof_symb_coreset;

  if (ss0_index > 13) {
#if ASSERTS_ENABLED
    ASSERT_DEATH({ pdcch_type0_css_occasions_get_pattern1(config); },
                 R"(SearchSpaceZero \([0-9]{1,}\) must be \[0\.\.13\] for FR2.)");
#endif // ASSERTS_ENABLED
    return;
  }

  pdcch_type0_css_occasion_pattern1_description occasion = pdcch_type0_css_occasions_get_pattern1(config);

  // Assert offset.
  if (ss0_index == 2 || ss0_index == 3 || ss0_index == 7) {
    ASSERT_EQ(occasion.offset, 2.5);
  } else if (ss0_index == 4 || ss0_index == 5 || ss0_index == 8 || ss0_index == 13) {
    ASSERT_EQ(occasion.offset, 5);
  } else if (ss0_index == 9 || ss0_index == 10 || ss0_index == 11) {
    ASSERT_EQ(occasion.offset, 7.5);
  } else {
    ASSERT_EQ(occasion.offset, 0.0);
  }
  // Assert M
  if (ss0_index == 0 || ss0_index == 2 || ss0_index == 4 || ss0_index == 9) {
    ASSERT_EQ(occasion.M, 1.0);
  } else if (ss0_index == 12 || ss0_index == 13) {
    ASSERT_EQ(occasion.M, 2.0);
  } else {
    ASSERT_EQ(occasion.M, 0.5);
  }
  // Assert start symbol.
  if (ss0_index == 1 || ss0_index == 3 || ss0_index == 5 || ss0_index == 10) {
    ASSERT_EQ(occasion.start_symbol.size(), 2);
    ASSERT_EQ(occasion.start_symbol[0], 0);
    ASSERT_EQ(occasion.start_symbol[1], 7);
  } else if (ss0_index == 6 || ss0_index == 7 || ss0_index == 8 || ss0_index == 11) {
    ASSERT_EQ(occasion.start_symbol.size(), 2);
    ASSERT_EQ(occasion.start_symbol[0], 0);
    ASSERT_EQ(occasion.start_symbol[1], nof_symb_coreset);
  } else {
    ASSERT_EQ(occasion.start_symbol.size(), 1);
    ASSERT_EQ(occasion.start_symbol[0], 0);
  }
}

INSTANTIATE_TEST_SUITE_P(OFHCompressionTestSuite,
                         pdcch_type0_css_occasions_fixture,
                         ::testing::Combine(::testing::Range(0U, 17U), ::testing::Range(1U, 3U)));