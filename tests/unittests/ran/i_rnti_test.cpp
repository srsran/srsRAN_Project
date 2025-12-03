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

#include "srsran/ran/gnb_id.h"
#include "srsran/ran/i_rnti.h"
#include <gtest/gtest.h>

using namespace srsran;

constexpr uint8_t  default_nof_ue_bits = 13;
constexpr gnb_id_t default_gnb_id{411, 22};
constexpr uint32_t default_ue_id = 0xffffffff;

class rnti_test : public ::testing::Test
{};

TEST_F(rnti_test, full_i_rnti_creation_from_invalid_number_fails)
{
  auto ret = full_i_rnti_t::from_uint(0x1ffffffffff, default_nof_ue_bits);
  ASSERT_FALSE(ret.has_value());
}

TEST_F(rnti_test, full_i_rnti_creation_from_valid_number_succeeds)
{
  auto ret = full_i_rnti_t::from_uint(0xdeadbeef, default_nof_ue_bits);
  ASSERT_TRUE(ret.has_value());
  ASSERT_EQ(ret.value().value(), 0xdeadbeef);
}

TEST_F(rnti_test, short_i_rnti_creation_from_invalid_number_fails)
{
  auto ret = short_i_rnti_t::from_uint(0x1fffff, default_nof_ue_bits);
  ASSERT_FALSE(ret.has_value());
}

TEST_F(rnti_test, short_i_rnti_creation_from_valid_number_succeeds)
{
  auto ret = short_i_rnti_t::from_uint(0x36000, default_nof_ue_bits);
  ASSERT_TRUE(ret.has_value());
  ASSERT_EQ(ret.value().value(), 0x36000);
}

TEST_F(rnti_test, i_rnti_creation_succeeds)
{
  // Check Full-I-RNTI creation for all possible nof ue bits values.
  for (uint8_t nof_ue_bits = 1; nof_ue_bits <= 18; ++nof_ue_bits) {
    auto ret = full_i_rnti_t{default_gnb_id.id, default_ue_id, nof_ue_bits};
    ASSERT_TRUE(ret.value() >= ret.min());
  }

  // Check Short-I-RNTI creation for all possible nof ue bits values.
  for (uint8_t nof_ue_bits = 1; nof_ue_bits <= 18; ++nof_ue_bits) {
    auto ret = short_i_rnti_t{default_gnb_id.id, default_ue_id, nof_ue_bits};
    ASSERT_TRUE(ret.value() >= ret.min());
  }

  full_i_rnti_t full_ret = full_i_rnti_t{default_gnb_id.id, default_ue_id, default_nof_ue_bits};
  ASSERT_EQ(full_ret.max(), 0x337fff);
  ASSERT_EQ(full_ret.min(), 0x336000);

  short_i_rnti_t short_ret = short_i_rnti_t{default_gnb_id.id, default_ue_id, default_nof_ue_bits};
  ASSERT_EQ(short_ret.max(), 0x37fff);
  ASSERT_EQ(short_ret.min(), 0x36000);
}
TEST_F(rnti_test, i_rnti_equality_operators_work_as_expected)
{
  auto ret1 = full_i_rnti_t{default_gnb_id.id, default_ue_id, default_nof_ue_bits};
  auto ret2 = full_i_rnti_t{default_gnb_id.id, default_ue_id, default_nof_ue_bits};
  auto ret3 = full_i_rnti_t{default_gnb_id.id, default_ue_id - 1, default_nof_ue_bits};

  ASSERT_TRUE(ret1 == ret2);
  ASSERT_FALSE(ret1 != ret2);
  ASSERT_FALSE(ret1 < ret2);
  ASSERT_TRUE(ret1 <= ret2);
  ASSERT_FALSE(ret1 > ret2);
  ASSERT_TRUE(ret1 >= ret2);

  ASSERT_FALSE(ret1 == ret3);
  ASSERT_TRUE(ret1 != ret3);
  ASSERT_FALSE(ret1 < ret3);
  ASSERT_FALSE(ret1 <= ret3);
  ASSERT_TRUE(ret1 > ret3);
  ASSERT_TRUE(ret1 >= ret3);
}
