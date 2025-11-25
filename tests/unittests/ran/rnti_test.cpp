/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ran/gnb_id.h"
#include "srsran/ran/rnti.h"
#include <gtest/gtest.h>

using namespace srsran;

constexpr uint8_t  default_nof_ue_bits = 13;
constexpr gnb_id_t default_gnb_id{411, 22};
constexpr uint32_t default_ue_id = 0xffffffff;

class rnti_test : public ::testing::Test
{};

TEST_F(rnti_test, full_i_rnti_creation_from_invalid_number_fails)
{
  auto ret = i_rnti_t::from_uint(0x1ffffffffff, default_nof_ue_bits, true);
  ASSERT_FALSE(ret.has_value());
}

TEST_F(rnti_test, full_i_rnti_creation_from_valid_number_succeeds)
{
  auto ret = i_rnti_t::from_uint(0xdeadbeef, default_nof_ue_bits, true);
  ASSERT_TRUE(ret.has_value());
  ASSERT_EQ(ret.value().full_i_rnti(), 0xdeadbeef);
}

TEST_F(rnti_test, short_i_rnti_creation_from_invalid_number_fails)
{
  auto ret = i_rnti_t::from_uint(0x1fffff, default_nof_ue_bits, false);
  ASSERT_FALSE(ret.has_value());
}

TEST_F(rnti_test, short_i_rnti_creation_from_valid_number_succeeds)
{
  auto ret = i_rnti_t::from_uint(0x36000, default_nof_ue_bits, false);
  ASSERT_TRUE(ret.has_value());
  ASSERT_EQ(ret.value().short_i_rnti(), 0x36000);
}

TEST_F(rnti_test, i_rnti_creation_succeeds)
{
  // Check creation for all possible nof ue bits values.
  for (uint8_t nof_ue_bits = 1; nof_ue_bits <= 18; ++nof_ue_bits) {
    auto ret = i_rnti_t::create(default_gnb_id.id, default_ue_id, nof_ue_bits);
    ASSERT_TRUE(ret.full_i_rnti().has_value());
  }

  auto ret = i_rnti_t::create(default_gnb_id.id, default_ue_id, default_nof_ue_bits);
  ASSERT_EQ(ret.max_full_i_rnti(), 0x337fff);
  ASSERT_EQ(ret.max_short_i_rnti(), 0x37fff);
  ASSERT_EQ(ret.min(), 0x336000);
}
TEST_F(rnti_test, i_rnti_equality_operators_work_as_expected)
{
  auto ret1 = i_rnti_t::create(default_gnb_id.id, default_ue_id, default_nof_ue_bits);
  auto ret2 = i_rnti_t::create(default_gnb_id.id, default_ue_id, default_nof_ue_bits);
  auto ret3 = i_rnti_t::create(default_gnb_id.id, default_ue_id - 1, default_nof_ue_bits);

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
