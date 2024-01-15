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

#include "lib/mac/mac_ul/mac_ul_sch_pdu.h"
#include "lib/mac/mac_ul/ul_phr.h"
#include "srsran/support/bit_encoding.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

// Test the unpacking function for MAC subPDU with MAC SDU (UL CCCH 48 bits).
TEST(mac_ul_subpdu, decode_ul_ccch_48)
{
  mac_ul_sch_subpdu subpdu;

  // MAC subPDU with:
  // - 8-bit R/LCID MAC subheader.
  // - MAC SDU for UL CCCH 48 bits.
  //
  // |   |   |   |   |   |   |   |   |
  // | R | R |         LCID          |  Octet 1
  // |      MAC SDU for UL CCCH      |  Octet 2
  // |      MAC SDU for UL CCCH      |  Octet 3
  //               ...
  // |      MAC SDU for UL CCCH      |  Octet 7

  // R/LCID MAC subheader = R|R|LCID = 0x34  ->  LCID=52
  // MAC SDU (UL CCCH)    = {0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06}  (Random 6B sequence)
  byte_buffer msg({0x34, 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06});
  ASSERT_TRUE(subpdu.unpack(msg));

  // Test expected length
  ASSERT_EQ(6, subpdu.sdu_length()) << "Wrong SDU length for UL CCCH (6 bytes)";
  ASSERT_EQ(7, subpdu.total_length()) << "Wrong subPDU length for UL CCCH (1 B header + 6B SDU)";
  ASSERT_EQ(subpdu.payload(), byte_buffer_view(msg).view(1, subpdu.sdu_length()));
  fmt::print("subPDU: {}\n", subpdu);
}

// Test the unpacking function for MAC subPDU with MAC SDU (UL CCCH 64 bits).
TEST(mac_ul_subpdu, decode_ul_ccch_64)
{
  mac_ul_sch_subpdu subpdu;

  // MAC subPDU with:
  // - 8-bit R/LCID MAC subheader.
  // - MAC SDU for UL CCCH 64 bits.
  //
  // |   |   |   |   |   |   |   |   |
  // | R | R |         LCID          |  Octet 1
  // |      MAC SDU for UL CCCH      |  Octet 2
  // |      MAC SDU for UL CCCH      |  Octet 3
  //               ...
  // |      MAC SDU for UL CCCH      |  Octet 9

  // R/LCID MAC subheader = R|R|LCID = 0x00  ->  LCID = 0
  // MAC SDU (UL CCCH)    = {0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06, 0x13, 0x54}  (Random 8B sequence)
  byte_buffer msg({0x00, 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06, 0x13, 0x54});
  ASSERT_TRUE(subpdu.unpack(msg));

  // Test expected length.
  ASSERT_EQ(8, subpdu.sdu_length()) << "Wrong SDU length for UL CCCH (8 bytes)";
  ASSERT_EQ(9, subpdu.total_length()) << "Wrong subPDU length for UL CCCH (1 B header + 8B SDU)";
  ASSERT_EQ(subpdu.payload(), byte_buffer_view(msg).view(1, subpdu.sdu_length()));
  fmt::print("subPDU: {}\n", subpdu);
}

// Test the unpacking function for MAC subPDU with MAC CE (Short BSR).
TEST(mac_ul_subpdu, decode_short_bsr)
{
  mac_ul_sch_subpdu subpdu;

  // MAC subPDU with:
  // - 8-bit R/LCID MAC subheader.
  // - MAC CE with Short BSR.
  //
  // |   |   |   |   |   |   |   |   |
  // | R | R |         LCID          |  Octet 1
  // |  LCG ID   |    Buffer Size    |  Octet 2

  // R/LCID MAC subheader = R|R|LCID = 0x3d or LCID=61
  // MAC CE Short BSR = {0x59}  LCG ID = 2, Buffer size = 25
  byte_buffer msg({0x3d, 0x59});
  ASSERT_TRUE(subpdu.unpack(msg));

  // Test expected length.
  ASSERT_EQ(1, subpdu.sdu_length()) << "Wrong MAC CE Long BSR (1 byte)";
  ASSERT_EQ(2, subpdu.total_length()) << "Wrong subPDU length for MAC CE Short BSR (1B header + 1B MAC-CE)";
  ASSERT_EQ(subpdu.payload(), byte_buffer_view(msg).view(1, subpdu.sdu_length()));

  lcg_bsr_report bsr_report = decode_sbsr(subpdu.payload());
  ASSERT_EQ(2U, bsr_report.lcg_id);
  ASSERT_EQ(25U, bsr_report.buffer_size);

  fmt::print("subPDU: {}\n", subpdu);
}

// Test the unpacking function for MAC subPDU with MAC CE (Short Truncated BSR).
TEST(mac_ul_subpdu, decode_short_trunc_bsr)
{
  mac_ul_sch_subpdu subpdu;

  // MAC subPDU with:
  // - 8-bit R/LCID MAC subheader.
  // - MAC CE with Short Truncated BSR.
  //
  // |   |   |   |   |   |   |   |   |
  // | R | R |         LCID          |  Octet 1
  // |  LCG ID   |    Buffer Size    |  Octet 2

  // R/LCID MAC subheader = R|R|LCID = 0x3b or LCID=59
  // MAC CE Short BSR = {0xae}  LCG ID = 2, Buffer size = 14
  byte_buffer msg({0x3b, 0xae});
  ASSERT_TRUE(subpdu.unpack(msg));

  // Test expected length.
  ASSERT_EQ(1, subpdu.sdu_length()) << "Wrong MAC CE Long BSR (1 byte)";
  ASSERT_EQ(2, subpdu.total_length()) << "Wrong subPDU length for MAC CE Short Trunc. BSR (1B header + 1B MAC-CE)";
  ASSERT_EQ(subpdu.payload(), byte_buffer_view(msg).view(1, subpdu.sdu_length()));

  lcg_bsr_report bsr_report = decode_sbsr(subpdu.payload());
  ASSERT_EQ(5U, bsr_report.lcg_id);
  ASSERT_EQ(14U, bsr_report.buffer_size);

  fmt::print("subPDU: {}\n", subpdu);
}

// Test the unpacking function for MAC subPDU with MAC CE (Long BSR for 2 LCGs).
TEST(mac_ul_subpdu, decode_long_bsr_with_2_lcgs)
{
  mac_ul_sch_subpdu subpdu;

  // MAC subPDU with:
  // - 16-bit R/F/LCID/L MAC subheader.
  // - MAC CE with Long BSR.
  //
  // |   |   |   |   |   |   |   |   |
  // | R | F |         LCID          |  Octet 1
  // |               L               |  Octet 2
  // |    LCG7, LCG6, ..., LCG0      |  Octet 3
  // |         Buffer size 1         |  Octet 4
  //               ...
  // |         Buffer size m         |  Octet 3+m

  // R/F/LCID/L MAC subheader = 0x3b - F=0, LCID=62, L=3
  // MAC CE Long BSR = {0x81, 0xd9, 0xab}  LCG_ID_7=true, LCG_ID_0= true
  // Buffer size(LCG_ID_0) = 217 (0xd9), Buffer size(LCG_ID_7) = 171 (0xab)
  byte_buffer msg({0x3e, 0x03, 0x81, 0xd9, 0xab});
  ASSERT_TRUE(subpdu.unpack(msg));

  // Test expected length.
  ASSERT_EQ(3, subpdu.sdu_length()) << "Wrong MAC CE Long BSR (3 bytes)";
  ASSERT_EQ(5, subpdu.total_length()) << "Wrong subPDU length for MAC CE Long BSR (2B header + 3B MAC-CE)";
  ASSERT_EQ(subpdu.payload(), byte_buffer_view(msg).view(2, subpdu.sdu_length()));

  expected<long_bsr_report> lbsr = decode_lbsr(bsr_format::LONG_BSR, subpdu.payload());
  ASSERT_TRUE(lbsr.has_value());

  const std::array<lcg_id_t, 2> expected_lcg_ids      = {uint_to_lcg_id(0U), uint_to_lcg_id(7U)};
  const std::array<unsigned, 2> expected_buffer_sizes = {217U, 171U};
  for (unsigned i = 0; i != 2; ++i) {
    ASSERT_EQ(expected_lcg_ids[i], lbsr.value().list[i].lcg_id);
    ASSERT_EQ(expected_buffer_sizes[i], lbsr.value().list[i].buffer_size);
  }

  fmt::print("subPDU: {}\n", subpdu);
}

// Test the unpacking function for MAC subPDU with MAC CE (Long BSR for 4 LCGs).
TEST(mac_ul_subpdu, decode_long_bsr_with_4_lcgs)
{
  mac_ul_sch_subpdu subpdu;

  // MAC subPDU with:
  // - 16-bit R/F/LCID/L MAC subheader.
  // - MAC CE with Long BSR.
  //
  // |   |   |   |   |   |   |   |   |
  // | R | F |         LCID          |  Octet 1
  // |               L               |  Octet 2
  // |    LCG7, LCG6, ..., LCG0      |  Octet 3
  // |         Buffer size 1         |  Octet 4
  //               ...
  // |         Buffer size m         |  Octet 3+m

  // R/F/LCID/L MAC subheader = 0x3e - F=0, LCID=62, L=5
  // MAC CE Long BSR = {0x5a, 0x02, 0x04, 0x05, 0x07}
  // LCG_ID_6=true, LCG_ID_4=true, LCG_ID_3=true, LCG_ID_1=true
  // Buffer size(LCG_ID_1)=2, Buffer size(LCG_ID_3)=4, Buffer size(LCG_ID_4)=5, Buffer size(LCG_ID_6)=7
  byte_buffer msg({0x3e, 0x05, 0x5a, 0x02, 0x04, 0x05, 0x07});
  ASSERT_TRUE(subpdu.unpack(msg));

  // Test expected length.
  ASSERT_EQ(5, subpdu.sdu_length()) << "Wrong MAC CE Long BSR (5 bytes)";
  ASSERT_EQ(7, subpdu.total_length()) << "Wrong subPDU length for MAC CE Long BSR (2B header + 5B MAC-CE)";
  ASSERT_EQ(subpdu.payload(), byte_buffer_view(msg).view(2, subpdu.sdu_length()));

  expected<long_bsr_report> lbsr = decode_lbsr(bsr_format::LONG_BSR, subpdu.payload());
  ASSERT_TRUE(lbsr.has_value());

  const std::array<lcg_id_t, 4> expected_lcgs = {
      uint_to_lcg_id(1), uint_to_lcg_id(3), uint_to_lcg_id(4), uint_to_lcg_id(6)};
  const std::array<unsigned, 4> expected_buffer_sizes = {2, 4, 5, 7};
  for (unsigned i = 0; i != 4; ++i) {
    ASSERT_EQ(expected_lcgs[i], lbsr.value().list[i].lcg_id);
    ASSERT_EQ(expected_buffer_sizes[i], lbsr.value().list[i].buffer_size);
  }

  fmt::print("subPDU: {}\n", subpdu);
}

// Test the unpacking function for MAC subPDU with MAC CE (Long BSR for all LCGs).
TEST(mac_ul_subpdu, decode_long_bsr_with_8_lcgs)
{
  mac_ul_sch_subpdu subpdu;

  // MAC subPDU with:
  // - 16-bit R/F/LCID/L MAC subheader.
  // - MAC CE with Long BSR.
  //
  // |   |   |   |   |   |   |   |   |
  // | R | F |         LCID          |  Octet 1
  // |               L               |  Octet 2
  // |    LCG7, LCG6, ..., LCG0      |  Octet 3
  // |         Buffer size 1         |  Octet 4
  //               ...
  // |         Buffer size m         |  Octet 3+m

  // R/F/LCID/L MAC subheader = 0x3e - F=0, LCID=62, L=9
  // MAC CE Long BSR = {0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}
  // All LCG_ID set to true (0xff)
  // Buffer size(LCG_ID_0)=1, ..., Buffer size(LCG_ID_n)=n+1, ... , Buffer size(LCG_ID_7)=8
  byte_buffer msg({0x3e, 0x9, 0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08});
  ASSERT_TRUE(subpdu.unpack(msg));

  // Test expected length.
  ASSERT_EQ(9, subpdu.sdu_length()) << "Wrong MAC CE Long BSR (9 bytes)";
  ASSERT_EQ(11, subpdu.total_length()) << "Wrong subPDU length for MAC CE Long BSR (2B header + 9B MAC-CE)";
  ASSERT_EQ(subpdu.payload(), byte_buffer_view(msg).view(2, subpdu.sdu_length()));

  expected<long_bsr_report> lbsr = decode_lbsr(bsr_format::LONG_BSR, subpdu.payload());

  for (unsigned i = 0; i != 8; ++i) {
    ASSERT_EQ(uint_to_lcg_id(i), lbsr.value().list[i].lcg_id);
    ASSERT_EQ(i + 1, lbsr.value().list[i].buffer_size);
  }

  fmt::print("subPDU: {}\n", subpdu);
}

// Test the unpacking function for MAC subPDU with MAC CE (Long Truncated BSR).
TEST(mac_ul_subpdu, decode_long_trunc_bsr)
{
  mac_ul_sch_subpdu subpdu;

  // MAC subPDU with:
  // - 16-bit R/F/LCID/L MAC subheader.
  // - MAC CE with Long Truncated BSR.
  //
  // |   |   |   |   |   |   |   |   |
  // | R | F |         LCID          |  Octet 1
  // |               L               |  Octet 2
  // |    LCG7, LCG6, ..., LCG0      |  Octet 3
  // |         Buffer size 1         |  Octet 4
  //               ...
  // |         Buffer size m         |  Octet 3+m

  // R/F/LCID/L MAC subheader = 0x3c - F=0, LCID=61, L=3
  // MAC CE Long Truncated  BSR = {0x81, 0xd9, 0xab}  LCG_ID_7=true, LCG_ID_0= true
  // Buffer size_1 = 217 (0xd9), Buffer size_2 = 171 (0xab)
  byte_buffer msg({0x3c, 0x3, 0x45, 0xd9, 0xab});
  ASSERT_TRUE(subpdu.unpack(msg));

  // Test expected length.
  ASSERT_EQ(3, subpdu.sdu_length()) << "Wrong MAC CE Long Trunc BSR (3 bytes)";
  ASSERT_EQ(5, subpdu.total_length()) << "Wrong subPDU length for MAC CE Long Trunc BSR (2B header + 3B MAC-CE)";
  ASSERT_EQ(subpdu.payload(), byte_buffer_view(msg).view(2, subpdu.sdu_length()));

  unsigned lcg_6_flag = (bool)(subpdu.payload()[0] & 0b01000000U);
  ASSERT_TRUE(lcg_6_flag);
  unsigned lcg_2_flag = (bool)(subpdu.payload()[0] & 0b00000100U);
  ASSERT_TRUE(lcg_2_flag);
  unsigned lcg_0_flag = (bool)(subpdu.payload()[0] & 0b00000001U);
  ASSERT_TRUE(lcg_0_flag);
  unsigned buffer_size_1 = subpdu.payload()[1];
  ASSERT_EQ(217U, buffer_size_1);
  unsigned buffer_size_2 = subpdu.payload()[2];
  ASSERT_EQ(171U, buffer_size_2);
  fmt::print("subPDU: {}\n", subpdu);
}

// Test the unpacking function for MAC subPDU with MAC CE (C-RNTI).
TEST(mac_ul_subpdu, decode_crnti_ce)
{
  mac_ul_sch_subpdu subpdu;

  // MAC subPDU with:
  // - 8-bit R/LCID MAC subheader.
  // - MAC CE with C-RNTI.
  //
  // |   |   |   |   |   |   |   |   |
  // | R | R |         LCID          |  Octet 1
  // |             RNTI              |  Octet 2
  // |             RNTI              |  Octet 3

  // R/LCID MAC subheader = R|R|LCID = 0x3a or LCID=61
  // MAC CE C-RNTI = {0x46, 0x01}, RNTI=0x4601
  byte_buffer msg({0x3a, 0x46, 0x01});
  ASSERT_TRUE(subpdu.unpack(msg));

  // Test expected length.
  ASSERT_EQ(2, subpdu.sdu_length()) << "Wrong MAC CE C-RNTI (2 bytes)";
  ASSERT_EQ(3, subpdu.total_length()) << "Wrong subPDU length for MAC CE C-RNTI (1B header + 2B MAC-CE)";
  ASSERT_EQ(subpdu.payload(), byte_buffer_view(msg).view(1, subpdu.sdu_length()));

  rnti_t rnti = decode_crnti_ce(subpdu.payload());
  ASSERT_EQ(to_rnti(0x4601), rnti);

  fmt::print("subPDU: {}\n", subpdu);
}

// Test the unpacking function for MAC subPDU with MAC CE (Single Entry PHR).
TEST(mac_ul_subpdu, decode_single_entry_phr)
{
  mac_ul_sch_subpdu subpdu;

  // MAC subPDU with:
  // - 8-bit R/LCID MAC subheader.
  // - MAC CE with Single Entry PHR.
  //
  // |   |   |   |   |   |   |   |   |
  // | R | R |         LCID          |  Octet 1
  // | R | R |          PH           |  Octet 2
  // | R | R |        P_CMAX         |  Octet 3

  // R/LCID MAC subheader = R|R|LCID = 0x39 or LCID=57
  // MAC CE SE PHR = {0x27, 0x2f}

  byte_buffer msg({0x39, 0x27, 0x2f});
  ASSERT_TRUE(subpdu.unpack(msg));

  // Test expected length.
  ASSERT_EQ(2, subpdu.sdu_length()) << "Wrong SDU length for MAC CE SE PHR (2 bytes)";
  ASSERT_EQ(3, subpdu.total_length()) << "Wrong subPDU length for MAC CE SE PHR (1 B header + 2B SDU)";
  ASSERT_EQ(subpdu.payload(), byte_buffer_view(msg).view(1, subpdu.sdu_length()));

  // Test Payload.
  const phr_report se_phr         = decode_se_phr(subpdu.payload());
  const auto       expected_ph    = ph_db_range(6, 7);
  const auto       expected_pcmax = p_cmax_dbm_range(17, 18);
  ASSERT_EQ(expected_ph, se_phr.get_se_phr().ph);
  ASSERT_EQ(expected_pcmax, se_phr.get_se_phr().p_cmax);
  ASSERT_EQ(se_phr.get_se_phr().serv_cell_id, to_du_cell_index(0));
  ASSERT_EQ(se_phr.get_se_phr().ph_type, ph_field_type_t::type1);
  fmt::print("subPDU: {}\n", subpdu);
}

// Test the unpacking function for MAC subPDU with padding.
TEST(mac_ul_subpdu, decode_subpdu_padding)
{
  mac_ul_sch_subpdu subpdu;

  // MAC subPDU with:
  // - 8-bit R/LCID MAC subheader.
  // - MAC CE with padding.
  //
  // |   |   |   |   |   |   |   |   |
  // | R | R |         LCID          |  Octet 1
  // |             0x00              |  Octet 2
  //               ...
  // |             0x00              |  Octet n

  // R/LCID MAC subheader = R|R|LCID = 0x3f or LCID=63
  // Padding = {0x00, 0x00}  - 2B of padding

  byte_buffer msg({0x3f, 0x00, 0x00});
  ASSERT_TRUE(subpdu.unpack(msg));

  // Test expected length.
  ASSERT_EQ(2, subpdu.sdu_length()) << "Wrong SDU length for MAC CE SE PHR (2 bytes)";
  ASSERT_EQ(3, subpdu.total_length()) << "Wrong subPDU length for MAC CE SE PHR (1 B header + 2B SDU)";
  ASSERT_EQ(subpdu.payload(), byte_buffer_view(msg).view(1, subpdu.sdu_length()));

  // Test Payload.
  unsigned padding_1 = subpdu.payload()[0];
  ASSERT_EQ(0x00, padding_1);
  unsigned padding_2 = subpdu.payload()[1];
  ASSERT_EQ(0x00, padding_2);
  fmt::print("subPDU: {}\n", subpdu);
}

TEST(mac_ul_pdu, decode_crnti_ce_and_sbsr)
{
  // PDU = [ subPDU_1 | subPDU_2 ]

  // subPDU_1 = [ R/LCID MAC subheader (1 byte) | MAC CE C-RNTI ]
  // R/LCID MAC subheader = R|R|LCID = 0x3a or LCID=58
  byte_buffer msg = {0x3a, 0x46, 0x1};

  // subPDU_2 = [ R/LCID MAC subheader (1 byte) | MAC CE Short BSR ]
  // R/LCID MAC subheader = R|R|LCID = 0x3d or LCID=63
  ASSERT_TRUE(msg.append({0x3d, 0x59}));

  mac_ul_sch_pdu pdu;
  ASSERT_TRUE(pdu.unpack(msg));
  ASSERT_EQ(pdu.nof_subpdus(), 2);

  // Test expected PDU.
  byte_buffer_view buf_view(msg);
  auto             subpdus = pdu.begin();
  ASSERT_EQ(2, subpdus->sdu_length()) << "Wrong SDU length for MAC CE C-RNTI (2B for payload)";
  ASSERT_EQ(3, subpdus->total_length()) << "Wrong subPDU length for MAC CE C-RNTI (1 B header + 2B Payload)";
  ASSERT_EQ(subpdus->payload(), buf_view.view(1, subpdus->sdu_length()));
  unsigned subpdu2_start = subpdus->total_length();
  subpdus++;
  ASSERT_EQ(1, subpdus->sdu_length()) << "Wrong SDU length for MAC CE Short BSR (1B for payload)";
  ASSERT_EQ(2, subpdus->total_length()) << "Wrong subPDU length for MAC CE Short BSR (1 B header + 1B Payload)";
  ASSERT_EQ(subpdus->payload(), buf_view.view(subpdu2_start + 1, subpdus->sdu_length()));
}

// Test the unpacking function for MAC PDU with 2 MAC SDUs: UL CCCH 48 bits and padding.
TEST(mac_ul_pdu, decode_ul_ccch_and_padding)
{
  mac_ul_sch_pdu pdu;

  // PDU = [ subPDU_1 | subPDU_2 ]
  // subPDU_1 = [ R/LCID MAC subheader (1 byte) | MAC SDU for UL CCCH (6bytes) ]
  // R/LCID MAC subheader = R|R|LCID = 0x34 or LCID=52
  // MAC SDU (UL CCCH)    = {0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06}  (Random 6B sequence)
  // subPDU_2 = [ R/LCID MAC subheader (1 byte) | padding (3bytes) ]
  // R/LCID MAC subheader = R|R|LCID = 0x3f or LCID=63
  byte_buffer msg({0x34, 0x1e, 0x4f, 0xc0, 0x4f, 0xa6, 0x06, 0x3f, 0x00, 0x00, 0x00});
  ASSERT_TRUE(pdu.unpack(msg));

  // Test expected PDU.
  auto subpdus = pdu.begin();
  ASSERT_EQ(6, subpdus->sdu_length()) << "Wrong SDU length for UL CCCH (6 bytes)";
  ASSERT_EQ(7, subpdus->total_length()) << "Wrong subPDU length for UL CCCH (1 B header + 6B SDU)";
  ASSERT_EQ(subpdus->payload(), byte_buffer_view(msg).view(1, subpdus->sdu_length()));

  subpdus++;
  ASSERT_EQ(3, subpdus->sdu_length()) << "Wrong padding length (3 bytes)";
  ASSERT_EQ(4, subpdus->total_length()) << "Wrong subPDU length padding (1 B header + 3B padding)";
  fmt::print("PDU:\n  Hex={:X}\n  subPDUs: {}\n", fmt::join(msg.begin(), msg.end(), ""), pdu);
}

TEST(mac_ul_pdu, decode_short_sdu)
{
  // PDU = [ subPDU_1 ]
  // MAC subPDU_1 with:
  // - 16-bit R|F|LCID|L MAC subheader (2 octets), with F=0, 1<=LCID<=32, L<256.
  // - MAC SDU (L octets).
  //
  // |   |   |   |   |   |   |   |   |
  // | R | F |         LCID          |  Octet 1
  // |              L                |  Octet 2
  // |            PAYLOAD            |  Octet 3
  //               ...
  // |            PAYLOAD            |  Octet L + 2

  size_t      L    = test_rgen::uniform_int<unsigned>(1, 255);
  lcid_t      lcid = uint_to_lcid(test_rgen::uniform_int<uint8_t>(LCID_SRB1, LCID_MAX_DRB));
  byte_buffer payload{test_rgen::random_vector<uint8_t>(L)};

  byte_buffer msg;
  bit_encoder enc(msg);
  enc.pack(0, 1);    // R.
  enc.pack(0, 1);    // F.
  enc.pack(lcid, 6); // LCID.
  enc.pack(L, 8);    // L.
  ASSERT_TRUE(msg.append(payload));

  mac_ul_sch_pdu pdu;
  ASSERT_TRUE(pdu.unpack(msg));
  ASSERT_EQ(pdu.nof_subpdus(), 1);

  ASSERT_EQ(pdu.subpdu(0).total_length(), msg.length());
  ASSERT_EQ(pdu.subpdu(0).lcid(), lcid);
  ASSERT_EQ(pdu.subpdu(0).sdu_length(), L);
  ASSERT_EQ(pdu.subpdu(0).payload(), payload);
}

TEST(mac_ul_pdu, decode_long_sdu)
{
  // PDU = [ subPDU_1 ]
  // MAC subPDU_1 with:
  // - 16-bit R|F|LCID|L MAC subheader (3 octets), with F=1, 1<=LCID<=32, L>=256.
  // - MAC SDU (L octets).
  //
  // |   |   |   |   |   |   |   |   |
  // | R | F |         LCID          |  Octet 1
  // |              L                |  Octet 2
  // |              L                |  Octet 3
  // |            PAYLOAD            |  Octet 4
  //               ...
  // |            PAYLOAD            |  Octet L + 3

  size_t      L    = test_rgen::uniform_int<unsigned>(256, 1000);
  lcid_t      lcid = uint_to_lcid(test_rgen::uniform_int<uint8_t>(LCID_SRB1, LCID_MAX_DRB));
  byte_buffer payload{test_rgen::random_vector<uint8_t>(L)};

  byte_buffer msg;
  bit_encoder enc(msg);
  enc.pack(0, 1);    // R.
  enc.pack(1, 1);    // F.
  enc.pack(lcid, 6); // LCID.
  enc.pack(L, 16);   // L (2 octets).
  ASSERT_TRUE(msg.append(payload));

  mac_ul_sch_pdu pdu;
  ASSERT_TRUE(pdu.unpack(msg));
  ASSERT_EQ(pdu.nof_subpdus(), 1);

  ASSERT_EQ(pdu.subpdu(0).total_length(), msg.length());
  ASSERT_EQ(pdu.subpdu(0).lcid(), lcid);
  ASSERT_EQ(pdu.subpdu(0).sdu_length(), L);
  ASSERT_EQ(pdu.subpdu(0).payload(), payload);
}

// Test the unpacking of an invalid MAC UL PDU.
TEST(mac_ul_pdu, decode_bad_mac_pdu)
{
  mac_ul_sch_pdu pdu;

  byte_buffer msg(std::vector<uint8_t>(11, 0x0));
  ASSERT_FALSE(pdu.unpack(msg));
  ASSERT_EQ(pdu.nof_subpdus(), 0);
}

TEST(mac_ul_pdu, handle_the_case_when_a_pdu_has_too_many_subpdus)
{
  constexpr unsigned nof_subpdus_per_pdu = 1000;

  size_t      L    = 1;
  lcid_t      lcid = srsran::LCID_SRB1;
  byte_buffer payload{test_rgen::random_vector<uint8_t>(L)};

  byte_buffer msg;
  bit_encoder enc(msg);

  for (unsigned i = 0; i != nof_subpdus_per_pdu; ++i) {
    enc.pack(0, 1);    // R.
    enc.pack(0, 1);    // F.
    enc.pack(lcid, 6); // LCID.
    enc.pack(L, 8);    // L.
    ASSERT_TRUE(msg.append(payload));
  }

  mac_ul_sch_pdu pdu;
  ASSERT_TRUE(pdu.unpack(msg)); // Should not crash.
  ASSERT_GT(pdu.nof_subpdus(), 0);
}

TEST(mac_ul_pdu, handle_the_case_when_pdu_length_is_too_short_to_decode_length_prefix)
{
  size_t      L    = 1;
  lcid_t      lcid = srsran::LCID_SRB1;
  byte_buffer payload{test_rgen::random_vector<uint8_t>(L)};

  byte_buffer msg;
  bit_encoder enc(msg);
  enc.pack(0, 1);    // R.
  enc.pack(1, 1);    // F.
  enc.pack(lcid, 6); // LCID.
  enc.pack(L, 8);    // L (too short for F=1).

  mac_ul_sch_pdu pdu;
  ASSERT_FALSE(pdu.unpack(msg)); // Should not crash.
}

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("MAC", true).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
