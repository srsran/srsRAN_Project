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

// Implement dummy scheduler feedback handler, only used for this test.
class dummy_scheduler_feedback_handler : public scheduler_feedback_handler
{
public:
  dummy_scheduler_feedback_handler() = default;
  void ul_sr_info(const sr_indication_message& sr) override { last_shed_req_msg = sr; };
  void ul_bsr(const ul_bsr_indication_message& bsr) override { last_bsr_msg = bsr; };

  // Compare last_shed_req_msg with a test message passed to the function.
  bool verify_sched_req_msg(const sr_indication_message& sr)
  {
    bool test_msg = last_shed_req_msg.cell_index == sr.cell_index && last_shed_req_msg.ue_index == sr.ue_index &&
                    last_shed_req_msg.crnti == sr.crnti && last_shed_req_msg.sr_payload == sr.sr_payload;

    return test_msg;
  }

  // Compare last_bsr_msg with a test message passed to the function.
  bool verify_bsr_msg(const ul_bsr_indication_message& test_usr_msg)
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
  sr_indication_message     last_shed_req_msg;
};

// Helper struct that creates a MAC UL to test the correct processing of RX indication messages.
struct test_bench {
  test_bench(rnti_t rnti, du_ue_index_t du_ue_idx, du_cell_index_t cell_idx_) :
    task_exec(128),
    ul_exec_mapper(task_exec),
    dl_exec_mapper({&task_exec}),
    cfg(du_mng_notifier, ul_exec_mapper, dl_exec_mapper, task_exec, phy_notifier),
    ue_rnti(rnti),
    cell_idx(cell_idx_),
    mac_ul(cfg, sched_feedback, rnti_table)
  {
    rnti_table.add_ue(rnti, du_ue_idx);
    rx_msg_sbsr.cell_index = cell_idx;
    rx_msg_sbsr.sl_rx      = slot_point(0, 1);
  }

  // Add a UE to the RNTI table.
  void add_ue(rnti_t rnti, du_ue_index_t du_ue_idx)
  {
    if (rnti_table.has_rnti(rnti)) {
      return;
    }
    rnti_table.add_ue(rnti, du_ue_idx);
  }

  // Add a PDU to the list of PDUs that will be included in the RX indication message.
  void enqueue_pdu(byte_buffer& pdu_payload)
  {
    // Create PDU content.
    mac_rx_pdu pdu{.rnti = ue_rnti, .rapid = 1, .harq_id = 0};
    pdu.pdu.append(pdu_payload);

    // Add PDU to the list in the RX indication message.
    rx_msg_sbsr.pdus.push_back(pdu);
  }

  // Send a RX indication message to the MAC UL and call the manual task executor.
  void send_rx_indication_msg(byte_buffer& pdu_payload)
  {
    // Add PDU to the list of PDUs to be included in the RX indication message.
    enqueue_pdu(pdu_payload);

    // Send RX data indication to MAC UL.
    mac_ul.handle_rx_data_indication(rx_msg_sbsr);

    // Call task executor manually.
    while (task_exec.has_pending_tasks()) {
      task_exec.try_run_next();
    }
  }

  // Call the dummy scheduler to compare the SR indication with a benchmark message.
  bool verify_sched_req_notification(const sr_indication_message& test_msg)
  {
    return sched_feedback.verify_sched_req_msg(test_msg);
  }

  // Call the dummy scheduler to compare the BSR indication with a benchmark message.
  bool verify_sched_bsr_notification(const ul_bsr_indication_message& test_msg)
  {
    return sched_feedback.verify_bsr_msg(test_msg);
  }

  // Call the dummy DU notifier to compare the UL CCCH indication with a benchmark message.
  bool verify_du_ul_ccch_msg(const ul_ccch_indication_message& test_msg)
  {
    return du_mng_notifier.verify_ul_ccch_msg(test_msg);
  }

private:
  manual_task_worker               task_exec;
  dummy_ul_executor_mapper         ul_exec_mapper;
  dummy_dl_executor_mapper         dl_exec_mapper;
  dummy_mac_result_notifier        phy_notifier;
  dummy_mac_event_indicator        du_mng_notifier;
  mac_common_config_t              cfg;
  du_rnti_table                    rnti_table;
  dummy_scheduler_feedback_handler sched_feedback;
  // This is the RNTI of the UE that appears in the mac_rx_pdu created by send_rx_indication_msg()
  rnti_t                 ue_rnti;
  du_cell_index_t        cell_idx;
  mac_ul_processor       mac_ul;
  mac_rx_data_indication rx_msg_sbsr;
};

// Test the unpacking function for MAC subPDU with MAC SDU (UL CCCH 48 bits).
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

// Test the unpacking function for MAC subPDU with MAC SDU (UL CCCH 64 bits).
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

  // Test expected length.
  TESTASSERT_EQ(8, subpdu.sdu_length(), "Wrong SDU length for UL CCCH (8 bytes)");
  TESTASSERT_EQ(9, subpdu.total_length(), "Wrong subPDU length for UL CCCH (1 B header + 8B SDU)");
  fmt::print("subPDU: {}\n", subpdu);
}

// Test the unpacking function for MAC subPDU with MAC CE (Short BSR).
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
  // MAC CE Short BSR = {0x59}  LCG ID = 2, Buffer size = 25
  byte_buffer msg({0x3d, 0x59});
  TESTASSERT(subpdu.unpack(msg));

  // Test expected length.
  TESTASSERT_EQ(1, subpdu.sdu_length(), "Wrong MAC CE Long BSR (1 byte)");
  TESTASSERT_EQ(2, subpdu.total_length(), "Wrong subPDU length for MAC CE Short BSR (1B header + 1B MAC-CE)");

  lcg_bsr_report bsr_report = decode_sbsr(subpdu.payload());
  TESTASSERT_EQ(2U, bsr_report.lcg_id);
  TESTASSERT_EQ(25U, bsr_report.buffer_size);

  fmt::print("subPDU: {}\n", subpdu);
}

// Test the unpacking function for MAC subPDU with MAC CE (Short Truncated BSR).
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
  // MAC CE Short BSR = {0xae}  LCG ID = 2, Buffer size = 14
  byte_buffer msg({0x3b, 0xae});
  TESTASSERT(subpdu.unpack(msg));

  // Test expected length.
  TESTASSERT_EQ(1, subpdu.sdu_length(), "Wrong MAC CE Long BSR (1 byte)");
  TESTASSERT_EQ(2, subpdu.total_length(), "Wrong subPDU length for MAC CE Short Trunc. BSR (1B header + 1B MAC-CE)");

  lcg_bsr_report bsr_report = decode_sbsr(subpdu.payload());
  TESTASSERT_EQ(5U, bsr_report.lcg_id);
  TESTASSERT_EQ(14U, bsr_report.buffer_size);

  fmt::print("subPDU: {}\n", subpdu);
}

// Test the unpacking function for MAC subPDU with MAC CE (Long BSR for 2 LCGs).
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
  // MAC CE Long BSR = {0x81, 0xd9, 0xab}  LCG_ID_7=true, LCG_ID_0= true
  // Buffer size(LCG_ID_0) = 217 (0xd9), Buffer size(LCG_ID_7) = 171 (0xab)
  byte_buffer msg({0x3e, 0x03, 0x81, 0xd9, 0xab});
  TESTASSERT(subpdu.unpack(msg));

  // Test expected length.
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

// Test the unpacking function for MAC subPDU with MAC CE (Long BSR for 4 LCGs).
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
  // LCG_ID_6=true, LCG_ID_4=true, LCG_ID_3=true, LCG_ID_1=true
  // Buffer size(LCG_ID_1)=2, Buffer size(LCG_ID_3)=4, Buffer size(LCG_ID_4)=5, Buffer size(LCG_ID_6)=7
  byte_buffer msg({0x3e, 0x05, 0x5a, 0x02, 0x04, 0x05, 0x07});
  TESTASSERT(subpdu.unpack(msg));

  // Test expected length.
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

// Test the unpacking function for MAC subPDU with MAC CE (Long BSR for all LCGs).
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
  // All LCG_ID set to true (0xff)
  // Buffer size(LCG_ID_0)=1, ..., Buffer size(LCG_ID_n)=n+1, ... , Buffer size(LCG_ID_7)=8
  byte_buffer msg({0x3e, 0x9, 0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08});
  TESTASSERT(subpdu.unpack(msg));

  // Test expected length.
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

// Test the unpacking function for MAC subPDU with MAC CE (Long Truncated BSR).
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
  // MAC CE Long Truncated  BSR = {0x81, 0xd9, 0xab}  LCG_ID_7=true, LCG_ID_0= true
  // Buffer size_1 = 217 (0xd9), Buffer size_2 = 171 (0xab)
  byte_buffer msg({0x3c, 0x3, 0x45, 0xd9, 0xab});
  TESTASSERT(subpdu.unpack(msg));

  // Test expected length.
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

// Test the unpacking function for MAC subPDU with MAC CE (C-RNTI).
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

  // Test expected length.
  TESTASSERT_EQ(2, subpdu.sdu_length(), "Wrong MAC CE C-RNTI (2 bytes)");
  TESTASSERT_EQ(3, subpdu.total_length(), "Wrong subPDU length for MAC CE C-RNTI (1B header + 2B MAC-CE)");

  rnti_t rnti = decode_crnti_ce(subpdu.payload());
  TESTASSERT_EQ(std::underlying_type_t<rnti_t>(0x4601), rnti);

  fmt::print("subPDU: {}\n", subpdu);
}

// Test the unpacking function for MAC subPDU with MAC CE (Single Entry PHR).
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

  // Test expected length.
  TESTASSERT_EQ(2, subpdu.sdu_length(), "Wrong SDU length for MAC CE SE PHR (2 bytes)");
  TESTASSERT_EQ(3, subpdu.total_length(), "Wrong subPDU length for MAC CE SE PHR (1 B header + 2B SDU)");

  // Test Payload.
  unsigned ph = subpdu.payload()[0] & 0b00111111U;
  TESTASSERT_EQ(0x27, ph);
  unsigned p_cmax = subpdu.payload()[1] & 0b00111111U;
  TESTASSERT_EQ(0x2f, p_cmax);
  fmt::print("subPDU: {}\n", subpdu);
}

// Test the unpacking function for MAC subPDU with padding.
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

  // Test expected length.
  TESTASSERT_EQ(2, subpdu.sdu_length(), "Wrong SDU length for MAC CE SE PHR (2 bytes)");
  TESTASSERT_EQ(3, subpdu.total_length(), "Wrong subPDU length for MAC CE SE PHR (1 B header + 2B SDU)");

  // Test Payload.
  unsigned padding_1 = subpdu.payload()[0];
  TESTASSERT_EQ(0x00, padding_1);
  unsigned padding_2 = subpdu.payload()[1];
  TESTASSERT_EQ(0x00, padding_2);
  fmt::print("subPDU: {}\n", subpdu);
}

// Test the unpacking function for MAC PDU with 2 MAC SDUs: UL CCCH 48 bits and padding.
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

  // Test expected PDU length.
  auto subpdus = pdu.begin();
  TESTASSERT_EQ(6, subpdus->sdu_length(), "Wrong SDU length for UL CCCH (6 bytes)");
  TESTASSERT_EQ(7, subpdus->total_length(), "Wrong subPDU length for UL CCCH (1 B header + 6B SDU)");

  subpdus++;
  TESTASSERT_EQ(3, subpdus->sdu_length(), "Wrong padding length (3 bytes)");
  TESTASSERT_EQ(4, subpdus->total_length(), "Wrong subPDU length padding (1 B header + 3B padding)");
  fmt::print("PDU:\n  Hex={:X}\n  subPDUs: {}\n", fmt::join(msg.begin(), msg.end(), ""), pdu);
}

// Test UL MAC processing of RX indication message with MAC PDU for UL CCCH 48 bits.
void test_rx_indication_processing_ccch_48bits()
{
  // Define UE and create test_bench.
  rnti_t          ue1_rnti = to_rnti(0x4601);
  du_ue_index_t   ue1_idx  = to_du_ue_index(1U);
  du_cell_index_t cell_idx = to_du_cell_index(1U);
  test_bench      t_bench(ue1_rnti, ue1_idx, cell_idx);

  // Create PDU content.
  // R/LCID MAC subheader | MAC SDU (UL CCCH 48 bits)
  // { 0x34  | 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06}  (Random 6B sequence)
  byte_buffer pdu({0x34, 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06});

  // Send RX data indication to MAC UL.
  t_bench.send_rx_indication_msg(pdu);

  // Create UL CCCH indication msg to verify MAC processing of PDU.
  struct ul_ccch_indication_message ul_ccch_msg {};
  ul_ccch_msg.cell_index = cell_idx;
  ul_ccch_msg.slot_rx    = slot_point{0, 1};
  ul_ccch_msg.crnti      = ue1_rnti;
  // Remove R/R/LCID header (0x34) from PDU
  ul_ccch_msg.subpdu.append({0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06});

  // Test if notification sent to DU manager has been received and it is correct.
  TESTASSERT(t_bench.verify_du_ul_ccch_msg(ul_ccch_msg));
}

// Test UL MAC processing of RX indication message with MAC PDU for UL CCCH 64 bits.
void test_rx_indication_processing_ccch_64bits()
{
  // Define UE and create test_bench.
  rnti_t          ue1_rnti = to_rnti(0x4601);
  du_ue_index_t   ue1_idx  = to_du_ue_index(1U);
  du_cell_index_t cell_idx = to_du_cell_index(1U);
  test_bench      t_bench(ue1_rnti, ue1_idx, cell_idx);

  // Create PDU content.
  // R/LCID MAC subheader | MAC SDU (UL CCCH 64 bits)
  // { 0x00  | 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06, 0x13, 0x54}  (Random 8B sequence)
  byte_buffer pdu({0x00, 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06, 0x13, 0x54});

  // Send RX data indication to MAC UL.
  t_bench.send_rx_indication_msg(pdu);

  // Create UL CCCH indication msg to verify MAC processing of PDU.
  struct ul_ccch_indication_message ul_ccch_msg {};
  ul_ccch_msg.cell_index = cell_idx;
  ul_ccch_msg.slot_rx    = slot_point{0, 1};
  ul_ccch_msg.crnti      = ue1_rnti;
  // Remove R/R/LCID header (0x00) from PDU
  ul_ccch_msg.subpdu.append({0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06, 0x13, 0x54});

  // Test if notification sent to DU manager has been received and it is correct.
  TESTASSERT(t_bench.verify_du_ul_ccch_msg(ul_ccch_msg));
}

// Test UL MAC processing of RX indication message with MAC PDU for MAC CE Short BSR.
void test_rx_indication_processing_sbsr()
{
  // Define UE and create test_bench.
  rnti_t          ue1_rnti = to_rnti(0x4601);
  du_ue_index_t   ue1_idx  = to_du_ue_index(1U);
  du_cell_index_t cell_idx = to_du_cell_index(1U);
  test_bench      t_bench(ue1_rnti, ue1_idx, cell_idx);

  // Create PDU content.
  // R/LCID MAC subheader | MAC CE Short BSR
  // { 0x3d | 0x59}
  byte_buffer pdu({0x3d, 0x59});

  // Send RX data indication to MAC UL
  t_bench.send_rx_indication_msg(pdu);

  // Create UL BSR indication  message to compare with one passed to the scheduler.
  ul_bsr_indication_message ul_bsr_ind{};
  ul_bsr_ind.cell_index = cell_idx;
  ul_bsr_ind.ue_index   = ue1_idx;
  ul_bsr_ind.crnti      = ue1_rnti;
  ul_bsr_ind.type       = bsr_format::SHORT_BSR;
  ul_bsr_lcg_report sbsr_report{.lcg_id = 2U, .nof_bytes = 28581};
  ul_bsr_ind.reported_lcgs.push_back(sbsr_report);
  // Test if notification sent to Scheduler has been received and it is correct.
  TESTASSERT(t_bench.verify_sched_bsr_notification(ul_bsr_ind));
}

// Test UL MAC processing of RX indication message with MAC PDU for MAC CE Short Truncated BSR.
void test_rx_indication_processing_s_trunc_bsr()
{
  // Define UE and create test_bench.
  rnti_t          ue1_rnti = to_rnti(0x4601);
  du_ue_index_t   ue1_idx  = to_du_ue_index(1U);
  du_cell_index_t cell_idx = to_du_cell_index(1U);
  test_bench      t_bench(ue1_rnti, ue1_idx, cell_idx);

  // Create PDU content.
  // R/LCID MAC subheader | MAC CE Short Truncated BSR
  // { 0x3b | 0xae}
  byte_buffer pdu({0x3b, 0xae});

  // Send RX data indication to MAC UL
  t_bench.send_rx_indication_msg(pdu);

  // Create UL BSR indication  message to compare with one passed to the scheduler.
  ul_bsr_indication_message ul_bsr_ind{};
  ul_bsr_ind.cell_index = cell_idx;
  ul_bsr_ind.ue_index   = ue1_idx;
  ul_bsr_ind.crnti      = ue1_rnti;
  ul_bsr_ind.type       = bsr_format::SHORT_TRUNC_BSR;
  ul_bsr_lcg_report sbsr_report{.lcg_id = 5U, .nof_bytes = 745};
  ul_bsr_ind.reported_lcgs.push_back(sbsr_report);

  // Test if notification sent to Scheduler has been received and it is correct.
  TESTASSERT(t_bench.verify_sched_bsr_notification(ul_bsr_ind));
}

// Test UL MAC processing of RX indication message with MAC PDU for MAC CE Long BSR.
void test_rx_indication_processing_long_bsr()
{
  // Define UE and create test_bench.
  rnti_t          ue1_rnti = to_rnti(0x4601);
  du_ue_index_t   ue1_idx  = to_du_ue_index(1U);
  du_cell_index_t cell_idx = to_du_cell_index(1U);
  test_bench      t_bench(ue1_rnti, ue1_idx, cell_idx);

  // Create PDU content.
  // R/F/LCID/L MAC subheader | MAC CE Short BSR
  // { 0x3e, 0x03 | 0x81, 0xd9, 0xab }
  byte_buffer pdu({0x3e, 0x03, 0x81, 0xd9, 0xab});

  // Send RX data indication to MAC UL.
  t_bench.send_rx_indication_msg(pdu);

  // Create UL BSR indication  message to compare with one passed to the scheduler.
  ul_bsr_indication_message ul_bsr_ind{};
  ul_bsr_ind.cell_index = to_du_cell_index(1);
  ul_bsr_ind.ue_index   = ue1_idx;
  ul_bsr_ind.crnti      = ue1_rnti;
  ul_bsr_ind.type       = bsr_format::LONG_BSR;
  ul_bsr_lcg_report bsr_report_lcg0{.lcg_id = 0U, .nof_bytes = 8453028U};
  ul_bsr_ind.reported_lcgs.push_back(bsr_report_lcg0);
  ul_bsr_lcg_report bsr_report_lcg7{.lcg_id = 7U, .nof_bytes = 468377U};
  ul_bsr_ind.reported_lcgs.push_back(bsr_report_lcg7);

  // Test if notification sent to Scheduler has been received and it is correct.
  TESTASSERT(t_bench.verify_sched_bsr_notification(ul_bsr_ind));
}

// Test UL MAC processing of RX indication message with MAC PDU for MAC CE C-RNTI.
void test_rx_indication_processing_ce_rnti()
{
  // Define UE and create test_bench.
  rnti_t          ue1_rnti = to_rnti(0x4602);
  du_ue_index_t   ue1_idx  = to_du_ue_index(2U);
  du_cell_index_t cell_idx = to_du_cell_index(1U);
  test_bench      t_bench(ue1_rnti, ue1_idx, cell_idx);
  // Add a UE. This RNTI (0x4601) is the one carried by the MAC CE C-RNTI and should be used in the sr_ind{} below.
  t_bench.add_ue(to_rnti(0x4601), to_du_ue_index(1U));

  // Create PDU content.
  // R/LCID MAC subheader | MAC CE C-RNTI
  // { 0x3A | 0x46, 0x01 }
  byte_buffer pdu({0x3a, 0x46, 0x01});

  // Send RX data indication to MAC UL.
  t_bench.send_rx_indication_msg(pdu);

  // Create UL BSR indication  message to compare with one passed to the scheduler.
  sr_indication_message sr_ind{};
  sr_ind.cell_index = cell_idx;
  sr_ind.ue_index   = to_du_ue_index(1U);
  sr_ind.crnti      = to_rnti(0x4601);
  sr_ind.sr_payload.resize(1);
  sr_ind.sr_payload.set(0);
  // Test if notification sent to Scheduler has been received and it is correct.
  TESTASSERT(t_bench.verify_sched_req_notification(sr_ind));
}

// Test UL MAC processing of RX indication message with MAC PDU for multiple subPDUs (MAC CE C-RNTI, MAC CE Short BSR).
void test_rx_indication_processing_ce_rnti_sbsr()
{
  // Define UE and create test_bench.
  rnti_t          ue1_rnti = to_rnti(0x4602);
  du_ue_index_t   ue1_idx  = to_du_ue_index(2U);
  du_cell_index_t cell_idx = to_du_cell_index(1U);
  test_bench      t_bench(ue1_rnti, ue1_idx, cell_idx);
  // Add a UE. This RNTI (0x4601) is the one carried by the MAC CE C-RNTI and should be used in the sr_ind{} below
  t_bench.add_ue(to_rnti(0x4601), to_du_ue_index(1U));

  // Create subPDU content.
  // R/LCID MAC subheader | MAC CE C-RNTI
  // { 0x3A | 0x46, 0x01 }
  byte_buffer pdu_ce_crnti({0x3a, 0x46, 0x01});
  t_bench.enqueue_pdu(pdu_ce_crnti);

  // Create subPDU content.
  // R/LCID MAC subheader | MAC CE Short BSR
  // { 0x3d | 0x59}
  byte_buffer pdu_sbsr({0x3d, 0x59});
  // Send RX data indication to MAC UL
  t_bench.send_rx_indication_msg(pdu_sbsr);

  // Create UL Sched Req indication message (generated by MAC CE C-RNTI) to compare with one passed to the scheduler.
  sr_indication_message sr_ind{};
  sr_ind.cell_index = cell_idx;
  sr_ind.ue_index   = to_du_ue_index(1U);
  sr_ind.crnti      = to_rnti(0x4601);
  sr_ind.sr_payload.resize(1);
  sr_ind.sr_payload.set(0);
  // Test if notification sent to Scheduler has been received and it is correct.
  TESTASSERT(t_bench.verify_sched_req_notification(sr_ind));

  // Create UL BSR indication message to compare with one passed to the scheduler.
  ul_bsr_indication_message ul_bsr_ind{};
  ul_bsr_ind.cell_index = cell_idx;
  ul_bsr_ind.ue_index   = ue1_idx;
  ul_bsr_ind.crnti      = ue1_rnti;
  ul_bsr_ind.type       = bsr_format::SHORT_BSR;
  ul_bsr_lcg_report sbsr_report{.lcg_id = 2U, .nof_bytes = 28581};
  ul_bsr_ind.reported_lcgs.push_back(sbsr_report);
  TESTASSERT(t_bench.verify_sched_bsr_notification(ul_bsr_ind));
}

int main()
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::info);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
  srslog::init();

  // Test Unpacking/decoding function for MAC CEs and SDUs.
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
  test_mac_ul_subpdu_padding();

  // Test Unpacking/decoding function for MAC PDU with multiple MAC SDUs.
  test_mac_ul_pdu();

  // Test generation of RX indication message and dispatching to correct entity.
  test_rx_indication_processing_ccch_48bits();
  test_rx_indication_processing_ccch_64bits();
  test_rx_indication_processing_sbsr();
  test_rx_indication_processing_s_trunc_bsr();
  test_rx_indication_processing_long_bsr();
  test_rx_indication_processing_ce_rnti();
  test_rx_indication_processing_ce_rnti_sbsr();
}