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

#include "lib/f1ap/common/asn1_helpers.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/nr_cgi_helpers.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

/// Test PLMN decoding
TEST(f1ap_asn1_helpers_test, test_ngi_converter_for_valid_plmn)
{
  // use known a PLMN
  asn1::f1ap::nr_cgi_s asn1_cgi;
  asn1_cgi.plmn_id.from_string("00f110"); // 001.01
  asn1_cgi.nr_cell_id.from_number(6576);

  // convert to internal NGI representation
  nr_cell_global_id_t ngi = cgi_from_asn1(asn1_cgi);

  ASSERT_TRUE(srsran::config_helpers::is_valid(ngi));
  ASSERT_EQ(0xf001, ngi.mcc);        // BCD-encoded MCC
  ASSERT_EQ(0xff01, ngi.mnc);        // BCD-encoded MNC
  ASSERT_EQ("00101", ngi.plmn);      // human-readable PLMN
  ASSERT_EQ("00f110", ngi.plmn_hex); // hex-encoded PLMN (like above)
}

TEST(f1ap_asn1_helpers_test, test_ngi_converter_for_invalid_plmn)
{
  // use known a PLMN
  asn1::f1ap::nr_cgi_s asn1_cgi;
  asn1_cgi.plmn_id.from_string("00f000"); // 000.00
  asn1_cgi.nr_cell_id.from_number(6576);

  // convert to internal NGI representation
  nr_cell_global_id_t ngi = cgi_from_asn1(asn1_cgi);

  ASSERT_FALSE(srsran::config_helpers::is_valid(ngi));
  ASSERT_EQ(0xf000, ngi.mcc);        // BCD-encoded MCC
  ASSERT_EQ(0xff00, ngi.mnc);        // BCD-encoded MNC
  ASSERT_EQ("00000", ngi.plmn);      // human-readable PLMN
  ASSERT_EQ("00f000", ngi.plmn_hex); // hex-encoded PLMN (like above)
}

static std::string create_random_ipv4_string()
{
  std::vector<uint8_t> nums = test_rgen::random_vector<uint8_t>(4);
  return fmt::format("{}.{}.{}.{}", nums[0], nums[1], nums[2], nums[3]);
}

static std::string create_random_ipv6_string()
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

static std::string generate_random_ipv4_bitstring()
{
  uint32_t    random_number = test_rgen::uniform_int<uint32_t>();
  std::string bitstr        = fmt::format("{:032b}", random_number);

  return bitstr;
}

static std::string generate_random_ipv6_bitstring()
{
  std::string bitstr;

  for (int i = 0; i < 2; i++) { // we need 128 bits for ipv6
    uint64_t random_number = test_rgen::uniform_int<uint64_t>();
    bitstr                 = bitstr + fmt::format("{:064b}", random_number);
  }

  return bitstr;
}

static uint32_t generate_gtp_teid()
{
  return test_rgen::uniform_int<uint32_t>();
}

TEST(f1ap_asn1_helpers_test, test_up_transport_layer_converter)
{
  up_transport_layer_info up_tp_layer_info = {transport_layer_address{create_random_ipv4_string()},
                                              int_to_gtpu_teid(0x1)};

  asn1::f1ap::up_transport_layer_info_c asn1_transport_layer_info;

  up_transport_layer_info_to_asn1(asn1_transport_layer_info, up_tp_layer_info);

  ASSERT_EQ(up_tp_layer_info.gtp_teid, int_to_gtpu_teid(asn1_transport_layer_info.gtp_tunnel().gtp_teid.to_number()));
  ASSERT_EQ(up_tp_layer_info.tp_address.to_bitstring(),
            asn1_transport_layer_info.gtp_tunnel().transport_layer_address.to_string());
}

TEST(transport_layer_address_test, ipv6_transport_layer_address_to_asn1)
{
  up_transport_layer_info up_tp_layer_info = {transport_layer_address{create_random_ipv6_string()},
                                              int_to_gtpu_teid(0x1)};

  asn1::f1ap::up_transport_layer_info_c asn1_transport_layer_info;

  up_transport_layer_info_to_asn1(asn1_transport_layer_info, up_tp_layer_info);

  ASSERT_EQ(up_tp_layer_info.gtp_teid, int_to_gtpu_teid(asn1_transport_layer_info.gtp_tunnel().gtp_teid.to_number()));
  ASSERT_EQ(up_tp_layer_info.tp_address.to_bitstring(),
            asn1_transport_layer_info.gtp_tunnel().transport_layer_address.to_string());
}

TEST(transport_layer_address_test, asn1_to_ipv4_transport_layer_address)
{
  asn1::f1ap::up_transport_layer_info_c asn1_transport_layer_info;
  asn1_transport_layer_info.set_gtp_tunnel().gtp_teid.from_number(generate_gtp_teid());
  asn1_transport_layer_info.set_gtp_tunnel().transport_layer_address.from_string(generate_random_ipv4_bitstring());

  // ASN1 -> internal representation.
  up_transport_layer_info up_tp_layer_info = asn1_to_up_transport_layer_info(asn1_transport_layer_info);

  ASSERT_EQ(up_tp_layer_info.gtp_teid, int_to_gtpu_teid(asn1_transport_layer_info.gtp_tunnel().gtp_teid.to_number()));
  ASSERT_EQ(up_tp_layer_info.tp_address.to_bitstring(),
            asn1_transport_layer_info.gtp_tunnel().transport_layer_address.to_string());
}

TEST(transport_layer_address_test, asn1_to_ipv6_transport_layer_address)
{
  asn1::f1ap::up_transport_layer_info_c asn1_transport_layer_info;
  asn1_transport_layer_info.set_gtp_tunnel().gtp_teid.from_number(0x1);
  asn1_transport_layer_info.set_gtp_tunnel().transport_layer_address.from_string(generate_random_ipv6_bitstring());

  up_transport_layer_info up_tp_layer_info = asn1_to_up_transport_layer_info(asn1_transport_layer_info);

  ASSERT_EQ(up_tp_layer_info.gtp_teid, int_to_gtpu_teid(asn1_transport_layer_info.gtp_tunnel().gtp_teid.to_number()));
  ASSERT_EQ(up_tp_layer_info.tp_address.to_bitstring(),
            asn1_transport_layer_info.gtp_tunnel().transport_layer_address.to_string());
}
