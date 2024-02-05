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

#include "srsran/adt/optional.h"
#include "srsran/ran/srs/srs_bandwidth_configuration.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(SRSConfiguration, Bsrs_0)
{
  int m_srs[] = {4,   8,   12,  16,  16,  20,  24,  24,  28,  32,  36,  40,  48,  48,  52,  56,
                 60,  64,  72,  72,  76,  80,  88,  96,  96,  104, 112, 120, 120, 120, 128, 128,
                 128, 132, 136, 144, 144, 144, 144, 152, 160, 160, 160, 168, 176, 184, 192, 192,
                 192, 192, 208, 216, 224, 240, 240, 240, 240, 256, 256, 256, 264, 272, 272, 272};

  for (unsigned Csrs = 0; Csrs != 64; ++Csrs) {
    optional<srs_configuration> config = srs_configuration_get(Csrs, 0);

    // Assert format.
    ASSERT_EQ(config->N, 1);
    ASSERT_EQ(config->m_srs, m_srs[Csrs]);
  }
}

TEST(SRSConfiguration, Bsrs_1)
{
  int m_srs[] = {4,  4,  4,  4,  8,  4,  4,   12,  4,   16,  12, 20, 16, 24,  4,   28, 20,  32,  24, 36, 4,  40,
                 44, 32, 48, 52, 56, 60, 40,  24,  64,  64,  16, 44, 68, 72,  48,  48, 16,  76,  80, 80, 32, 84,
                 88, 92, 96, 96, 64, 24, 104, 108, 112, 120, 80, 48, 24, 128, 128, 16, 132, 136, 68, 16};

  int N[] = {1, 2, 3, 4, 2, 5, 6, 2, 7, 2, 3, 2, 3, 2, 13, 2, 3, 2, 3, 2, 19, 2, 2, 3, 2,  2, 2, 2,  3, 5, 2, 2,
             8, 3, 2, 2, 3, 3, 9, 2, 2, 2, 5, 2, 2, 2, 2,  2, 3, 8, 2, 2, 2,  2, 3, 5, 10, 2, 2, 16, 2, 2, 4, 17};

  for (unsigned Csrs = 0; Csrs != 64; ++Csrs) {
    optional<srs_configuration> config = srs_configuration_get(Csrs, 1);

    // Assert format.
    ASSERT_EQ(config->N, N[Csrs]);
    ASSERT_EQ(config->m_srs, m_srs[Csrs]);
  }
}

TEST(SRSConfiguration, Bsrs_2)
{
  int m_srs[] = {4,  4,  4,  4,  4,  4,  4,  4,  4,  8,  4,  4,  8,  12, 4,  4,  4,  16, 12, 12, 4,  20,
                 4,  16, 24, 4,  28, 20, 8,  12, 32, 16, 8,  4,  4,  36, 24, 16, 8,  4,  40, 20, 16, 28,
                 44, 4,  48, 24, 16, 8,  52, 36, 56, 60, 20, 16, 12, 64, 32, 8,  44, 68, 4,  8};

  int N[] = {1, 1,  1,  1, 2, 1, 1, 3,  1, 2, 3, 5, 2, 2,  1, 7, 5, 2, 2, 3, 1, 2, 11, 2, 2, 13, 2, 3, 5, 2, 2,  4,
             2, 11, 17, 2, 2, 3, 2, 19, 2, 4, 2, 3, 2, 23, 2, 4, 4, 3, 2, 3, 2, 2, 4,  3, 2, 2,  4, 2, 3, 2, 17, 2};

  for (unsigned Csrs = 0; Csrs != 64; ++Csrs) {
    optional<srs_configuration> config = srs_configuration_get(Csrs, 2);

    // Assert format.
    ASSERT_EQ(config->N, N[Csrs]);
    ASSERT_EQ(config->m_srs, m_srs[Csrs]);
  }
}

TEST(SRSConfiguration, Bsrs_3)
{
  int m_srs[] = {4, 4, 4, 4, 4,  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
                 4, 4, 4, 4, 12, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 4};

  int N[] = {1, 1, 1, 1, 1, 1, 1, 1, 1,  2, 1, 1, 2,  3, 1,  1, 1, 4, 3,  3, 1,  5,  1, 4, 6, 1,  7, 5, 2,  3,  8, 4,
             2, 1, 1, 9, 2, 4, 2, 1, 10, 5, 4, 7, 11, 1, 12, 6, 4, 2, 13, 9, 14, 15, 5, 2, 3, 16, 8, 2, 11, 17, 1, 2};

  for (unsigned Csrs = 0; Csrs != 64; ++Csrs) {
    optional<srs_configuration> config = srs_configuration_get(Csrs, 3);

    // Assert format.
    ASSERT_EQ(config->N, N[Csrs]);
    ASSERT_EQ(config->m_srs, m_srs[Csrs]);
  }
}

TEST(SRSConfiguration, InvalidParams)
{
  unsigned invalid_b_srs = 4;
  unsigned invalid_c_srs = 64;

  // Test invalid b_srs;
  for (unsigned c_srs = 0; c_srs != 64; ++c_srs) {
    optional<srs_configuration> config = srs_configuration_get(c_srs, invalid_b_srs);

    ASSERT_FALSE(config.has_value());
  }

  // Test invalid c_srs;
  for (unsigned b_srs = 0; b_srs != 4; ++b_srs) {
    optional<srs_configuration> config = srs_configuration_get(invalid_c_srs, b_srs);

    ASSERT_FALSE(config.has_value());
  }
}
