/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ran/plmn_id.h"
#include <gtest/gtest.h>

using namespace srsran;

class mcc_test : public ::testing::Test
{};

TEST_F(mcc_test, mcc_creation_from_invalid_bcd_integer_fails)
{
  auto ret = mobile_country_code::from_bcd(0x0000);
  ASSERT_TRUE(ret.is_error());
}

TEST_F(mcc_test, mcc_creation_from_valid_bcd_integer_succeeds)
{
  auto ret = mobile_country_code::from_bcd(0xf001);
  ASSERT_TRUE(ret.has_value());
  ASSERT_EQ(ret.value().to_string(), "001");
  ASSERT_EQ(ret.value().to_bcd(), 0xf001);
}

TEST_F(mcc_test, mcc_creation_from_invalid_string_fails)
{
  auto ret = mobile_country_code::from_string("1234");
  ASSERT_TRUE(ret.is_error());
}

TEST_F(mcc_test, mcc_creation_from_valid_string_succeeds)
{
  auto ret = mobile_country_code::from_string("001");
  ASSERT_TRUE(ret.has_value());
  ASSERT_EQ(ret.value().to_string(), "001");
  ASSERT_EQ(ret.value().to_bcd(), 0xf001);
}

class mnc_test : public ::testing::Test
{};

TEST_F(mnc_test, mnc_creation_from_invalid_bcd_integer_fails)
{
  auto ret = mobile_network_code::from_bcd(0x0000);
  ASSERT_TRUE(ret.is_error());
}

TEST_F(mnc_test, mnc_creation_from_valid_2_digit_bcd_integer_succeeds)
{
  auto ret = mobile_network_code::from_bcd(0xFF01);
  ASSERT_TRUE(ret.has_value());
  mobile_network_code mnc = ret.value();
  ASSERT_EQ(mnc.to_string(), "01");
  ASSERT_EQ(mnc.to_bcd(), 0xFF01);
  ASSERT_EQ(mnc.nof_digits(), 2);
}

TEST_F(mnc_test, mnc_creation_from_valid_3_digit_bcd_integer_succeeds)
{
  auto ret = mobile_network_code::from_bcd(0xF001);
  ASSERT_TRUE(ret.has_value());
  mobile_network_code mnc = ret.value();
  ASSERT_EQ(mnc.to_string(), "001");
  ASSERT_EQ(mnc.to_bcd(), 0xF001);
  ASSERT_EQ(mnc.nof_digits(), 3);
}

TEST_F(mnc_test, mnc_creation_from_invalid_string_fails)
{
  auto ret = mobile_network_code::from_string("1234");
  ASSERT_TRUE(ret.is_error());
}

TEST_F(mnc_test, mnc_creation_from_valid_2_digit_string_succeeds)
{
  auto ret = mobile_network_code::from_string("01");
  ASSERT_TRUE(ret.has_value());
  mobile_network_code mnc = ret.value();
  ASSERT_EQ(mnc.to_string(), "01");
  ASSERT_EQ(mnc.to_bcd(), 0xFF01);
  ASSERT_EQ(mnc.nof_digits(), 2);
}

TEST_F(mnc_test, mnc_creation_from_valid_3_digit_string_succeeds)
{
  auto ret = mobile_network_code::from_string("001");
  ASSERT_TRUE(ret.has_value());
  mobile_network_code mnc = ret.value();
  ASSERT_EQ(mnc.to_string(), "001");
  ASSERT_EQ(mnc.to_bcd(), 0xF001);
  ASSERT_EQ(mnc.nof_digits(), 3);
}

class plmn_id_test : public ::testing::Test
{};

TEST_F(plmn_id_test, plmn_id_creation_from_invalid_string_fails)
{
  auto ret = plmn_identity::from_string("1234567");
  ASSERT_TRUE(ret.is_error());
}

TEST_F(plmn_id_test, plmn_id_creation_from_valid_string_succeeds)
{
  auto ret = plmn_identity::from_string("00101");
  ASSERT_TRUE(ret.has_value());
  plmn_identity plmn = ret.value();
  ASSERT_EQ(plmn.to_bcd(), 0xF110);
  ASSERT_EQ(plmn.to_string(), "00101");
  ASSERT_EQ(plmn.mcc().to_string(), "001");
  ASSERT_EQ(plmn.mnc().to_string(), "01");
}

TEST_F(plmn_id_test, plmn_id_creation_from_mnc_mcc_succeeds)
{
  plmn_identity plmn{mobile_country_code::from_string("001").value(), mobile_network_code::from_string("01").value()};
  ASSERT_EQ(plmn.to_bcd(), 0xF110);
  ASSERT_EQ(plmn.to_string(), "00101");
  ASSERT_EQ(plmn.mcc().to_string(), "001");
  ASSERT_EQ(plmn.mnc().to_string(), "01");
}
