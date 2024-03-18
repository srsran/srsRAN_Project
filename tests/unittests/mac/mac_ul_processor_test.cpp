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

#include "lib/mac/mac_ul/mac_scheduler_ce_info_handler.h"
#include "lib/mac/mac_ul/mac_ul_processor.h"
#include "lib/mac/rnti_manager.h"
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
  optional<du_ue_index_t>             last_crnti_ce;

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
  void handle_ul_phr_indication(const mac_phr_ce_info& phr) override { last_phr_msg = phr; }

  void handle_crnti_ce_indication(du_ue_index_t old_ue_index, du_cell_index_t cell_index) override
  {
    last_crnti_ce = old_ue_index;
  }

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

  // Allocate new TC-RNTI.
  rnti_t allocate_tc_rnti() { return rnti_mng.allocate(); }

  // Add a UE to the RNTI table and UE context repository.
  void add_ue(rnti_t rnti, du_ue_index_t du_ue_idx)
  {
    srsran_assert(not rnti_mng.has_rnti(rnti), "rnti={} already exists", rnti);
    srsran_assert(not test_ues.contains(du_ue_idx), "ueId={} already exists", rnti);
    rnti_mng.add_ue(rnti, du_ue_idx);
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
    ASSERT_TRUE(pdu.pdu.append(pdu_payload));

    // Add PDU to the list in the RX indication message.
    rx_msg_sbsr.pdus.push_back(pdu);
  }

  // Send a RX indication message to the MAC UL and call the manual task executor.
  void send_rx_indication_msg(rnti_t rnti, byte_buffer& pdu_payload)
  {
    // Add PDU to the list of PDUs to be included in the RX indication message.
    enqueue_pdu(rnti, pdu_payload);

    // Send RX data indication to MAC UL.
    send_rx_indication_msg(rx_msg_sbsr);
  }

  void send_rx_indication_msg(mac_rx_data_indication rx_ind)
  {
    // Send RX data indication to MAC UL.
    mac_ul.handle_rx_data_indication(rx_ind);

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

  // Call the dummy DU notifier to ensure no UL CCCH indication was forwarded.
  bool verify_no_ul_ccch_msg() { return du_mng_notifier.verify_no_ul_ccch_msg(); }

  const slotted_array<mac_test_ue, MAX_NOF_DU_UES>& get_test_ues() const { return test_ues; }

  void run_slot()
  {
    logger.set_level(srslog::basic_levels::debug);
    task_exec.run_pending_tasks();
  }

  bool verify_no_bsr_notification() const { return not sched_ce_handler.last_bsr_msg.has_value(); }

  bool verify_no_bsr_notification(rnti_t rnti) const
  {
    return verify_no_bsr_notification() or sched_ce_handler.last_bsr_msg->rnti != rnti;
  }

  bool verify_no_sr_notification() const { return not sched_ce_handler.last_sched_cmd.has_value(); }
  bool verify_no_sr_notification(rnti_t rnti) const
  {
    return verify_no_sr_notification() or sched_ce_handler.last_sched_cmd->rnti != rnti;
  }

  const dummy_sched_ce_info_handler& sched_ce_notifier() { return sched_ce_handler; }

private:
  srslog::basic_logger&       logger = srslog::fetch_basic_logger("MAC", true);
  manual_task_worker          task_exec{128};
  dummy_ue_executor_mapper    ul_exec_mapper{task_exec};
  dummy_mac_event_indicator   du_mng_notifier;
  rnti_manager                rnti_mng;
  dummy_sched_ce_info_handler sched_ce_handler;
  null_mac_pcap               pcap;
  mac_ul_config               cfg{task_exec, ul_exec_mapper, du_mng_notifier, sched_ce_handler, rnti_mng, pcap};
  // This is the RNTI of the UE that appears in the mac_rx_pdu created by send_rx_indication_msg()
  du_cell_index_t        cell_idx;
  mac_ul_processor       mac_ul{cfg};
  mac_rx_data_indication rx_msg_sbsr;

  slotted_array<mac_test_ue, MAX_NOF_DU_UES> test_ues;
};

mac_rx_data_indication create_rx_data_indication(du_cell_index_t cell_idx, rnti_t rnti, byte_buffer pdu_payload)
{
  mac_rx_data_indication rx_ind{slot_point{subcarrier_spacing::kHz15, 0}, cell_idx, {}};
  rx_ind.pdus.push_back(mac_rx_pdu{.rnti = rnti, .rapid = 1, .harq_id = 0});
  rx_ind.pdus.back().pdu = std::move(pdu_payload);
  return rx_ind;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Test UL MAC processing of RX indication message with MAC PDU for UL CCCH 48 bits.
TEST(mac_ul_processor, decode_ul_ccch_48bit)
{
  // Define UE and create test_bench.
  du_cell_index_t cell_idx = to_du_cell_index(0U);
  test_bench      t_bench(cell_idx);
  rnti_t          tc_rnti = t_bench.allocate_tc_rnti();

  // Create PDU content.
  // R/LCID MAC subheader | MAC SDU (UL CCCH 48 bits)
  // { 0x34  | 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06}  (Random 6B sequence)
  byte_buffer payload = byte_buffer::create({0x34, 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06}).value();

  // Send RX data indication to MAC UL.
  t_bench.send_rx_indication_msg(tc_rnti, payload);

  // Create UL CCCH indication msg to verify MAC processing of PDU.
  struct ul_ccch_indication_message ul_ccch_msg {};
  ul_ccch_msg.cell_index = cell_idx;
  ul_ccch_msg.slot_rx    = slot_point{0, 1};
  ul_ccch_msg.tc_rnti    = tc_rnti;
  // Remove R/R/LCID header (0x34) from PDU
  ASSERT_TRUE(ul_ccch_msg.subpdu.append({0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06}));

  // Test if notification sent to DU manager has been received and it is correct.
  ASSERT_TRUE(t_bench.verify_du_ul_ccch_msg(ul_ccch_msg));
}

// Test UL MAC processing of RX indication message with MAC PDU for UL CCCH 64 bits.
TEST(mac_ul_processor, decode_ul_ccch_64bit)
{
  // Define UE and create test_bench.
  du_cell_index_t cell_idx = to_du_cell_index(0U);
  test_bench      t_bench(cell_idx);
  rnti_t          tc_rnti = t_bench.allocate_tc_rnti();

  // Create PDU content.
  // R/LCID MAC subheader | MAC SDU (UL CCCH 64 bits)
  // { 0x00  | 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06, 0x13, 0x54}  (Random 8B sequence)
  byte_buffer payload = byte_buffer::create({0x00, 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06, 0x13, 0x54}).value();

  // Send RX data indication to MAC UL.
  t_bench.send_rx_indication_msg(tc_rnti, payload);

  // Create UL CCCH indication msg to verify MAC processing of PDU.
  struct ul_ccch_indication_message ul_ccch_msg {};
  ul_ccch_msg.cell_index = cell_idx;
  ul_ccch_msg.slot_rx    = slot_point{0, 1};
  ul_ccch_msg.tc_rnti    = tc_rnti;
  // Remove R/R/LCID header (0x00) from PDU
  ASSERT_TRUE(ul_ccch_msg.subpdu.append({0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06, 0x13, 0x54}));

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
  byte_buffer pdu = byte_buffer::create({0x3d, 0x59}).value();

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
  byte_buffer pdu = byte_buffer::create({0x3b, 0xae}).value();

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
  byte_buffer pdu = byte_buffer::create({0x3e, 0x03, 0x81, 0xd9, 0xab}).value();

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
  byte_buffer pdu = byte_buffer::create({0x3e, 0x02, 0x1, 0xff}).value();

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
  du_cell_index_t cell_idx = to_du_cell_index(1U);
  test_bench      t_bench(cell_idx);
  // Add a UE. This RNTI (0x4601) is the one carried by the MAC CE C-RNTI and should be used in the sr_ind{} below.
  t_bench.add_ue(to_rnti(0x4601), to_du_ue_index(1U));
  // Add a TC-RNTI, which will be used to send the C-RNTI CE in Msg3.
  rnti_t tc_rnti = t_bench.allocate_tc_rnti();

  // Create PDU content.
  // R/LCID MAC subheader | MAC CE C-RNTI
  // { 0x3a | 0x46, 0x01 }
  byte_buffer pdu = byte_buffer::create({0x3a, 0x46, 0x01}).value();

  // Send RX data indication to MAC UL.
  t_bench.send_rx_indication_msg(tc_rnti, pdu);

  // Test that C-RNTI CE was notified to the scheduler.
  ASSERT_EQ(t_bench.sched_ce_notifier().last_crnti_ce, to_du_ue_index(1U));

  // Test if notification sent to Scheduler has been received and it is correct.
  ASSERT_TRUE(t_bench.verify_sched_req_notification(to_du_ue_index(1)));
}

// Test UL MAC processing of RX indication message with MAC PDU for multiple subPDUs (MAC CE C-RNTI, MAC CE Short BSR).
// The BSR should be directed at the old C-RNTI rather than the TC-RNTI.
TEST(mac_ul_processor, decode_crnti_ce_and_sbsr)
{
  // Define UE and create test_bench.
  du_cell_index_t cell_idx = to_du_cell_index(1U);
  test_bench      t_bench(to_rnti(0x4601), to_du_ue_index(0U), cell_idx);
  // Allocate TC-RNTI which will be used to transfer MAC CE C-RNTI and MAC CE Short BSR.
  rnti_t tc_rnti = t_bench.allocate_tc_rnti();

  // Create PDU content.
  byte_buffer payload;
  // > Create MAC CE C-RNTI subPDU.
  // R/LCID MAC subheader | MAC CE C-RNTI
  // { 0x3a | 0x46, 0x01 }
  ASSERT_TRUE(payload.append(byte_buffer::create({0x3a, 0x46, 0x01}).value()));
  // > Create MAC CE Short BSR subPDU.
  // R/LCID MAC subheader | MAC CE Short BSR
  // { 0x3d | 0x59}
  ASSERT_TRUE(payload.append(byte_buffer::create({0x3d, 0x59}).value()));
  // Send RX data indication to MAC UL
  t_bench.send_rx_indication_msg(create_rx_data_indication(cell_idx, tc_rnti, std::move(payload)));

  // Given that a BSR was included in the Msg3, there is no need to schedule a SR notification to complete the RA
  // procedure.
  ASSERT_TRUE(t_bench.verify_no_sr_notification());

  // Create UL BSR indication message to compare with one passed to the scheduler.
  // Note: The C-RNTI should correspond to the old C-RNTI.
  mac_bsr_ce_info bsr;
  bsr.cell_index  = cell_idx;
  bsr.ue_index    = to_du_ue_index(0);
  bsr.rnti        = to_rnti(0x4601);
  bsr.bsr_fmt     = bsr_format::SHORT_BSR;
  bsr.lcg_reports = {lcg_bsr_report{.lcg_id = uint_to_lcg_id(2U), .buffer_size = 25}};
  ASSERT_NO_FATAL_FAILURE(t_bench.verify_sched_bsr_notification(bsr));
  ASSERT_TRUE(t_bench.verify_no_bsr_notification(tc_rnti));
}

// Test UL MAC processing of RX indication message with MAC PDU for multiple subPDUs (MAC CE C-RNTI, MAC CE Short BSR),
// when old C-RNTI does not exist.
TEST(mac_ul_processor, handle_crnti_ce_with_inexistent_old_crnti)
{
  // Define UE and create test_bench.
  du_cell_index_t cell_idx = to_du_cell_index(0U);
  test_bench      t_bench(cell_idx);
  rnti_t          tc_rnti = t_bench.allocate_tc_rnti();

  // Create PDU content.
  byte_buffer payload;
  // > Create subPDU content.
  // R/LCID MAC subheader | MAC CE C-RNTI
  // { 0x3a | 0x46, 0x01 }
  byte_buffer ce_crnti = byte_buffer::create({0x3a, 0x46, 0x01}).value();
  ASSERT_TRUE(payload.append(ce_crnti));
  // > Create subPDU content.
  // R/LCID MAC subheader | MAC CE Short BSR
  // { 0x3d | 0x59}
  byte_buffer sbsr = byte_buffer::create({0x3d, 0x59}).value();
  ASSERT_TRUE(payload.append(sbsr));

  // Send RX data indication to MAC UL
  t_bench.send_rx_indication_msg(create_rx_data_indication(cell_idx, tc_rnti, std::move(payload)));

  // Ensure Scheduler did not get notified of any BSR.
  ASSERT_TRUE(t_bench.verify_no_bsr_notification());
  ASSERT_TRUE(t_bench.verify_no_bsr_notification());
  ASSERT_TRUE(t_bench.verify_no_sr_notification());
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
  byte_buffer pdu = byte_buffer::create({0x39, 0x27, 0x2f}).value();

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

// Test UL MAC processing of RX indication message with MAC PDU with UL-CCCH CE and Single Entry PHR CE.
TEST(mac_ul_processor, when_ul_ccch_and_phr_are_received_then_phr_is_ignored)
{
  // Define UE and create test_bench.
  const du_cell_index_t cell_idx = to_du_cell_index(1U);
  test_bench            t_bench(cell_idx);
  rnti_t                tc_rnti = t_bench.allocate_tc_rnti();

  // Create PDU content.
  byte_buffer payload;
  // > MAC subPDU with UL-CCCH:
  // R/LCID MAC subheader | MAC SDU (UL CCCH 48 bits)
  // { 0x34  | 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06}  (Random 6B sequence)
  ASSERT_TRUE(payload.append(byte_buffer::create({0x34, 0x1e, 0x4f, 0xc0, 0x04, 0xa6, 0x06}).value()));
  // > MAC subPDU with PHR:
  // R/LCID MAC subheader = R|R|LCID = 0x39 or LCID=57
  // MAC CE SE PHR = {0x27, 0x2f}
  ASSERT_TRUE(payload.append(byte_buffer::create({0x39, 0x27, 0x2f}).value()));

  // Send RX data indication to MAC UL
  t_bench.send_rx_indication_msg(create_rx_data_indication(cell_idx, tc_rnti, std::move(payload)));

  // Note: For now PHRs are ignored in this scenario.
  ASSERT_TRUE(t_bench.verify_no_sr_notification(tc_rnti));
}

TEST(mac_ul_processor, when_pdu_is_filled_with_zerosfor_existing_ue_then_the_mac_pdu_is_discarded)
{
  rnti_t          ue1_rnti = to_rnti(0x4601);
  du_ue_index_t   ue1_idx  = to_du_ue_index(1U);
  du_cell_index_t cell_idx = to_du_cell_index(1U);
  test_bench      t_bench(ue1_rnti, ue1_idx, cell_idx);

  // Create PDU content. The PDU is made of several SDUs with size equal to UL-CCCH CE to be decodeable.
  byte_buffer pdu;
  // R/F/LCID MAC subheader | MAC CE UL-CCCH 64
  byte_buffer ul_ccch = byte_buffer::create({0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}).value();
  ASSERT_TRUE(pdu.append(ul_ccch));
  ASSERT_TRUE(pdu.append(ul_ccch));
  ASSERT_TRUE(pdu.append(ul_ccch));

  // Send RX data indication to MAC UL.
  t_bench.send_rx_indication_msg(ue1_rnti, pdu);

  // Test if notification sent to DU manager has been received and it is correct.
  ASSERT_TRUE(t_bench.verify_no_ul_ccch_msg());
}
