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

#include "lib/mac/mac_ul/mac_scheduler_ce_info_handler.h"
#include "lib/mac/mac_ul/mac_ul_processor.h"
#include "mac_ctrl_test_dummies.h"
#include "mac_test_helpers.h"
#include "srsran/scheduler/scheduler_feedback_handler.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace test_helpers;

class dummy_sched_ce_info_handler : public mac_scheduler_ce_info_handler
{
public:
  optional<mac_phr_ce_info>           last_phr_msg;
  optional<mac_bsr_ce_info>           last_bsr_msg;
  optional<mac_ul_scheduling_command> last_sched_cmd;
  optional<mac_ce_scheduling_command> last_ce_cmd;

  /// \brief Forward to scheduler any decoded UL BSRs for a given UE.
  void handle_ul_bsr_indication(const mac_bsr_ce_info& bsr) override { last_bsr_msg = bsr; }

  /// \brief Force the UL grant scheduling for a given UE.
  void handle_ul_sched_command(const mac_ul_scheduling_command& sched_cmd) override { last_sched_cmd = sched_cmd; }

  void handle_dl_mac_ce_indication(const mac_ce_scheduling_command& mac_ce) override { last_ce_cmd = mac_ce; }

  /// Verify a SR indication was added for a given UE.
  bool verify_sched_req_msg(du_ue_index_t ue_index)
  {
    if (last_sched_cmd.has_value()) {
      return last_sched_cmd->ue_index == ue_index;
    }
    return false;
  }

  /// Compare last_bsr_msg with a test message passed to the function.
  void verify_bsr_msg(const mac_bsr_ce_info& bsr)
  {
    EXPECT_EQ(last_bsr_msg->cell_index, bsr.cell_index);
    EXPECT_EQ(last_bsr_msg->ue_index, bsr.ue_index);
    EXPECT_EQ(last_bsr_msg->rnti, bsr.rnti);
    EXPECT_EQ(last_bsr_msg->bsr_fmt, bsr.bsr_fmt);
    EXPECT_EQ(last_bsr_msg->lcg_reports.size(), bsr.lcg_reports.size());
    for (size_t n = 0; n < bsr.lcg_reports.size(); ++n) {
      EXPECT_EQ(last_bsr_msg->lcg_reports[n].lcg_id, bsr.lcg_reports[n].lcg_id);
      EXPECT_EQ(last_bsr_msg->lcg_reports[n].buffer_size, bsr.lcg_reports[n].buffer_size);
    }
    if (bsr.bsr_fmt == bsr_format::SHORT_BSR or bsr.bsr_fmt == bsr_format::SHORT_TRUNC_BSR) {
      EXPECT_EQ(last_bsr_msg->lcg_reports.size(), 1);
    }
  }

  /// \brief Forward to scheduler any decoded UL PHRs for a given UE.
  virtual void handle_ul_phr_indication(const mac_phr_ce_info& phr) override { last_phr_msg = phr; }

  /// Compare verify_phr_msg with a test message passed to the function.
  // TODO: Handle verification of Multiple Entry PHR.
  void verify_phr_msg(const mac_phr_ce_info& phr_info)
  {
    EXPECT_EQ(last_phr_msg->cell_index, phr_info.cell_index);
    EXPECT_EQ(last_phr_msg->ue_index, phr_info.ue_index);
    EXPECT_EQ(last_phr_msg->rnti, phr_info.rnti);
    EXPECT_EQ(last_phr_msg->phr.get_se_phr().ph, phr_info.phr.get_se_phr().ph);
    EXPECT_EQ(last_phr_msg->phr.get_se_phr().p_cmax, phr_info.phr.get_se_phr().p_cmax);
    EXPECT_EQ(last_phr_msg->phr.get_se_phr().serv_cell_id, phr_info.phr.get_se_phr().serv_cell_id);
    EXPECT_EQ(last_phr_msg->phr.get_se_phr().ph_type, phr_info.phr.get_se_phr().ph_type);
  }
};

// Helper struct that creates a MAC UL to test the correct processing of RX indication messages.
struct test_bench {
  static constexpr unsigned DEFAULT_ACTIVITY_TIMEOUT = 100;

  test_bench(du_cell_index_t cell_idx_) : cell_idx(cell_idx_)
  {
    rx_msg_sbsr.cell_index = cell_idx;
    rx_msg_sbsr.sl_rx      = slot_point(0, 1);

    logger.set_level(srslog::basic_levels::debug);
    srslog::init();
  }

  test_bench(rnti_t rnti, du_ue_index_t du_ue_idx, du_cell_index_t cell_idx_) : test_bench(cell_idx_)
  {
    add_ue(rnti, du_ue_idx);
  }

  // Add a UE to the RNTI table.
  void add_ue(rnti_t rnti, du_ue_index_t du_ue_idx, unsigned activity_timeout = DEFAULT_ACTIVITY_TIMEOUT)
  {
    srsran_assert(not rnti_table.has_rnti(rnti), "RNTI={:#x} already exists", rnti);
    srsran_assert(not test_ues.contains(du_ue_idx), "ueId={:#x} already exists", rnti);
    rnti_table.add_ue(rnti, du_ue_idx);
    test_ues.emplace(du_ue_idx);
    test_ues[du_ue_idx].rnti     = rnti;
    test_ues[du_ue_idx].ue_index = du_ue_idx;
    test_ues[du_ue_idx].add_bearer(LCID_SRB1);
    async_task<bool>         t = mac_ul.add_ue(test_ues[du_ue_idx].make_ue_create_request());
    lazy_task_launcher<bool> launcher(t);
    srsran_assert(t.ready(), "UE addition should have completed");
  }

  // Add a PDU to the list of PDUs that will be included in the RX indication message.
  void enqueue_pdu(rnti_t rnti, byte_buffer& pdu_payload)
  {
    // Create PDU content.
    mac_rx_pdu pdu{.rnti = rnti, .rapid = 1, .harq_id = 0};
    pdu.pdu.append(pdu_payload);

    // Add PDU to the list in the RX indication message.
    rx_msg_sbsr.pdus.push_back(pdu);
  }

  // Send a RX indication message to the MAC UL and call the manual task executor.
  void send_rx_indication_msg(rnti_t rnti, byte_buffer& pdu_payload)
  {
    // Add PDU to the list of PDUs to be included in the RX indication message.
    enqueue_pdu(rnti, pdu_payload);

    // Send RX data indication to MAC UL.
    mac_ul.handle_rx_data_indication(rx_msg_sbsr);

    // Call task executor manually.
    while (task_exec.has_pending_tasks()) {
      task_exec.try_run_next();
    }
  }

  // Call the dummy scheduler to compare the SR indication with a benchmark message.
  bool verify_sched_req_notification(du_ue_index_t ue_index) { return sched_ce_handler.verify_sched_req_msg(ue_index); }

  // Call the dummy scheduler to compare the BSR indication with a benchmark message.
  void verify_sched_bsr_notification(const mac_bsr_ce_info& bsr) { sched_ce_handler.verify_bsr_msg(bsr); }

  // Call the dummy scheduler to compare the PHR indication with a benchmark message.
  void verify_sched_phr_notification(const mac_phr_ce_info& phr) { sched_ce_handler.verify_phr_msg(phr); }

  // Call the dummy DU notifier to compare the UL CCCH indication with a benchmark message.
  bool verify_du_ul_ccch_msg(const ul_ccch_indication_message& test_msg)
  {
    return du_mng_notifier.verify_ul_ccch_msg(test_msg);
  }

  const slotted_array<mac_test_ue, MAX_NOF_DU_UES>& get_test_ues() const { return test_ues; }

  void run_slot()
  {
    logger.set_level(srslog::basic_levels::debug);
    task_exec.run_pending_tasks();
  }

  bool verify_no_bsr_notification(rnti_t rnti) const
  {
    return not sched_ce_handler.last_bsr_msg.has_value() or sched_ce_handler.last_bsr_msg->rnti != rnti;
  }

  bool verify_no_sr_notification(rnti_t rnti) const
  {
    return not sched_ce_handler.last_sched_cmd.has_value() or sched_ce_handler.last_sched_cmd->rnti != rnti;
  }

private:
  srslog::basic_logger&       logger = srslog::fetch_basic_logger("MAC", true);
  manual_task_worker          task_exec{128};
  dummy_ue_executor_mapper    ul_exec_mapper{task_exec};
  dummy_mac_event_indicator   du_mng_notifier;
  du_rnti_table               rnti_table;
  dummy_sched_ce_info_handler sched_ce_handler;
  dummy_mac_pcap              pcap;
  mac_ul_config               cfg{task_exec, ul_exec_mapper, du_mng_notifier, sched_ce_handler, rnti_table, pcap};
  // This is the RNTI of the UE that appears in the mac_rx_pdu created by send_rx_indication_msg()
  du_cell_index_t        cell_idx;
  mac_ul_processor       mac_ul{cfg};
  mac_rx_data_indication rx_msg_sbsr;

  slotted_array<mac_test_ue, MAX_NOF_DU_UES> test_ues;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Test UL MAC processing of RX indication message with MAC PDU for UL CCCH 48 bits.
TEST(mac_ul_processor, decode_ul_ccch_48bit)
{
  // Define UE and create test_bench.
  rnti_t          ue1_rnti = to_rnti(0x4602);
  du_ue_index_t   ue1_idx  = to_du_ue_index(1U);
  du_cell_index_t cell_idx = to_du_cell_index(0U);
  test_bench      t_bench(ue1_rnti, ue1_idx, cell_idx);

  // Create PDU content.
  // R/LCID MAC subheader | MAC SDU (UL CCCH 48 bits)
  // { 0x34  | 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06}  (Random 6B sequence)
  byte_buffer pdu({0x34, 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06});

  // Send RX data indication to MAC UL.
  t_bench.send_rx_indication_msg(to_rnti(0x4601), pdu);

  // Create UL CCCH indication msg to verify MAC processing of PDU.
  struct ul_ccch_indication_message ul_ccch_msg {};
  ul_ccch_msg.cell_index = cell_idx;
  ul_ccch_msg.slot_rx    = slot_point{0, 1};
  ul_ccch_msg.tc_rnti    = to_rnti(0x4601);
  // Remove R/R/LCID header (0x34) from PDU
  ul_ccch_msg.subpdu.append({0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06});

  // Test if notification sent to DU manager has been received and it is correct.
  ASSERT_TRUE(t_bench.verify_du_ul_ccch_msg(ul_ccch_msg));
}

// Test UL MAC processing of RX indication message with MAC PDU for UL CCCH 64 bits.
TEST(mac_ul_processor, decode_ul_ccch_64bit)
{
  // Define UE and create test_bench.
  rnti_t          ue1_rnti = to_rnti(0x4602);
  du_ue_index_t   ue1_idx  = to_du_ue_index(0U);
  du_cell_index_t cell_idx = to_du_cell_index(0U);
  test_bench      t_bench(ue1_rnti, ue1_idx, cell_idx);

  // Create PDU content.
  // R/LCID MAC subheader | MAC SDU (UL CCCH 64 bits)
  // { 0x00  | 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06, 0x13, 0x54}  (Random 8B sequence)
  byte_buffer pdu({0x00, 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06, 0x13, 0x54});

  // Send RX data indication to MAC UL.
  t_bench.send_rx_indication_msg(to_rnti(0x4601), pdu);

  // Create UL CCCH indication msg to verify MAC processing of PDU.
  struct ul_ccch_indication_message ul_ccch_msg {};
  ul_ccch_msg.cell_index = cell_idx;
  ul_ccch_msg.slot_rx    = slot_point{0, 1};
  ul_ccch_msg.tc_rnti    = to_rnti(0x4601);
  // Remove R/R/LCID header (0x00) from PDU
  ul_ccch_msg.subpdu.append({0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06, 0x13, 0x54});

  // Test if notification sent to DU manager has been received and it is correct.
  ASSERT_TRUE(t_bench.verify_du_ul_ccch_msg(ul_ccch_msg));
}

// Test UL MAC processing of RX indication message with MAC PDU for MAC CE Short BSR.
TEST(mac_ul_processor, decode_short_bsr)
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
  t_bench.send_rx_indication_msg(ue1_rnti, pdu);

  // Create UL BSR indication  message to compare with one passed to the scheduler.
  mac_bsr_ce_info bsr;
  bsr.cell_index  = cell_idx;
  bsr.ue_index    = ue1_idx;
  bsr.rnti        = ue1_rnti;
  bsr.bsr_fmt     = bsr_format::SHORT_BSR;
  bsr.lcg_reports = {lcg_bsr_report{.lcg_id = uint_to_lcg_id(2U), .buffer_size = 25}};

  // Test if notification sent to Scheduler has been received and it is correct.
  ASSERT_NO_FATAL_FAILURE(t_bench.verify_sched_bsr_notification(bsr));
}

// Test UL MAC processing of RX indication message with MAC PDU for MAC CE Short Truncated BSR.
TEST(mac_ul_processor, decode_short_trunc_bsr)
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
  t_bench.send_rx_indication_msg(ue1_rnti, pdu);

  // Create UL BSR indication  message to compare with one passed to the scheduler.
  mac_bsr_ce_info bsr;
  bsr.cell_index  = cell_idx;
  bsr.ue_index    = ue1_idx;
  bsr.rnti        = ue1_rnti;
  bsr.bsr_fmt     = bsr_format::SHORT_TRUNC_BSR;
  bsr.lcg_reports = {lcg_bsr_report{.lcg_id = uint_to_lcg_id(5U), .buffer_size = 14}};

  // Test if notification sent to Scheduler has been received and it is correct.
  ASSERT_NO_FATAL_FAILURE(t_bench.verify_sched_bsr_notification(bsr));
}

// Test UL MAC processing of RX indication message with MAC PDU for MAC CE Long BSR.
TEST(mac_ul_processor, decode_long_bsr)
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
  t_bench.send_rx_indication_msg(ue1_rnti, pdu);

  // Create UL BSR indication message to compare with one passed to the scheduler.
  mac_bsr_ce_info bsr;
  bsr.cell_index  = cell_idx;
  bsr.ue_index    = ue1_idx;
  bsr.rnti        = ue1_rnti;
  bsr.bsr_fmt     = bsr_format::LONG_BSR;
  bsr.lcg_reports = {lcg_bsr_report{.lcg_id = uint_to_lcg_id(0U), .buffer_size = 217},
                     lcg_bsr_report{.lcg_id = uint_to_lcg_id(7U), .buffer_size = 171}};
  ASSERT_NO_FATAL_FAILURE(t_bench.verify_sched_bsr_notification(bsr));
}

// Test UL MAC processing of RX indication message with MAC PDU and invalid MAC CE Long BSR.
TEST(mac_ul_processor, decode_invalid_long_bsr)
{
  // Define UE and create test_bench.
  rnti_t          ue1_rnti = to_rnti(0x4601);
  du_ue_index_t   ue1_idx  = to_du_ue_index(1U);
  du_cell_index_t cell_idx = to_du_cell_index(1U);
  test_bench      t_bench(ue1_rnti, ue1_idx, cell_idx);

  // Create PDU content.
  // R/F/LCID/L MAC subheader | MAC CE Long BSR
  // { 0x3e, 0x03 | 0x81, 0xd9, 0xab }
  byte_buffer pdu({0x3e, 0x02, 0x1, 0xff});

  // Send RX data indication to MAC UL.
  t_bench.send_rx_indication_msg(ue1_rnti, pdu);

  // Create UL BSR indication message to compare with one passed to the scheduler.
  mac_bsr_ce_info bsr;
  bsr.cell_index  = cell_idx;
  bsr.ue_index    = ue1_idx;
  bsr.rnti        = ue1_rnti;
  bsr.bsr_fmt     = bsr_format::LONG_BSR;
  bsr.lcg_reports = {lcg_bsr_report{.lcg_id = uint_to_lcg_id(0U), .buffer_size = 255}};
  ASSERT_TRUE(t_bench.verify_no_bsr_notification(ue1_rnti));
}

// Test UL MAC processing of RX indication message with MAC PDU for MAC CE C-RNTI.
TEST(mac_ul_processor, decode_crnti_ce)
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
  // { 0x3a | 0x46, 0x01 }
  byte_buffer pdu({0x3a, 0x46, 0x01});

  // Send RX data indication to MAC UL.
  t_bench.send_rx_indication_msg(ue1_rnti, pdu);

  // Test if notification sent to Scheduler has been received and it is correct.
  ASSERT_TRUE(t_bench.verify_sched_req_notification(to_du_ue_index(1)));
}

// Test UL MAC processing of RX indication message with MAC PDU for multiple subPDUs (MAC CE C-RNTI, MAC CE Short BSR).
TEST(mac_ul_processor, decode_crnti_ce_and_sbsr)
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
  // { 0x3a | 0x46, 0x01 }
  byte_buffer pdu_ce_crnti({0x3a, 0x46, 0x01});
  t_bench.enqueue_pdu(ue1_rnti, pdu_ce_crnti);

  // Create subPDU content.
  // R/LCID MAC subheader | MAC CE Short BSR
  // { 0x3d | 0x59}
  byte_buffer pdu_sbsr({0x3d, 0x59});
  // Send RX data indication to MAC UL
  t_bench.send_rx_indication_msg(ue1_rnti, pdu_sbsr);

  // Create UL Sched Req indication message (generated by MAC CE C-RNTI) to compare with one passed to the scheduler.
  // Test if notification sent to Scheduler has been received and it is correct.
  ASSERT_TRUE(t_bench.verify_sched_req_notification(to_du_ue_index(1U)));

  // Create UL BSR indication message to compare with one passed to the scheduler.
  mac_bsr_ce_info bsr;
  bsr.cell_index  = cell_idx;
  bsr.ue_index    = ue1_idx;
  bsr.rnti        = ue1_rnti;
  bsr.bsr_fmt     = bsr_format::SHORT_BSR;
  bsr.lcg_reports = {lcg_bsr_report{.lcg_id = uint_to_lcg_id(2U), .buffer_size = 25}};
  ASSERT_NO_FATAL_FAILURE(t_bench.verify_sched_bsr_notification(bsr));
}

// Test UL MAC processing of RX indication message with MAC PDU for multiple subPDUs (MAC CE C-RNTI, MAC CE Short BSR),
// when old C-RNTI does not exist.
TEST(mac_ul_processor, handle_crnti_ce_with_inexistent_old_crnti)
{
  // Define UE and create test_bench.
  rnti_t          ue2_rnti = to_rnti(0x4602);
  du_ue_index_t   ue2_idx  = to_du_ue_index(2U);
  du_cell_index_t cell_idx = to_du_cell_index(0U);
  test_bench      t_bench(cell_idx);
  t_bench.add_ue(ue2_rnti, ue2_idx);

  // Create PDU content.
  byte_buffer pdu;
  // > Create subPDU content.
  // R/LCID MAC subheader | MAC CE C-RNTI
  // { 0x3a | 0x46, 0x01 }
  byte_buffer ce_crnti({0x3a, 0x46, 0x01});
  pdu.append(ce_crnti);
  // > Create subPDU content.
  // R/LCID MAC subheader | MAC CE Short BSR
  // { 0x3d | 0x59}
  byte_buffer sbsr({0x3d, 0x59});
  pdu.append(sbsr);

  // Send RX data indication to MAC UL
  t_bench.send_rx_indication_msg(ue2_rnti, pdu);

  // Ensure Scheduler did not get notified of any BSR.
  ASSERT_TRUE(t_bench.verify_no_bsr_notification(to_rnti(0x4601)));
  ASSERT_TRUE(t_bench.verify_no_bsr_notification(ue2_rnti));
  ASSERT_TRUE(t_bench.verify_no_sr_notification(to_rnti(0x4601)));
}

// Test UL MAC processing of RX indication message with MAC PDU for MAC CE Single Entry PHR.
TEST(mac_ul_processor, verify_single_entry_phr)
{
  // Define UE and create test_bench.
  const rnti_t          ue1_rnti = to_rnti(0x4601);
  const du_ue_index_t   ue1_idx  = to_du_ue_index(1U);
  const du_cell_index_t cell_idx = to_du_cell_index(1U);
  test_bench            t_bench(ue1_rnti, ue1_idx, cell_idx);

  // Create PDU content.
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
  byte_buffer pdu({0x39, 0x27, 0x2f});

  // Send RX data indication to MAC UL
  t_bench.send_rx_indication_msg(ue1_rnti, pdu);

  // Create UL PHR indication message to compare with one passed to the scheduler.
  mac_phr_ce_info phr_ind{};
  phr_ind.cell_index = cell_idx;
  phr_ind.ue_index   = ue1_idx;
  phr_ind.rnti       = ue1_rnti;
  phr_ind.phr.set_se_phr({.serv_cell_id = to_du_cell_index(0),
                          .ph_type      = srsran::ph_field_type_t::type1,
                          .ph           = ph_db_range(6, 7),
                          .p_cmax       = p_cmax_dbm_range(17, 18)});

  // Test if notification sent to Scheduler has been received and it is correct.
  ASSERT_NO_FATAL_FAILURE(t_bench.verify_sched_phr_notification(phr_ind));
}
