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

#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

std::string create_random_ipv4_string()
{
  std::vector<uint8_t> nums = test_rgen::random_vector<uint8_t>(4);
  return fmt::format("{}.{}.{}.{}", nums[0], nums[1], nums[2], nums[3]);
}

std::string create_random_ipv6_string()
{
  std::vector<uint16_t> nums = test_rgen::random_vector<uint16_t>(8);
  return fmt::format("{:x}:{:x}:{:x}:{:x}:{:x}:{:x}:{:x}:{:x}",
                     nums[0],
                     nums[1],
                     nums[2],
                     nums[3],
                     nums[4],
                     nums[5],
                     nums[6],
                     nums[7]);
}

std::string generate_random_ipv4_bitstring()
{
  uint32_t    random_number = test_rgen::uniform_int<uint32_t>();
  std::string bitstr        = fmt::format("{:032b}", random_number);

  return bitstr;
}

std::string generate_random_ipv6_bitstring()
{
  std::string bitstr;

  for (int i = 0; i < 2; i++) { // we need 128 bits for ipv6
    uint64_t random_number = test_rgen::uniform_int<uint64_t>();
    bitstr                 = bitstr + fmt::format("{:064b}", random_number);
  }

  return bitstr;
}

TEST(transport_layer_address_test, conversion_to_ipv4_string)
{
  std::string             ipv4_str = create_random_ipv4_string();
  transport_layer_address addr{ipv4_str};
  ASSERT_EQ(addr.to_string(), ipv4_str);
  ASSERT_EQ(fmt::format("{}", addr), ipv4_str);
}

TEST(transport_layer_address_test, conversion_to_ipv6_string)
{
  std::string             ipv6_str = create_random_ipv6_string();
  transport_layer_address addr{ipv6_str};
  ASSERT_EQ(addr.to_string(), ipv6_str);
  ASSERT_EQ(fmt::format("{}", addr), ipv6_str);
}

TEST(transport_layer_address_test, ipv4_address_comparison)
{
  std::string             ipv4_str1 = create_random_ipv4_string();
  std::string             ipv4_str2 = create_random_ipv4_string();
  transport_layer_address addr1{ipv4_str1}, addr2{ipv4_str2};
  ASSERT_EQ(addr1, ipv4_str1);
  ASSERT_EQ(addr2, ipv4_str2);
  ASSERT_EQ(addr1 == ipv4_str1, addr2 == ipv4_str2);
}

TEST(transport_layer_address_test, ipv6_address_comparison)
{
  std::string             ipv6_str1 = create_random_ipv6_string();
  std::string             ipv6_str2 = create_random_ipv6_string();
  transport_layer_address addr1{ipv6_str1}, addr2{ipv6_str2};
  ASSERT_EQ(addr1, ipv6_str1);
  ASSERT_EQ(addr2, ipv6_str2);
  ASSERT_EQ(addr1 == ipv6_str1, addr2 == ipv6_str2);
}

TEST(transport_layer_address_test, ipv4_transport_layer_address_to_asn1)
{
  up_transport_layer_info up_tp_layer_info = {transport_layer_address{create_random_ipv4_string()},
                                              int_to_gtp_teid(0x1)};

  asn1::f1ap::up_transport_layer_info_c asn1_transport_layer_info;

  up_transport_layer_info_to_asn1(asn1_transport_layer_info, up_tp_layer_info);

  ASSERT_EQ(up_tp_layer_info.gtp_teid, int_to_gtp_teid(asn1_transport_layer_info.gtp_tunnel().gtp_teid.to_number()));
  ASSERT_EQ(up_tp_layer_info.tp_address.to_bitstring(),
            asn1_transport_layer_info.gtp_tunnel().transport_layer_address.to_string());
}

TEST(transport_layer_address_test, ipv6_transport_layer_address_to_asn1)
{
  up_transport_layer_info up_tp_layer_info = {transport_layer_address{create_random_ipv6_string()},
                                              int_to_gtp_teid(0x1)};

  asn1::f1ap::up_transport_layer_info_c asn1_transport_layer_info;

  up_transport_layer_info_to_asn1(asn1_transport_layer_info, up_tp_layer_info);

  ASSERT_EQ(up_tp_layer_info.gtp_teid, int_to_gtp_teid(asn1_transport_layer_info.gtp_tunnel().gtp_teid.to_number()));
  ASSERT_EQ(up_tp_layer_info.tp_address.to_bitstring(),
            asn1_transport_layer_info.gtp_tunnel().transport_layer_address.to_string());
}

TEST(transport_layer_address_test, asn1_to_ipv4_transport_layer_address)
{
  asn1::f1ap::up_transport_layer_info_c asn1_transport_layer_info;
  asn1_transport_layer_info.set_gtp_tunnel().gtp_teid.from_number(0x1);
  asn1_transport_layer_info.set_gtp_tunnel().transport_layer_address.from_string(generate_random_ipv4_bitstring());

  up_transport_layer_info up_tp_layer_info = asn1_to_up_transport_layer_info(asn1_transport_layer_info);

  ASSERT_EQ(up_tp_layer_info.gtp_teid, int_to_gtp_teid(asn1_transport_layer_info.gtp_tunnel().gtp_teid.to_number()));
  ASSERT_EQ(up_tp_layer_info.tp_address.to_bitstring(),
            asn1_transport_layer_info.gtp_tunnel().transport_layer_address.to_string());
}

TEST(transport_layer_address_test, asn1_to_ipv6_transport_layer_address)
{
  asn1::f1ap::up_transport_layer_info_c asn1_transport_layer_info;
  asn1_transport_layer_info.set_gtp_tunnel().gtp_teid.from_number(0x1);
  asn1_transport_layer_info.set_gtp_tunnel().transport_layer_address.from_string(generate_random_ipv6_bitstring());

  up_transport_layer_info up_tp_layer_info = asn1_to_up_transport_layer_info(asn1_transport_layer_info);

  ASSERT_EQ(up_tp_layer_info.gtp_teid, int_to_gtp_teid(asn1_transport_layer_info.gtp_tunnel().gtp_teid.to_number()));
  ASSERT_EQ(up_tp_layer_info.tp_address.to_bitstring(),
            asn1_transport_layer_info.gtp_tunnel().transport_layer_address.to_string());
}
