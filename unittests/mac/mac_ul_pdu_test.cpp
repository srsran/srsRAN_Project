/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../lib/mac/du_rnti_table.h"
#include "../../lib/mac/mac_config.h"
#include "../../lib/mac/mac_ul/mac_ul_processor.h"
#include "mac_ctrl_test_dummies.h"
#include "srsgnb/scheduler/scheduler_configurator.h"
#include "srsgnb/scheduler/scheduler_feedback_handler.h"
#include "srsgnb/support/executors/manual_task_worker.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

class sched_cfg_dummy_notifier : public sched_configuration_notifier
{
public:
  void on_ue_config_complete(du_ue_index_t ue_index) override {}
  void on_ue_delete_response(du_ue_index_t ue_index) override {}
};

class dummy_scheduler_feedback_handler : public scheduler_feedback_handler
{
public:
  dummy_scheduler_feedback_handler() = default;
  void ul_sr_info(const sr_indication_message& sr) override{};
  void ul_bsr(const ul_bsr_indication_message& bsr) override { last_bsr_msg = bsr; };

  bool verify_bsr_msg(ul_bsr_indication_message& test_usr_msg)
  {
    bool test_msg = last_bsr_msg.cell_index == test_usr_msg.cell_index &&
                    last_bsr_msg.ue_index == test_usr_msg.ue_index && last_bsr_msg.crnti == test_usr_msg.crnti &&
                    last_bsr_msg.type == test_usr_msg.type;
    for (size_t n = 0; n < test_usr_msg.reported_lcgs.size(); ++n) {
      test_msg = test_msg && last_bsr_msg.reported_lcgs[n].lcg_id == test_usr_msg.reported_lcgs[n].lcg_id &&
                 last_bsr_msg.reported_lcgs[n].nof_bytes == test_usr_msg.reported_lcgs[n].nof_bytes;
    }

    return test_msg;
  }

private:
  ul_bsr_indication_message last_bsr_msg;
};

void test_mac_ul_subpdu_ccch_48_unpack()
{
  test_delimit_logger delim{"subPDU - CCCH 48 bits"};
  mac_ul_sch_subpdu   subpdu;

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
  TESTASSERT(subpdu.unpack(msg));

  // TODO: Fix the == operator for byte_buffer_view, as it gives a SIGSEGV
  // byte_buffer_view comparison = byte_buffer_view(msg).view(1, 6);
  // byte_buffer_view original = subpdu.payload();
  // TESTASSERT(original == comparison);

  // Test expected length
  TESTASSERT_EQ(6, subpdu.sdu_length(), "Wrong SDU length for UL CCCH (6 bytes)");
  TESTASSERT_EQ(7, subpdu.total_length(), "Wrong subPDU length for UL CCCH (1 B header + 6B SDU)");
  fmt::print("subPDU: {}\n", subpdu);
}

void test_mac_ul_subpdu_ccch_64_unpack()
{
  test_delimit_logger delim{"subPDU - CCCH 64 bits"};
  mac_ul_sch_subpdu   subpdu;

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
  TESTASSERT(subpdu.unpack(msg));

  // Test expected length
  TESTASSERT_EQ(8, subpdu.sdu_length(), "Wrong SDU length for UL CCCH (8 bytes)");
  TESTASSERT_EQ(9, subpdu.total_length(), "Wrong subPDU length for UL CCCH (1 B header + 8B SDU)");
  fmt::print("subPDU: {}\n", subpdu);
}

void test_mac_ul_subpdu_short_bsr_unpack_decode()
{
  test_delimit_logger delim{"subPDU - Short BSR"};
  mac_ul_sch_subpdu   subpdu;

  // MAC subPDU with:
  // - 8-bit R/LCID MAC subheader.
  // - MAC CE with Short BSR.
  //
  // |   |   |   |   |   |   |   |   |
  // | R | R |         LCID          |  Octet 1
  // |  LCG ID   |    Buffer Size    |  Octet 2

  // R/LCID MAC subheader = R|R|LCID = 0x3d or LCID=61
  // MAC CE Short BSR = {0x59}  LGC ID = 2, Buffer size = 25
  byte_buffer msg({0x3d, 0x59});
  TESTASSERT(subpdu.unpack(msg));

  // Test expected length
  TESTASSERT_EQ(1, subpdu.sdu_length(), "Wrong MAC CE Long BSR (1 byte)");
  TESTASSERT_EQ(2, subpdu.total_length(), "Wrong subPDU length for MAC CE Short BSR (1B header + 1B MAC-CE)");

  lcg_bsr_report bsr_report = decode_sbsr(subpdu.payload());
  TESTASSERT_EQ(2U, bsr_report.lcg_id);
  TESTASSERT_EQ(25U, bsr_report.buffer_size);

  fmt::print("subPDU: {}\n", subpdu);
}

void test_mac_ul_subpdu_short_trunc_bsr_unpack_decode()
{
  test_delimit_logger delim{"subPDU - Short Truncated BSR"};
  mac_ul_sch_subpdu   subpdu;

  // MAC subPDU with:
  // - 8-bit R/LCID MAC subheader.
  // - MAC CE with Short Truncated BSR.
  //
  // |   |   |   |   |   |   |   |   |
  // | R | R |         LCID          |  Octet 1
  // |  LCG ID   |    Buffer Size    |  Octet 2

  // R/LCID MAC subheader = R|R|LCID = 0x3b or LCID=59
  // MAC CE Short BSR = {0xae}  LGC ID = 2, Buffer size = 14
  byte_buffer msg({0x3b, 0xae});
  TESTASSERT(subpdu.unpack(msg));

  // Test expected length
  TESTASSERT_EQ(1, subpdu.sdu_length(), "Wrong MAC CE Long BSR (1 byte)");
  TESTASSERT_EQ(2, subpdu.total_length(), "Wrong subPDU length for MAC CE Short Trunc. BSR (1B header + 1B MAC-CE)");

  lcg_bsr_report bsr_report = decode_sbsr(subpdu.payload());
  TESTASSERT_EQ(5U, bsr_report.lcg_id);
  TESTASSERT_EQ(14U, bsr_report.buffer_size);

  fmt::print("subPDU: {}\n", subpdu);
}

void test_mac_ul_subpdu_long_bsr_unpack_decode()
{
  test_delimit_logger delim{"subPDU - Long BSR"};
  mac_ul_sch_subpdu   subpdu;

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
  // MAC CE Long BSR = {0x81, 0xd9, 0xab}  LGC_ID_7=true, LGC_ID_0= true
  // Buffer size(LGC_ID_0) = 171 (0xd9), Buffer size(LGC_ID_7) = 217 (0xab)
  byte_buffer msg({0x3e, 0x03, 0x81, 0xd9, 0xab});
  TESTASSERT(subpdu.unpack(msg));

  // Test expected length
  TESTASSERT_EQ(3, subpdu.sdu_length(), "Wrong MAC CE Long BSR (3 bytes)");
  TESTASSERT_EQ(5, subpdu.total_length(), "Wrong subPDU length for MAC CE Long BSR (2B header + 3B MAC-CE)");

  long_bsr_report lbsr = decode_lbsr(bsr_format::LONG_BSR, subpdu.payload());

  bool lcg_7_flag = (bool)(lbsr.bitmap & 0b10000000U);
  TESTASSERT_EQ(true, lcg_7_flag);
  bool lcg_0_flag = (bool)(lbsr.bitmap & 0b00000001U);
  TESTASSERT_EQ(true, lcg_0_flag);

  unsigned buffer_size_lcg0 = lbsr.list[0].buffer_size;
  TESTASSERT_EQ(217U, buffer_size_lcg0);
  unsigned buffer_size_lcg7 = lbsr.list[1].buffer_size;
  TESTASSERT_EQ(171U, buffer_size_lcg7);

  fmt::print("subPDU: {}\n", subpdu);
}

void test_mac_ul_subpdu_long_bsr_unpack_decode_II()
{
  test_delimit_logger delim{"subPDU - Long BSR - 4 LCGs"};
  mac_ul_sch_subpdu   subpdu;

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
  // LGC_ID_6=true, LGC_ID_4=true, LGC_ID_3=true, LGC_ID_1=true
  // Buffer size(LGC_ID_1)=2, Buffer size(LGC_ID_3)=4, Buffer size(LGC_ID_4)=5, Buffer size(LGC_ID_6)=7
  byte_buffer msg({0x3e, 0x05, 0x5a, 0x02, 0x04, 0x05, 0x07});
  TESTASSERT(subpdu.unpack(msg));

  // Test expected length
  TESTASSERT_EQ(5, subpdu.sdu_length(), "Wrong MAC CE Long BSR (5 bytes)");
  TESTASSERT_EQ(7, subpdu.total_length(), "Wrong subPDU length for MAC CE Long BSR (2B header + 5B MAC-CE)");

  long_bsr_report lbsr = decode_lbsr(bsr_format::LONG_BSR, subpdu.payload());

  bool lcg_6_flag = (bool)(lbsr.bitmap & 0b01000000U);
  TESTASSERT_EQ(true, lcg_6_flag);
  bool lcg_4_flag = (bool)(lbsr.bitmap & 0b00010000U);
  TESTASSERT_EQ(true, lcg_4_flag);
  bool lcg_3_flag = (bool)(lbsr.bitmap & 0b00001000U);
  TESTASSERT_EQ(true, lcg_3_flag);
  bool lcg_1_flag = (bool)(lbsr.bitmap & 0b00000010U);
  TESTASSERT_EQ(true, lcg_1_flag);

  unsigned buffer_size_lcg1 = lbsr.list[0].buffer_size;
  TESTASSERT_EQ(2U, buffer_size_lcg1);
  unsigned buffer_size_lcg3 = lbsr.list[1].buffer_size;
  TESTASSERT_EQ(4U, buffer_size_lcg3);
  unsigned buffer_size_lcg4 = lbsr.list[2].buffer_size;
  TESTASSERT_EQ(5U, buffer_size_lcg4);
  unsigned buffer_size_lcg6 = lbsr.list[3].buffer_size;
  TESTASSERT_EQ(7U, buffer_size_lcg6);

  fmt::print("subPDU: {}\n", subpdu);
}

void test_mac_ul_subpdu_long_bsr_unpack_decode_III()
{
  test_delimit_logger delim{"subPDU - Long BSR - 8 LCGs"};
  mac_ul_sch_subpdu   subpdu;

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
  // All LGC_ID set to true (0xff)
  // Buffer size(LGC_ID_0)=1, ..., Buffer size(LGC_ID_n)=n+1, ... , Buffer size(LGC_ID_7)=8
  byte_buffer msg({0x3e, 0x9, 0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08});
  TESTASSERT(subpdu.unpack(msg));

  // Test expected length
  TESTASSERT_EQ(9, subpdu.sdu_length(), "Wrong MAC CE Long BSR (9 bytes)");
  TESTASSERT_EQ(11, subpdu.total_length(), "Wrong subPDU length for MAC CE Long BSR (2B header + 9B MAC-CE)");

  long_bsr_report lbsr = decode_lbsr(bsr_format::LONG_BSR, subpdu.payload());

  bool lcg_7_flag = (bool)(lbsr.bitmap & 0b10000000U);
  TESTASSERT_EQ(true, lcg_7_flag);
  bool lcg_6_flag = (bool)(lbsr.bitmap & 0b01000000U);
  TESTASSERT_EQ(true, lcg_6_flag);
  bool lcg_5_flag = (bool)(lbsr.bitmap & 0b00100000U);
  TESTASSERT_EQ(true, lcg_5_flag);
  bool lcg_4_flag = (bool)(lbsr.bitmap & 0b00010000U);
  TESTASSERT_EQ(true, lcg_4_flag);
  bool lcg_3_flag = (bool)(lbsr.bitmap & 0b00001000U);
  TESTASSERT_EQ(true, lcg_3_flag);
  bool lcg_2_flag = (bool)(lbsr.bitmap & 0b00000100U);
  TESTASSERT_EQ(true, lcg_2_flag);
  bool lcg_1_flag = (bool)(lbsr.bitmap & 0b00000010U);
  TESTASSERT_EQ(true, lcg_1_flag);
  bool lcg_0_flag = (bool)(lbsr.bitmap & 0b00000001U);
  TESTASSERT_EQ(true, lcg_0_flag);

  unsigned buffer_size_lcg0 = lbsr.list[0].buffer_size;
  TESTASSERT_EQ(1U, buffer_size_lcg0);
  unsigned buffer_size_lcg1 = lbsr.list[1].buffer_size;
  TESTASSERT_EQ(2U, buffer_size_lcg1);
  unsigned buffer_size_lcg2 = lbsr.list[2].buffer_size;
  TESTASSERT_EQ(3U, buffer_size_lcg2);
  unsigned buffer_size_lcg3 = lbsr.list[3].buffer_size;
  TESTASSERT_EQ(4U, buffer_size_lcg3);
  unsigned buffer_size_lcg4 = lbsr.list[4].buffer_size;
  TESTASSERT_EQ(5U, buffer_size_lcg4);
  unsigned buffer_size_lcg5 = lbsr.list[5].buffer_size;
  TESTASSERT_EQ(6U, buffer_size_lcg5);
  unsigned buffer_size_lcg6 = lbsr.list[6].buffer_size;
  TESTASSERT_EQ(7U, buffer_size_lcg6);
  unsigned buffer_size_lcg7 = lbsr.list[7].buffer_size;
  TESTASSERT_EQ(8U, buffer_size_lcg7);

  fmt::print("subPDU: {}\n", subpdu);
}

void test_mac_ul_subpdu_long_truncated_bsr_unpack()
{
  test_delimit_logger delim{"subPDU - Long Truncated BSR"};
  mac_ul_sch_subpdu   subpdu;

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
  // MAC CE Long Truncated  BSR = {0x81, 0xd9, 0xab}  LGC_ID_7=true, LGC_ID_0= true
  // Buffer size_1 = 217 (0xd9), Buffer size_2 = 171 (0xab)
  byte_buffer msg({0x3c, 0x3, 0x45, 0xd9, 0xab});
  TESTASSERT(subpdu.unpack(msg));

  // Test expected length
  TESTASSERT_EQ(3, subpdu.sdu_length(), "Wrong MAC CE Long Trunc BSR (3 bytes)");
  TESTASSERT_EQ(5, subpdu.total_length(), "Wrong subPDU length for MAC CE Long Trunc BSR (2B header + 3B MAC-CE)");

  unsigned lcg_6_flag = (bool)(subpdu.payload()[0] & 0b01000000U);
  TESTASSERT_EQ(true, lcg_6_flag);
  unsigned lcg_2_flag = (bool)(subpdu.payload()[0] & 0b00000100U);
  TESTASSERT_EQ(true, lcg_2_flag);
  unsigned lcg_0_flag = (bool)(subpdu.payload()[0] & 0b00000001U);
  TESTASSERT_EQ(true, lcg_0_flag);
  unsigned buffer_size_1 = subpdu.payload()[1];
  TESTASSERT_EQ(217U, buffer_size_1);
  unsigned buffer_size_2 = subpdu.payload()[2];
  TESTASSERT_EQ(171U, buffer_size_2);
  fmt::print("subPDU: {}\n", subpdu);
}

void test_mac_ul_subpdu_crnti_unpack_decode()
{
  test_delimit_logger delim{"subPDU - C-RNTI"};
  mac_ul_sch_subpdu   subpdu;

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
  TESTASSERT(subpdu.unpack(msg));

  // Test expected length
  TESTASSERT_EQ(2, subpdu.sdu_length(), "Wrong MAC CE C-RNTI (2 bytes)");
  TESTASSERT_EQ(3, subpdu.total_length(), "Wrong subPDU length for MAC CE C-RNTI (1B header + 2B MAC-CE)");

  rnti_t rnti = decode_crnti_ce(subpdu.payload());
  TESTASSERT_EQ(std::underlying_type_t<rnti_t>(0x4601), rnti);

  fmt::print("subPDU: {}\n", subpdu);
}

void test_mac_ul_subpdu_se_phr_unpack()
{
  test_delimit_logger delim{"subPDU - SE - PHR"};
  mac_ul_sch_subpdu   subpdu;

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
  TESTASSERT(subpdu.unpack(msg));

  // Test expected length
  TESTASSERT_EQ(2, subpdu.sdu_length(), "Wrong SDU length for MAC CE SE PHR (2 bytes)");
  TESTASSERT_EQ(3, subpdu.total_length(), "Wrong subPDU length for MAC CE SE PHR (1 B header + 2B SDU)");

  // Test Payload
  unsigned ph = subpdu.payload()[0] & 0b00111111U;
  TESTASSERT_EQ(0x27, ph);
  unsigned p_cmax = subpdu.payload()[1] & 0b00111111U;
  TESTASSERT_EQ(0x2f, p_cmax);
  fmt::print("subPDU: {}\n", subpdu);
}

void test_mac_ul_subpdu_padding()
{
  test_delimit_logger delim{"subPDU - padding"};
  mac_ul_sch_subpdu   subpdu;

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
  TESTASSERT(subpdu.unpack(msg));

  // Test expected length
  TESTASSERT_EQ(2, subpdu.sdu_length(), "Wrong SDU length for MAC CE SE PHR (2 bytes)");
  TESTASSERT_EQ(3, subpdu.total_length(), "Wrong subPDU length for MAC CE SE PHR (1 B header + 2B SDU)");

  // Test Payload
  unsigned padding_1 = subpdu.payload()[0];
  TESTASSERT_EQ(0x00, padding_1);
  unsigned padding_2 = subpdu.payload()[1];
  TESTASSERT_EQ(0x00, padding_2);
  fmt::print("subPDU: {}\n", subpdu);
}

void test_mac_ul_pdu()
{
  test_delimit_logger delim{"UL PDU test"};
  mac_ul_sch_pdu      pdu;

  // PDU = [ subPDU_1 | subPDU_2 ]
  // subPDU_1 = [ R/LCID MAC subheader (1 byte) | MAC SDU for UL CCCH (6bytes) ]
  // R/LCID MAC subheader = R|R|LCID = 0x34 or LCID=52
  // MAC SDU (UL CCCH)    = {0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06}  (Random 6B sequence)
  // subPDU_2 = [ R/LCID MAC subheader (1 byte) | padding (3bytes) ]
  // R/LCID MAC subheader = R|R|LCID = 0x3f or LCID=63
  byte_buffer msg({0x34, 0x1e, 0x4f, 0xc0, 0x4f, 0xa6, 0x06, 0x3f, 0x00, 0x00, 0x00});
  TESTASSERT(pdu.unpack(msg));

  // Test expected PDU length
  auto subpdus = pdu.begin();
  TESTASSERT_EQ(6, subpdus->sdu_length(), "Wrong SDU length for UL CCCH (6 bytes)");
  TESTASSERT_EQ(7, subpdus->total_length(), "Wrong subPDU length for UL CCCH (1 B header + 6B SDU)");

  subpdus++;
  TESTASSERT_EQ(3, subpdus->sdu_length(), "Wrong padding length (3 bytes)");
  TESTASSERT_EQ(4, subpdus->total_length(), "Wrong subPDU length padding (1 B header + 3B padding)");
  fmt::print("PDU:\n  Hex={:X}\n  subPDUs: {}\n", fmt::join(msg.begin(), msg.end(), ""), pdu);
}

void test_rx_indication_processing_ccch_48bits()
{
  auto& logger = srslog::fetch_basic_logger("TEST");
  // Use manual task worker for this unittest
  manual_task_worker        task_exec{128};
  dummy_ul_executor_mapper  ul_exec_mapper{task_exec};
  dummy_dl_executor_mapper  dl_exec_mapper{&task_exec};
  dummy_mac_result_notifier phy_notifier;
  dummy_mac_event_indicator du_mng_notifier;
  mac_common_config_t       cfg{du_mng_notifier, ul_exec_mapper, dl_exec_mapper, task_exec, phy_notifier};

  // Add 1 UE to RNTI table
  du_rnti_table rnti_table{};
  rnti_t        ue1_rnti = to_rnti(0x4601);
  du_ue_index_t ue1_idx  = to_du_ue_index(1U);
  rnti_table.add_ue(ue1_rnti, ue1_idx);

  // Create MAC UL processor
  dummy_scheduler_feedback_handler sched_feedback{};
  mac_ul_processor                 mac_ul(cfg, sched_feedback, rnti_table);

  // Create PDU content
  mac_rx_pdu pdu{.rnti = to_rnti(0x4601), .rapid = 1, .harq_id = 0};
  // R/LCID MAC subheader | MAC SDU (UL CCCH 48 bits)
  // { 0x34  | 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06}  (Random 6B sequence)
  pdu.pdu.append({0x34, 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06});

  // Create RX data indication
  mac_rx_data_indication rx_msg;
  rx_msg.cell_index = to_du_cell_index(1U);
  rx_msg.sl_rx      = slot_point{0, 2};
  rx_msg.pdus.push_back(pdu);

  // Send RX data indication to MAC UL
  mac_ul.handle_rx_data_indication(rx_msg);

  // Call task executor manually
  while (task_exec.has_pending_tasks()) {
    logger.info("Running next task in main worker");
    task_exec.try_run_next();
  }

  // Create UL CCCH indication msg to verify MAC processing of PDU
  struct ul_ccch_indication_message ul_ccch_msg {};
  ul_ccch_msg.cell_index = to_du_cell_index(1U);
  ul_ccch_msg.slot_rx    = slot_point{0, 2};
  ul_ccch_msg.crnti      = ue1_rnti;
  // Remove R/R/LCID header (0x34) from PDU
  ul_ccch_msg.subpdu.append({0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06});

  // Test if notification sent to DU manager has been received and it is correct
  TESTASSERT(du_mng_notifier.verify_ul_ccch_msg(ul_ccch_msg));
}

void test_rx_indication_processing_ccch_64bits()
{
  auto& logger = srslog::fetch_basic_logger("TEST");
  // Use manual task worker for this unittest
  manual_task_worker        task_exec{128};
  dummy_ul_executor_mapper  ul_exec_mapper{task_exec};
  dummy_dl_executor_mapper  dl_exec_mapper{&task_exec};
  dummy_mac_result_notifier phy_notifier;
  dummy_mac_event_indicator du_mng_notifier;
  mac_common_config_t       cfg{du_mng_notifier, ul_exec_mapper, dl_exec_mapper, task_exec, phy_notifier};

  // Add 1 UE to RNTI table
  du_rnti_table rnti_table{};
  rnti_t        ue1_rnti = to_rnti(0x4601);
  du_ue_index_t ue1_idx  = to_du_ue_index(1U);
  rnti_table.add_ue(ue1_rnti, ue1_idx);

  // Create MAC UL processor
  dummy_scheduler_feedback_handler sched_feedback{};
  mac_ul_processor                 mac_ul(cfg, sched_feedback, rnti_table);

  // Create PDU content
  mac_rx_pdu pdu{.rnti = to_rnti(0x4601), .rapid = 1, .harq_id = 0};
  // R/LCID MAC subheader | MAC SDU (UL CCCH 64 bits)
  // { 0x00  | 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06, 0x13, 0x54}  (Random 8B sequence)
  pdu.pdu.append({0x00, 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06, 0x13, 0x54});

  // Create RX data indication
  mac_rx_data_indication rx_msg;
  rx_msg.cell_index = to_du_cell_index(1);
  rx_msg.sl_rx      = slot_point{0, 2};
  rx_msg.pdus.push_back(pdu);

  // Send RX data indication to MAC UL
  mac_ul.handle_rx_data_indication(rx_msg);

  // Call task executor manually
  while (task_exec.has_pending_tasks()) {
    logger.info("Running next task in main worker");
    task_exec.try_run_next();
  }

  // Create UL CCCH indication msg to verify MAC processing of PDU
  struct ul_ccch_indication_message ul_ccch_msg {};
  ul_ccch_msg.cell_index = to_du_cell_index(1U);
  ul_ccch_msg.slot_rx    = slot_point{0, 2};
  ul_ccch_msg.crnti      = ue1_rnti;
  // Remove R/R/LCID header (0x00) from PDU
  ul_ccch_msg.subpdu.append({0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06, 0x13, 0x54});

  // Test if notification sent to DU manager has been received and it is correct
  TESTASSERT(du_mng_notifier.verify_ul_ccch_msg(ul_ccch_msg));
}

void test_rx_indication_processing_sbsr()
{
  auto& logger = srslog::fetch_basic_logger("TEST");
  // Use manual task worker for this unittest
  manual_task_worker        task_exec{128};
  dummy_ul_executor_mapper  ul_exec_mapper{task_exec};
  dummy_dl_executor_mapper  dl_exec_mapper{&task_exec};
  dummy_mac_result_notifier phy_notifier;
  dummy_mac_event_indicator du_mng_notifier;
  mac_common_config_t       cfg{du_mng_notifier, ul_exec_mapper, dl_exec_mapper, task_exec, phy_notifier};

  // Add 1 UE to RNTI table
  du_rnti_table rnti_table{};
  rnti_t        ue1_rnti = to_rnti(0x4601);
  du_ue_index_t ue1_idx  = to_du_ue_index(1U);
  rnti_table.add_ue(ue1_rnti, ue1_idx);

  // Create MAC UL processor
  dummy_scheduler_feedback_handler sched_feedback{};
  mac_ul_processor                 mac_ul(cfg, sched_feedback, rnti_table);

  // Create PDU content
  mac_rx_pdu pdu{.rnti = to_rnti(0x4601), .rapid = 1, .harq_id = 0};
  // R/LCID MAC subheader | MAC CE Short BSR
  // { 0x3d | 0x59}
  pdu.pdu.append({0x3d, 0x59});

  // Create RX data indication
  mac_rx_data_indication rx_msg_sbsr;
  rx_msg_sbsr.cell_index = to_du_cell_index(1);
  rx_msg_sbsr.sl_rx      = slot_point(0, 1);
  rx_msg_sbsr.pdus.push_back(pdu);

  // Send RX data indication to MAC UL
  mac_ul.handle_rx_data_indication(rx_msg_sbsr);

  // Call task executor manually
  while (task_exec.has_pending_tasks()) {
    logger.info("Running next task in main worker");
    task_exec.try_run_next();
  }

  // Create UL BSR indication  message to compare with one passed to the scheduler
  ul_bsr_indication_message ul_bsr_ind{};
  ul_bsr_ind.cell_index = to_du_cell_index(1U);
  ul_bsr_ind.ue_index   = ue1_idx;
  ul_bsr_ind.crnti      = ue1_rnti;
  ul_bsr_ind.type       = bsr_format::SHORT_BSR;
  ul_bsr_lcg_report sbsr_report{.lcg_id = 2U, .nof_bytes = 28581};
  ul_bsr_ind.reported_lcgs.push_back(sbsr_report);

  // Test if notification sent to Scheduler has been received and it is correct
  TESTASSERT(sched_feedback.verify_bsr_msg(ul_bsr_ind));
}

void test_rx_indication_processing_s_trunc_bsr()
{
  auto& logger = srslog::fetch_basic_logger("TEST");
  // Use manual task worker for this unittest
  manual_task_worker        task_exec{128};
  dummy_ul_executor_mapper  ul_exec_mapper{task_exec};
  dummy_dl_executor_mapper  dl_exec_mapper{&task_exec};
  dummy_mac_result_notifier phy_notifier;
  dummy_mac_event_indicator du_mng_notifier;
  mac_common_config_t       cfg{du_mng_notifier, ul_exec_mapper, dl_exec_mapper, task_exec, phy_notifier};

  // Add 1 UE to RNTI table
  du_rnti_table rnti_table{};
  rnti_t        ue1_rnti = to_rnti(0x4601);
  du_ue_index_t ue1_idx  = to_du_ue_index(1U);
  rnti_table.add_ue(ue1_rnti, ue1_idx);

  // Create MAC UL processor
  dummy_scheduler_feedback_handler sched_feedback{};
  mac_ul_processor                 mac_ul(cfg, sched_feedback, rnti_table);

  // Create PDU content
  mac_rx_pdu pdu{.rnti = to_rnti(0x4601), .rapid = 1, .harq_id = 0};
  // R/LCID MAC subheader | MAC CE Short Truncated BSR
  // { 0x3b | 0xae}
  pdu.pdu.append({0x3b, 0xae});

  // Create RX data indication
  mac_rx_data_indication rx_msg_sbsr;
  rx_msg_sbsr.cell_index = to_du_cell_index(1);
  rx_msg_sbsr.sl_rx      = slot_point(0, 1);
  rx_msg_sbsr.pdus.push_back(pdu);

  // Send RX data indication to MAC UL
  mac_ul.handle_rx_data_indication(rx_msg_sbsr);

  // Call task executor manually
  while (task_exec.has_pending_tasks()) {
    logger.info("Running next task in main worker");
    task_exec.try_run_next();
  }

  // Create UL BSR indication  message to compare with one passed to the scheduler
  ul_bsr_indication_message ul_bsr_ind{};
  ul_bsr_ind.cell_index = to_du_cell_index(1U);
  ul_bsr_ind.ue_index   = ue1_idx;
  ul_bsr_ind.crnti      = ue1_rnti;
  ul_bsr_ind.type       = bsr_format::SHORT_TRUNC_BSR;
  ul_bsr_lcg_report sbsr_report{.lcg_id = 5U, .nof_bytes = 745};
  ul_bsr_ind.reported_lcgs.push_back(sbsr_report);

  // Test if notification sent to Scheduler has been received and it is correct
  TESTASSERT(sched_feedback.verify_bsr_msg(ul_bsr_ind));
}

void test_rx_indication_processing_long_bsr()
{
  auto& logger = srslog::fetch_basic_logger("TEST");
  // Use manual task worker for this unittest
  manual_task_worker        task_exec{128};
  dummy_ul_executor_mapper  ul_exec_mapper{task_exec};
  dummy_dl_executor_mapper  dl_exec_mapper{&task_exec};
  dummy_mac_result_notifier phy_notifier;
  dummy_mac_event_indicator du_mng_notifier;
  mac_common_config_t       cfg{du_mng_notifier, ul_exec_mapper, dl_exec_mapper, task_exec, phy_notifier};

  // Add 1 UE to RNTI table
  du_rnti_table rnti_table{};
  rnti_t        ue1_rnti = to_rnti(0x4601);
  du_ue_index_t ue1_idx  = to_du_ue_index(1U);
  rnti_table.add_ue(ue1_rnti, ue1_idx);

  // Create MAC UL processor
  dummy_scheduler_feedback_handler sched_feedback{};
  mac_ul_processor                 mac_ul(cfg, sched_feedback, rnti_table);

  // Create PDU content
  mac_rx_pdu pdu{.rnti = to_rnti(0x4601), .rapid = 1, .harq_id = 0};
  // R/F/LCID/L MAC subheader | MAC CE Short BSR
  // { 0x3e, 0x03 | 0x81, 0xd9, 0xab }
  pdu.pdu.append({0x3e, 0x03, 0x81, 0xd9, 0xab});

  // Create RX data indication
  mac_rx_data_indication rx_msg_sbsr;
  rx_msg_sbsr.cell_index = to_du_cell_index(1);
  rx_msg_sbsr.sl_rx      = slot_point(0, 1);
  rx_msg_sbsr.pdus.push_back(pdu);

  // Send RX data indication to MAC UL
  mac_ul.handle_rx_data_indication(rx_msg_sbsr);

  // Call task executor manually
  while (task_exec.has_pending_tasks()) {
    logger.info("Running next task in main worker");
    task_exec.try_run_next();
  }

  // Create UL BSR indication  message to compare with one passed to the scheduler
  ul_bsr_indication_message ul_bsr_ind{};
  ul_bsr_ind.cell_index = to_du_cell_index(1);
  ul_bsr_ind.ue_index   = ue1_idx;
  ul_bsr_ind.crnti      = ue1_rnti;
  ul_bsr_ind.type       = bsr_format::LONG_BSR;
  ul_bsr_lcg_report bsr_report_lcg0{.lcg_id = 0U, .nof_bytes = 8453028U};
  ul_bsr_ind.reported_lcgs.push_back(bsr_report_lcg0);
  ul_bsr_lcg_report bsr_report_lcg7{.lcg_id = 7U, .nof_bytes = 468377U};
  ul_bsr_ind.reported_lcgs.push_back(bsr_report_lcg7);

  // Test if notification sent to Scheduler has been received and it is correct
  TESTASSERT(sched_feedback.verify_bsr_msg(ul_bsr_ind));
}

int main()
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::info);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
  srslog::init();

  // Test Unpacking/decoding function for MAC CEs and SDUs
  test_mac_ul_subpdu_ccch_48_unpack();
  test_mac_ul_subpdu_ccch_64_unpack();
  test_mac_ul_subpdu_short_bsr_unpack_decode();
  test_mac_ul_subpdu_short_trunc_bsr_unpack_decode();
  test_mac_ul_subpdu_long_bsr_unpack_decode();
  test_mac_ul_subpdu_long_bsr_unpack_decode_II();
  test_mac_ul_subpdu_long_bsr_unpack_decode_III();
  test_mac_ul_subpdu_long_truncated_bsr_unpack();
  test_mac_ul_subpdu_crnti_unpack_decode();
  test_mac_ul_subpdu_se_phr_unpack();

  test_mac_ul_pdu();

  // Test generation of RX indication message and dispatching to correct entity.
  test_rx_indication_processing_ccch_48bits();
  test_rx_indication_processing_ccch_64bits();
  test_rx_indication_processing_sbsr();
  test_rx_indication_processing_s_trunc_bsr();
  test_rx_indication_processing_long_bsr();
}