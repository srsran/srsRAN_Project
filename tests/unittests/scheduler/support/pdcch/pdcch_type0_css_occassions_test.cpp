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

using namespace srsran;

static void test_pattern1()
{
  for (bool is_fr2 : {true, false}) {
    for (unsigned ss0_index = 0, ss0_index_end = is_fr2 ? 14 : 16; ss0_index != ss0_index_end; ++ss0_index) {
      for (unsigned nof_symb_coreset : {1, 2, 3}) {
        pdcch_type0_css_occasion_pattern1_configuration config;
        config.is_fr2           = is_fr2;
        config.ss_zero_index    = ss0_index;
        config.nof_symb_coreset = nof_symb_coreset;

        pdcch_type0_css_occasion_pattern1_description occasion = pdcch_type0_css_occasions_get_pattern1(config);

        if (is_fr2) {
          // Assert offset.
          if (ss0_index == 2 || ss0_index == 3 || ss0_index == 7) {
            TESTASSERT_EQ(occasion.offset, 2.5);
          } else if (ss0_index == 4 || ss0_index == 5 || ss0_index == 8 || ss0_index == 13) {
            TESTASSERT_EQ(occasion.offset, 5);
          } else if (ss0_index == 9 || ss0_index == 10 || ss0_index == 11) {
            TESTASSERT_EQ(occasion.offset, 7.5);
          } else {
            TESTASSERT_EQ(occasion.offset, 0.0);
          }
          // Assert M
          if (ss0_index == 0 || ss0_index == 2 || ss0_index == 4 || ss0_index == 9) {
            TESTASSERT_EQ(occasion.M, 1.0);
          } else if (ss0_index == 12 || ss0_index == 13) {
            TESTASSERT_EQ(occasion.M, 2.0);
          } else {
            TESTASSERT_EQ(occasion.M, 0.5);
          }
          // Assert start symbol.
          if (ss0_index == 1 || ss0_index == 3 || ss0_index == 5 || ss0_index == 10) {
            TESTASSERT_EQ(occasion.start_symbol.size(), 2);
            TESTASSERT_EQ(occasion.start_symbol[0], 0);
            TESTASSERT_EQ(occasion.start_symbol[1], 7);
          } else if (ss0_index == 6 || ss0_index == 7 || ss0_index == 8 || ss0_index == 11) {
            TESTASSERT_EQ(occasion.start_symbol.size(), 2);
            TESTASSERT_EQ(occasion.start_symbol[0], 0);
            TESTASSERT_EQ(occasion.start_symbol[1], nof_symb_coreset);
          } else {
            TESTASSERT_EQ(occasion.start_symbol.size(), 1);
            TESTASSERT_EQ(occasion.start_symbol[0], 0);
          }
        } else {
          // Assert offset.
          if (ss0_index == 2 || ss0_index == 3 || ss0_index == 12 || ss0_index == 13) {
            TESTASSERT_EQ(occasion.offset, 2);
          } else if (ss0_index == 4 || ss0_index == 5 || ss0_index == 9 || ss0_index == 14 || ss0_index == 15) {
            TESTASSERT_EQ(occasion.offset, 5);
          } else if (ss0_index == 6 || ss0_index == 7) {
            TESTASSERT_EQ(occasion.offset, 7);
          } else {
            TESTASSERT_EQ(occasion.offset, 0);
          }
          // Assert M
          if (ss0_index == 1 || ss0_index == 3 || ss0_index == 5 || ss0_index == 7) {
            TESTASSERT_EQ(occasion.M, 0.5);
          } else if (ss0_index == 8 || ss0_index == 9) {
            TESTASSERT_EQ(occasion.M, 2.0);
          } else {
            TESTASSERT_EQ(occasion.M, 1.0);
          }
          // Assert start symbol.
          if (ss0_index == 1 || ss0_index == 3 || ss0_index == 5 || ss0_index == 7) {
            TESTASSERT_EQ(occasion.start_symbol.size(), 2);
            TESTASSERT_EQ(occasion.start_symbol[0], 0);
            TESTASSERT_EQ(occasion.start_symbol[1], nof_symb_coreset);
          } else if (ss0_index == 10 || ss0_index == 12 || ss0_index == 14) {
            TESTASSERT_EQ(occasion.start_symbol.size(), 1);
            TESTASSERT_EQ(occasion.start_symbol[0], 1);
          } else if (ss0_index == 11 || ss0_index == 13 || ss0_index == 15) {
            TESTASSERT_EQ(occasion.start_symbol.size(), 1);
            TESTASSERT_EQ(occasion.start_symbol[0], 2);
          } else {
            TESTASSERT_EQ(occasion.start_symbol.size(), 1);
            TESTASSERT_EQ(occasion.start_symbol[0], 0);
          }
        }
      }
    }
  }
}

int main()
{
  test_pattern1();
}