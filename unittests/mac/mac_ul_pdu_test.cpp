/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/mac/mac_ul/mac_ul_sch_pdu.h"
#include <gtest/gtest.h>

using namespace srsgnb;

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

  // TODO: Fix the == operator for byte_buffer_view, as it gives a SIGSEGV
  // byte_buffer_view comparison = byte_buffer_view(msg).view(1, 6);
  // byte_buffer_view original = subpdu.payload();
  // ASSERT_TRUE(original == comparison);

  // Test expected length
  ASSERT_EQ(6, subpdu.sdu_length()) << "Wrong SDU length for UL CCCH (6 bytes)";
  ASSERT_EQ(7, subpdu.total_length()) << "Wrong subPDU length for UL CCCH (1 B header + 6B SDU)";
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

  long_bsr_report lbsr = decode_lbsr(bsr_format::LONG_BSR, subpdu.payload());

  bool lcg_7_flag = (bool)(lbsr.bitmap & 0b10000000U);
  ASSERT_TRUE(lcg_7_flag);
  bool lcg_0_flag = (bool)(lbsr.bitmap & 0b00000001U);
  ASSERT_TRUE(lcg_0_flag);

  unsigned buffer_size_lcg0 = lbsr.list[0].buffer_size;
  ASSERT_EQ(217U, buffer_size_lcg0);
  unsigned buffer_size_lcg7 = lbsr.list[1].buffer_size;
  ASSERT_EQ(171U, buffer_size_lcg7);

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

  long_bsr_report lbsr = decode_lbsr(bsr_format::LONG_BSR, subpdu.payload());

  bool lcg_6_flag = (bool)(lbsr.bitmap & 0b01000000U);
  ASSERT_TRUE(lcg_6_flag);
  bool lcg_4_flag = (bool)(lbsr.bitmap & 0b00010000U);
  ASSERT_TRUE(lcg_4_flag);
  bool lcg_3_flag = (bool)(lbsr.bitmap & 0b00001000U);
  ASSERT_TRUE(lcg_3_flag);
  bool lcg_1_flag = (bool)(lbsr.bitmap & 0b00000010U);
  ASSERT_TRUE(lcg_1_flag);

  unsigned buffer_size_lcg1 = lbsr.list[0].buffer_size;
  ASSERT_EQ(2U, buffer_size_lcg1);
  unsigned buffer_size_lcg3 = lbsr.list[1].buffer_size;
  ASSERT_EQ(4U, buffer_size_lcg3);
  unsigned buffer_size_lcg4 = lbsr.list[2].buffer_size;
  ASSERT_EQ(5U, buffer_size_lcg4);
  unsigned buffer_size_lcg6 = lbsr.list[3].buffer_size;
  ASSERT_EQ(7U, buffer_size_lcg6);

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

  long_bsr_report lbsr = decode_lbsr(bsr_format::LONG_BSR, subpdu.payload());

  bool lcg_7_flag = (bool)(lbsr.bitmap & 0b10000000U);
  ASSERT_TRUE(lcg_7_flag);
  bool lcg_6_flag = (bool)(lbsr.bitmap & 0b01000000U);
  ASSERT_TRUE(lcg_6_flag);
  bool lcg_5_flag = (bool)(lbsr.bitmap & 0b00100000U);
  ASSERT_TRUE(lcg_5_flag);
  bool lcg_4_flag = (bool)(lbsr.bitmap & 0b00010000U);
  ASSERT_TRUE(lcg_4_flag);
  bool lcg_3_flag = (bool)(lbsr.bitmap & 0b00001000U);
  ASSERT_TRUE(lcg_3_flag);
  bool lcg_2_flag = (bool)(lbsr.bitmap & 0b00000100U);
  ASSERT_TRUE(lcg_2_flag);
  bool lcg_1_flag = (bool)(lbsr.bitmap & 0b00000010U);
  ASSERT_TRUE(lcg_1_flag);
  bool lcg_0_flag = (bool)(lbsr.bitmap & 0b00000001U);
  ASSERT_TRUE(lcg_0_flag);

  unsigned buffer_size_lcg0 = lbsr.list[0].buffer_size;
  ASSERT_EQ(1U, buffer_size_lcg0);
  unsigned buffer_size_lcg1 = lbsr.list[1].buffer_size;
  ASSERT_EQ(2U, buffer_size_lcg1);
  unsigned buffer_size_lcg2 = lbsr.list[2].buffer_size;
  ASSERT_EQ(3U, buffer_size_lcg2);
  unsigned buffer_size_lcg3 = lbsr.list[3].buffer_size;
  ASSERT_EQ(4U, buffer_size_lcg3);
  unsigned buffer_size_lcg4 = lbsr.list[4].buffer_size;
  ASSERT_EQ(5U, buffer_size_lcg4);
  unsigned buffer_size_lcg5 = lbsr.list[5].buffer_size;
  ASSERT_EQ(6U, buffer_size_lcg5);
  unsigned buffer_size_lcg6 = lbsr.list[6].buffer_size;
  ASSERT_EQ(7U, buffer_size_lcg6);
  unsigned buffer_size_lcg7 = lbsr.list[7].buffer_size;
  ASSERT_EQ(8U, buffer_size_lcg7);

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

  rnti_t rnti = decode_crnti_ce(subpdu.payload());
  ASSERT_EQ(to_rnti(0x4601), rnti);

  fmt::print("subPDU: {}\n", subpdu);
}

// Test the unpacking function for MAC subPDU with MAC CE (Single Entry PHR).
TEST(mac_ul_subpdu, decode_se_phr)
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

  // Test Payload.
  unsigned ph = subpdu.payload()[0] & 0b00111111U;
  ASSERT_EQ(0x27, ph);
  unsigned p_cmax = subpdu.payload()[1] & 0b00111111U;
  ASSERT_EQ(0x2f, p_cmax);
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

  // Test Payload.
  unsigned padding_1 = subpdu.payload()[0];
  ASSERT_EQ(0x00, padding_1);
  unsigned padding_2 = subpdu.payload()[1];
  ASSERT_EQ(0x00, padding_2);
  fmt::print("subPDU: {}\n", subpdu);
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

  // Test expected PDU length.
  auto subpdus = pdu.begin();
  ASSERT_EQ(6, subpdus->sdu_length()) << "Wrong SDU length for UL CCCH (6 bytes)";
  ASSERT_EQ(7, subpdus->total_length()) << "Wrong subPDU length for UL CCCH (1 B header + 6B SDU)";

  subpdus++;
  ASSERT_EQ(3, subpdus->sdu_length()) << "Wrong padding length (3 bytes)";
  ASSERT_EQ(4, subpdus->total_length()) << "Wrong subPDU length padding (1 B header + 3B padding)";
  fmt::print("PDU:\n  Hex={:X}\n  subPDUs: {}\n", fmt::join(msg.begin(), msg.end(), ""), pdu);
}

// Test the unpacking of an invalid MAC UL PDU.
TEST(mac_ul_pdu, decode_bad_mac_pdu)
{
  mac_ul_sch_pdu pdu;

  byte_buffer msg(std::vector<uint8_t>(11, 0x0));
  ASSERT_FALSE(pdu.unpack(msg));
  ASSERT_EQ(pdu.nof_subpdus(), 0);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);

  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  srslog::init();

  return RUN_ALL_TESTS();
}
