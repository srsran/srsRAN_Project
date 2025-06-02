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

#include "srsran/ran/pusch/pusch_antenna_ports_mapping.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(PuschPrecodingInfo, Table7_3_1_1_2_2)
{
  static constexpr unsigned                                      max_rank = 4;
  static constexpr srs_resource_configuration::one_two_four_enum nof_srs_ports =
      srs_resource_configuration::one_two_four_enum::four;

#ifndef SRSRAN_HAS_ENTERPRISE
  GTEST_SKIP();
#endif // SRSRAN_HAS_ENTERPRISE

  for (unsigned row_index = 0; row_index != 62; ++row_index) {
    unsigned nof_layers = 0;
    unsigned tpmi       = 0;

    if (row_index < 4) {
      nof_layers = 1;
      tpmi       = row_index;
    } else if (row_index < 10) {
      nof_layers = 2;
      tpmi       = row_index - 4;
    } else if (row_index < 11) {
      nof_layers = 3;
      tpmi       = row_index - 10;
    } else if (row_index < 12) {
      nof_layers = 4;
      tpmi       = row_index - 11;
    } else if (row_index < 20) {
      nof_layers = 1;
      tpmi       = row_index - 8;
    } else if (row_index < 28) {
      nof_layers = 2;
      tpmi       = row_index - 14;
    } else if (row_index < 30) {
      nof_layers = 3;
      tpmi       = row_index - 27;
    } else if (row_index < 32) {
      nof_layers = 4;
      tpmi       = row_index - 29;
    } else if (row_index < 48) {
      nof_layers = 1;
      tpmi       = row_index - 20;
    } else if (row_index < 56) {
      nof_layers = 2;
      tpmi       = row_index - 34;
    } else if (row_index < 60) {
      nof_layers = 3;
      tpmi       = row_index - 53;
    } else {
      nof_layers = 4;
      tpmi       = row_index - 57;
    }

    ASSERT_EQ(get_pusch_precoding_info_row_index(nof_layers, max_rank, nof_srs_ports, tpmi), row_index);
  }
}

TEST(PuschPrecodingInfo, Table7_3_1_1_2_3)
{
  static constexpr unsigned                                      max_rank = 1;
  static constexpr srs_resource_configuration::one_two_four_enum nof_srs_ports =
      srs_resource_configuration::one_two_four_enum::four;
  static constexpr unsigned nof_layers = 1;

  for (unsigned row_index = 0; row_index != 28; ++row_index) {
    unsigned tpmi = row_index;

    ASSERT_EQ(get_pusch_precoding_info_row_index(nof_layers, max_rank, nof_srs_ports, tpmi), row_index);
  }
}

TEST(PuschPrecodingInfo, Table7_3_1_1_2_4)
{
  static constexpr unsigned                                      max_rank = 2;
  static constexpr srs_resource_configuration::one_two_four_enum nof_srs_ports =
      srs_resource_configuration::one_two_four_enum::two;

#ifndef SRSRAN_HAS_ENTERPRISE
  GTEST_SKIP();
#endif // SRSRAN_HAS_ENTERPRISE

  for (unsigned row_index = 0; row_index != 9; ++row_index) {
    unsigned tpmi       = 0;
    unsigned nof_layers = 0;

    if (row_index < 2) {
      tpmi       = row_index;
      nof_layers = 1;
    } else if (row_index < 3) {
      tpmi       = row_index - 2;
      nof_layers = 2;
    } else if (row_index < 7) {
      tpmi       = row_index - 1;
      nof_layers = 1;
    } else {
      tpmi       = row_index - 6;
      nof_layers = 2;
    }

    ASSERT_EQ(get_pusch_precoding_info_row_index(nof_layers, max_rank, nof_srs_ports, tpmi), row_index);
  }
}

TEST(PuschPrecodingInfo, Table7_3_1_1_2_5)
{
  static constexpr unsigned                                      max_rank = 1;
  static constexpr srs_resource_configuration::one_two_four_enum nof_srs_ports =
      srs_resource_configuration::one_two_four_enum::two;
  static constexpr unsigned nof_layers = 1;

  for (unsigned row_index = 0; row_index != 6; ++row_index) {
    unsigned tpmi = row_index;

    ASSERT_EQ(get_pusch_precoding_info_row_index(nof_layers, max_rank, nof_srs_ports, tpmi), row_index);
  }
}
