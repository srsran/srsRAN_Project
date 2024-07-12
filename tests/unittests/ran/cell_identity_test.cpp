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

#include "srsran/ran/nr_cell_identity.h"
#include "srsran/ran/plmn_identity.h"
#include <gtest/gtest.h>

using namespace srsran;

class bcd_helper_test : public ::testing::Test
{};

TEST_F(bcd_helper_test, is_valid_mcc_checks_bcd_integer)
{
  ASSERT_FALSE(bcd_helper::is_valid_mcc(0x0000));
  ASSERT_FALSE(bcd_helper::is_valid_mcc(0xfa00));
  ASSERT_FALSE(bcd_helper::is_valid_mcc(0xff00));
  ASSERT_TRUE(bcd_helper::is_valid_mcc(0xf000));
  ASSERT_TRUE(bcd_helper::is_valid_mcc(0xf001));
  ASSERT_TRUE(bcd_helper::is_valid_mcc(0xf100));
}

TEST_F(bcd_helper_test, is_valid_mcc_checks_string)
{
  ASSERT_FALSE(bcd_helper::is_valid_mcc("1234"));
  ASSERT_FALSE(bcd_helper::is_valid_mcc("abc"));
  ASSERT_FALSE(bcd_helper::is_valid_mcc("12a"));
  ASSERT_FALSE(bcd_helper::is_valid_mcc("12"));
  ASSERT_TRUE(bcd_helper::is_valid_mcc("123"));
}

TEST_F(bcd_helper_test, is_valid_mcc_checks_bytes)
{
  ASSERT_FALSE(bcd_helper::is_valid_mcc(std::array<uint8_t, 3>{10, 0, 1}));
  ASSERT_TRUE(bcd_helper::is_valid_mcc(std::array<uint8_t, 3>{1, 2, 9}));
}

TEST_F(bcd_helper_test, is_valid_mnc_checks_bcd_integer)
{
  ASSERT_FALSE(bcd_helper::is_valid_mnc(0x0000));
  ASSERT_FALSE(bcd_helper::is_valid_mcc(0xfa00));
  ASSERT_FALSE(bcd_helper::is_valid_mcc(0xfff0));
  ASSERT_TRUE(bcd_helper::is_valid_mnc(0xf000));
  ASSERT_TRUE(bcd_helper::is_valid_mnc(0xff00));
  ASSERT_TRUE(bcd_helper::is_valid_mnc(0xff10));
}

TEST_F(bcd_helper_test, is_valid_mnc_checks_string)
{
  ASSERT_FALSE(bcd_helper::is_valid_mnc("1234"));
  ASSERT_FALSE(bcd_helper::is_valid_mnc("abc"));
  ASSERT_FALSE(bcd_helper::is_valid_mnc("ab"));
  ASSERT_FALSE(bcd_helper::is_valid_mnc("12a"));
  ASSERT_TRUE(bcd_helper::is_valid_mnc("12"));
  ASSERT_TRUE(bcd_helper::is_valid_mnc("123"));
}

TEST_F(bcd_helper_test, is_valid_mnc_checks_bytes)
{
  static_vector<uint8_t, 3> bytes = {10, 1};
  ASSERT_FALSE(bcd_helper::is_valid_mnc(bytes));
  bytes = {10, 0, 1};
  ASSERT_FALSE(bcd_helper::is_valid_mnc(bytes));
  bytes = {1, 2};
  ASSERT_TRUE(bcd_helper::is_valid_mnc(bytes));
  bytes = {1, 2, 3};
  ASSERT_TRUE(bcd_helper::is_valid_mnc(bytes));
}

TEST_F(bcd_helper_test, conversion_string_to_mcc)
{
  uint16_t    mcc;
  std::string mcc_str;

  // invalid conversions.
  ASSERT_FALSE(bcd_helper::string_to_mcc("12a", &mcc));
  ASSERT_FALSE(bcd_helper::string_to_mcc("12", &mcc));
  ASSERT_FALSE(bcd_helper::string_to_mcc("1234", &mcc));
  ASSERT_FALSE(bcd_helper::mcc_to_string(0x1234, &mcc_str));

  // valid conversions.
  ASSERT_TRUE(bcd_helper::string_to_mcc("123", &mcc));
  ASSERT_EQ(mcc, 0xf123U);
  ASSERT_TRUE(bcd_helper::mcc_to_string(mcc, &mcc_str));
  ASSERT_EQ(mcc_str, "123");
}

TEST_F(bcd_helper_test, conversion_string_to_mnc)
{
  uint16_t    mnc;
  std::string mnc_str;

  // invalid conversions.
  ASSERT_FALSE(bcd_helper::string_to_mnc("12a", &mnc));
  ASSERT_FALSE(bcd_helper::string_to_mnc("1", &mnc));
  ASSERT_FALSE(bcd_helper::string_to_mnc("1234", &mnc));
  ASSERT_FALSE(bcd_helper::mnc_to_string(0x1234, &mnc_str));

  // valid 2-digit conversion.
  ASSERT_TRUE(bcd_helper::string_to_mnc("12", &mnc));
  ASSERT_EQ(mnc, 0xff12U);
  ASSERT_TRUE(bcd_helper::mnc_to_string(mnc, &mnc_str));
  ASSERT_EQ(mnc_str, "12");

  // valid 3-digit conversion.
  ASSERT_TRUE(bcd_helper::string_to_mnc("123", &mnc));
  ASSERT_EQ(mnc, 0xf123U);
  ASSERT_TRUE(bcd_helper::mnc_to_string(mnc, &mnc_str));
  ASSERT_EQ(mnc_str, "123");
}

TEST_F(bcd_helper_test, conversion_string_to_plmn)
{
  // invalid conversions.
  ASSERT_EQ(bcd_helper::plmn_string_to_bcd("1234"), 0);
  ASSERT_EQ(bcd_helper::plmn_string_to_bcd("1234567"), 0);
  ASSERT_EQ(bcd_helper::plmn_string_to_bcd("12345a"), 0);
  ASSERT_TRUE(bcd_helper::plmn_bcd_to_string(0x1234567).empty());

  // valid conversions.
  ASSERT_EQ(bcd_helper::plmn_string_to_bcd("12345"), 0x21f354);
  ASSERT_EQ(bcd_helper::plmn_string_to_bcd("123456"), 0x214365);
  ASSERT_EQ(bcd_helper::plmn_string_to_bcd("00101"), 0xf110);
  ASSERT_EQ(bcd_helper::plmn_string_to_bcd("20899"), 0x2f899);
  ASSERT_EQ(bcd_helper::plmn_bcd_to_string(bcd_helper::plmn_string_to_bcd("12345")), "12345");
  ASSERT_EQ(bcd_helper::plmn_bcd_to_string(bcd_helper::plmn_string_to_bcd("123456")), "123456");
  ASSERT_EQ(bcd_helper::plmn_bcd_to_string(0xf110), "00101");
  ASSERT_EQ(bcd_helper::plmn_bcd_to_string(0x2f899), "20899");
}

TEST_F(bcd_helper_test, conversion_plmn_to_mccmnc)
{
  uint16_t mcc, mnc;

  // invalid conversions.
  ASSERT_FALSE(bcd_helper::ngap_plmn_to_mccmnc(0x1234567, &mcc, &mnc));
  ASSERT_EQ(bcd_helper::bcd_plmn_to_mcc(0x1234567), 0);
  ASSERT_EQ(bcd_helper::bcd_plmn_to_mnc(0x1234567), 0);

  // valid conversions.
  ASSERT_EQ(bcd_helper::bcd_plmn_to_mcc(0x21f354), 0xf123U);
  ASSERT_EQ(bcd_helper::bcd_plmn_to_mnc(0x21f354), 0xff45U);
  ASSERT_EQ(bcd_helper::bcd_plmn_to_mcc(0xf110), 0xf001U);
  ASSERT_EQ(bcd_helper::bcd_plmn_to_mnc(0xf110), 0xff01U);
  ASSERT_EQ(bcd_helper::bcd_plmn_to_mcc(0x2f899), 0xf208U);
  ASSERT_EQ(bcd_helper::bcd_plmn_to_mnc(0x2f899), 0xff99U);
  ASSERT_TRUE(bcd_helper::ngap_plmn_to_mccmnc(0x21f354, &mcc, &mnc));
  ASSERT_EQ(mcc, 0xf123);
  ASSERT_EQ(mnc, 0xff45);
}

class mcc_test : public ::testing::Test
{};

TEST_F(mcc_test, mcc_creation_from_invalid_bcd_integer_fails)
{
  auto ret = mobile_country_code::from_bcd(0x0000);
  ASSERT_FALSE(ret.has_value());
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
  ASSERT_FALSE(ret.has_value());
}

TEST_F(mcc_test, mcc_creation_from_valid_string_succeeds)
{
  auto ret = mobile_country_code::from_string("001");
  ASSERT_TRUE(ret.has_value());
  ASSERT_EQ(ret.value().to_string(), "001");
  ASSERT_EQ(ret.value().to_bcd(), 0xf001);
}

TEST_F(mcc_test, mcc_creation_from_invalid_bytes_fails)
{
  std::array<uint8_t, 3> bytes = {20, 21, 1};
  auto                   ret   = mobile_country_code::from_bytes(bytes);
  ASSERT_FALSE(ret.has_value());
}

TEST_F(mcc_test, mcc_creation_from_valid_bytes_succeeds)
{
  std::array<uint8_t, 3> bytes = {0, 0, 1};
  auto                   ret   = mobile_country_code::from_bytes(bytes);
  ASSERT_TRUE(ret.has_value());
  mobile_country_code mcc = ret.value();
  ASSERT_EQ(mcc.to_string(), "001");
  ASSERT_EQ(mcc.to_bcd(), 0xf001);
  ASSERT_EQ(mcc.to_bytes(), bytes);
}

class mnc_test : public ::testing::Test
{};

TEST_F(mnc_test, mnc_creation_from_invalid_bcd_integer_fails)
{
  auto ret = mobile_network_code::from_bcd(0x0000);
  ASSERT_FALSE(ret.has_value());
}

TEST_F(mnc_test, mnc_creation_from_valid_2_digit_bcd_integer_succeeds)
{
  auto ret = mobile_network_code::from_bcd(0xff01U);
  ASSERT_TRUE(ret.has_value());
  mobile_network_code mnc = ret.value();
  ASSERT_EQ(mnc.to_string(), "01");
  ASSERT_EQ(mnc.to_bcd(), 0xff01U);
  ASSERT_EQ(mnc.nof_digits(), 2);
}

TEST_F(mnc_test, mnc_creation_from_valid_3_digit_bcd_integer_succeeds)
{
  auto ret = mobile_network_code::from_bcd(0xf001U);
  ASSERT_TRUE(ret.has_value());
  mobile_network_code mnc = ret.value();
  ASSERT_EQ(mnc.to_string(), "001");
  ASSERT_EQ(mnc.to_bcd(), 0xf001U);
  ASSERT_EQ(mnc.nof_digits(), 3);
}

TEST_F(mnc_test, mnc_creation_from_invalid_string_fails)
{
  auto ret = mobile_network_code::from_string("1234");
  ASSERT_FALSE(ret.has_value());
}

TEST_F(mnc_test, mnc_creation_from_valid_2_digit_string_succeeds)
{
  auto ret = mobile_network_code::from_string("01");
  ASSERT_TRUE(ret.has_value());
  mobile_network_code mnc = ret.value();
  ASSERT_EQ(mnc.to_string(), "01");
  ASSERT_EQ(mnc.to_bcd(), 0xff01U);
  ASSERT_EQ(mnc.nof_digits(), 2);
}

TEST_F(mnc_test, mnc_creation_from_valid_3_digit_string_succeeds)
{
  auto ret = mobile_network_code::from_string("001");
  ASSERT_TRUE(ret.has_value());
  mobile_network_code mnc = ret.value();
  ASSERT_EQ(mnc.to_string(), "001");
  ASSERT_EQ(mnc.to_bcd(), 0xf001U);
  ASSERT_EQ(mnc.nof_digits(), 3);
}

TEST_F(mnc_test, mnc_creation_from_invalid_bytes_fails)
{
  std::array<uint8_t, 3> bytes = {21, 1};
  auto                   ret   = mobile_network_code::from_bytes(bytes);
  ASSERT_FALSE(ret.has_value());
}

TEST_F(mnc_test, mnc_creation_from_valid_2_digit_bytes_succeeds)
{
  static_vector<uint8_t, 3> bytes = {0, 1};
  auto                      ret   = mobile_network_code::from_bytes(bytes);
  ASSERT_TRUE(ret.has_value());
  mobile_network_code mnc = ret.value();
  ASSERT_EQ(mnc.to_string(), "01");
  ASSERT_EQ(mnc.to_bcd(), 0xff01U);
  ASSERT_EQ(mnc.to_bytes(), bytes);
}

TEST_F(mnc_test, mnc_creation_from_valid_3_digit_bytes_succeeds)
{
  static_vector<uint8_t, 3> bytes = {0, 0, 1};
  auto                      ret   = mobile_network_code::from_bytes(bytes);
  ASSERT_TRUE(ret.has_value());
  mobile_network_code mnc = ret.value();
  ASSERT_EQ(mnc.to_string(), "001");
  ASSERT_EQ(mnc.to_bcd(), 0xf001U);
  ASSERT_EQ(mnc.to_bytes(), bytes);
}

class plmn_id_test : public ::testing::Test
{};

TEST_F(plmn_id_test, plmn_id_creation_from_invalid_string_fails)
{
  auto ret = plmn_identity::parse("1234567");
  ASSERT_FALSE(ret.has_value());
}

TEST_F(plmn_id_test, plmn_id_creation_from_valid_string_succeeds)
{
  auto ret = plmn_identity::parse("00101");
  ASSERT_TRUE(ret.has_value());
  plmn_identity plmn = ret.value();
  ASSERT_EQ(plmn.to_bcd(), 0xf110U);
  ASSERT_EQ(plmn.to_string(), "00101");
  ASSERT_EQ(plmn.mcc().to_string(), "001");
  ASSERT_EQ(plmn.mnc().to_string(), "01");
}

TEST_F(plmn_id_test, plmn_id_creation_from_mnc_mcc_succeeds)
{
  plmn_identity plmn{mobile_country_code::from_string("001").value(), mobile_network_code::from_string("01").value()};
  ASSERT_EQ(plmn.to_bcd(), 0xf110U);
  ASSERT_EQ(plmn.to_string(), "00101");
  ASSERT_EQ(plmn.mcc().to_string(), "001");
  ASSERT_EQ(plmn.mnc().to_string(), "01");
}

TEST_F(plmn_id_test, plmn_id_creation_from_invalid_bytes_fails)
{
  auto ret = plmn_identity::from_bytes({0x00, 0x1f, 0x0a});
  ASSERT_FALSE(ret.has_value());
  ret = plmn_identity::from_bytes({0x00, 0xdf, 0x01});
}

TEST_F(plmn_id_test, plmn_id_creation_from_valid_bytes_succeeds)
{
  plmn_identity plmn = plmn_identity::from_bytes({0x00, 0xf1, 0x10}).value();
  ASSERT_EQ(plmn.to_string(), "00101");
  ASSERT_EQ(plmn.to_bcd(), 0xf110);
  plmn = plmn_identity::from_bytes({0x00, 0x01, 0x10}).value();
  ASSERT_EQ(plmn.to_string(), "001001");
  ASSERT_EQ(plmn.to_bcd(), 0x0110);
  plmn = plmn_identity::from_bytes({0x21, 0x43, 0x65}).value();
  ASSERT_EQ(plmn.to_string(), "123456");
  ASSERT_EQ(plmn.to_bcd(), 0x214365);
}

TEST_F(plmn_id_test, plmn_id_conversion_to_bytes)
{
  plmn_identity          plmn  = plmn_identity::parse("00101").value();
  std::array<uint8_t, 3> bytes = {0x00, 0xf1, 0x10};
  ASSERT_EQ(plmn.to_bytes(), bytes);
  plmn  = plmn_identity::parse("001001").value();
  bytes = {0x00, 0x01, 0x10};
  ASSERT_EQ(plmn.to_bytes(), bytes);
  plmn  = plmn_identity::parse("123456").value();
  bytes = {0x21, 0x43, 0x65};
  ASSERT_EQ(plmn.to_bytes(), bytes);
  ASSERT_EQ(plmn.to_string(), "123456");
}

TEST_F(plmn_id_test, plmn_id_test_value_is_mcc_mnc_test_value)
{
  ASSERT_EQ(mobile_country_code::test_value().to_string(), "001");
  ASSERT_EQ(mobile_network_code::test_value().to_string(), "01");
  ASSERT_EQ(plmn_identity::test_value().mcc(), mobile_country_code::test_value());
  ASSERT_EQ(plmn_identity::test_value().mnc(), mobile_network_code::test_value());
}

class nci_test : public ::testing::Test
{};

TEST_F(nci_test, nci_creation_from_invalid_number_fails)
{
  auto ret = nr_cell_identity::create(0x123456789a);
  ASSERT_FALSE(ret.has_value());
}

TEST_F(nci_test, nci_creation_from_valid_number_succeeds)
{
  auto ret = nr_cell_identity::create(0x19b01);
  ASSERT_TRUE(ret.has_value());
  nr_cell_identity nci = ret.value();
  ASSERT_EQ(nci.value(), 0x19b01);

  ret = nr_cell_identity::create(0x123456789);
  ASSERT_TRUE(ret.has_value());
  nci = ret.value();
  ASSERT_EQ(nci.value(), 0x123456789);
}

TEST_F(nci_test, nci_creation_from_invalid_string_fails)
{
  auto ret = nr_cell_identity::parse_hex("123456789a");
  ASSERT_FALSE(ret.has_value());
  ret = nr_cell_identity::parse_hex("12345678x");
  ASSERT_FALSE(ret.has_value());
}

TEST_F(nci_test, nci_creation_from_valid_string_succeeds)
{
  auto ret = nr_cell_identity::parse_hex("19B01");
  ASSERT_TRUE(ret.has_value());
  nr_cell_identity nci = ret.value();
  ASSERT_EQ(nci.value(), 0x19b01);
  ASSERT_EQ(fmt::format("{:x}", nci), "19b01");
  ASSERT_EQ(fmt::format("{:#x}", nci), "0x19b01");

  ret = nr_cell_identity::parse_hex("123456789");
  ASSERT_TRUE(ret.has_value());
  nci = ret.value();
  ASSERT_EQ(nci.value(), 0x123456789);
  ASSERT_EQ(fmt::format("{:x}", nci), "123456789");
}

TEST_F(nci_test, nci_to_sector_id)
{
  auto ret = nr_cell_identity::create(0x19b01);
  ASSERT_TRUE(ret.has_value());
  nr_cell_identity nci = ret.value();
  ASSERT_EQ(nci.sector_id(4), 0x01);
  ASSERT_EQ(nci.sector_id(8), 0x01);
  ASSERT_EQ(nci.sector_id(12), 0xb01);
  ASSERT_EQ(nci.sector_id(14), 0x1b01);
}

TEST_F(nci_test, nci_to_gnb_id)
{
  auto ret = nr_cell_identity::create(0x19b01);
  ASSERT_TRUE(ret.has_value());
  nr_cell_identity nci = ret.value();
  gnb_id_t         gnb_id{0x19, 24};
  ASSERT_EQ(nci.gnb_id(24), gnb_id);
  gnb_id = {0x19b, 28};
  ASSERT_EQ(nci.gnb_id(28), gnb_id);
  gnb_id = {0x19b0, 32};
  ASSERT_EQ(nci.gnb_id(32), gnb_id);
}

TEST_F(nci_test, invalid_gnb_id_and_sector_id_to_nci_fails)
{
  gnb_id_t gnb_id{0x19, 24};
  auto     ret = nr_cell_identity::create(gnb_id, 0x1b01);
  ASSERT_FALSE(ret.has_value());
}

TEST_F(nci_test, valid_gnb_id_and_sector_id_to_nci_succeeds)
{
  gnb_id_t gnb_id{0x19, 24};
  auto     ret = nr_cell_identity::create(gnb_id, 0xb01);
  ASSERT_TRUE(ret.has_value());
  nr_cell_identity nci = ret.value();
  ASSERT_EQ(nci.gnb_id(24), gnb_id);
  ASSERT_EQ(nci.sector_id(12), 0xb01);
}
