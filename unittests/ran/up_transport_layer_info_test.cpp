/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/ran/up_transport_layer_info.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;

std::string create_random_ipv4_string()
{
  std::vector<uint8_t> nums = test_rgen::random_vector<uint8_t>(4);
  return fmt::format("{}.{}.{}.{}", nums[0], nums[1], nums[2], nums[3]);
}

TEST(transport_layer_address_test, conversion_to_ipv4_string)
{
  std::string             ipv4_str = create_random_ipv4_string();
  transport_layer_address addr{ipv4_str};
  ASSERT_EQ(addr.to_string(), ipv4_str);
  ASSERT_EQ(fmt::format("{}", addr), ipv4_str);
}

TEST(transport_layer_address_test, conversion_to_ipv4_network_number)
{
  transport_layer_address addr;
  ASSERT_EQ(addr.from_number(0xdeadbeef).to_number(), 0xdeadbeef);
}

TEST(transport_layer_address_test, address_comparison)
{
  std::string             ipv4_str1 = create_random_ipv4_string();
  std::string             ipv4_str2 = create_random_ipv4_string();
  transport_layer_address addr1{ipv4_str1}, addr2{ipv4_str2};
  ASSERT_EQ(addr1, ipv4_str1);
  ASSERT_EQ(addr2, ipv4_str2);
  ASSERT_EQ(addr1 == ipv4_str1, addr2 == ipv4_str2);
}
