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

#include "lib/f1ap/common/f1ap_asn1_packer.h"
#include "test_utils/du_high_test_bench.h"
#include "tests/unittests/f1ap/du/f1ap_du_test_helpers.h"
#include "tests/unittests/gateways/test_helpers.h"
#include "srsran/support/test_utils.h"

using namespace srsran;
using namespace srs_du;
using namespace asn1::f1ap;

class du_high_tester : public du_high_test_bench, public testing::Test
{};

/// Test F1 setup over "local" connection to DU.
TEST_F(du_high_tester, when_du_high_started_then_connection_to_cu_takes_place)
{
  // Starting the DU-high initiates the F1 Setup procedure.
  ASSERT_EQ(this->cu_notifier.last_f1ap_msgs.size(), 1);
  ASSERT_EQ(this->cu_notifier.last_f1ap_msgs.back().pdu.type().value, f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(this->cu_notifier.last_f1ap_msgs.back().pdu.init_msg().proc_code, ASN1_F1AP_ID_F1_SETUP);
}

TEST_F(du_high_tester, when_ccch_msg_is_received_then_ue_context_is_created)
{
  // Reset the last sent F1AP PDU (e.g. F1 setup).
  cu_notifier.last_f1ap_msgs.clear();

  // Add UE
  du_obj.get_pdu_handler(to_du_cell_index(0))
      .handle_rx_data_indication(create_ccch_message(next_slot, to_rnti(0x4601)));

  this->run_until([this]() { return not cu_notifier.last_f1ap_msgs.empty(); });
  ASSERT_EQ(cu_notifier.last_f1ap_msgs.size(), 1);
  ASSERT_TRUE(is_init_ul_rrc_msg_transfer_valid(cu_notifier.last_f1ap_msgs.back(), to_rnti(0x4601)));
}

TEST_F(du_high_tester, when_two_concurrent_ccch_msg_are_received_then_two_ue_contexts_are_created)
{
  cu_notifier.last_f1ap_msgs.clear();

  // Add two UEs.
  du_obj.get_pdu_handler(to_du_cell_index(0))
      .handle_rx_data_indication(create_ccch_message(next_slot, to_rnti(0x4601)));
  du_obj.get_pdu_handler(to_du_cell_index(1))
      .handle_rx_data_indication(create_ccch_message(next_slot, to_rnti(0x4602)));

  this->run_until([this]() { return cu_notifier.last_f1ap_msgs.size() >= 2; });
  ASSERT_EQ(cu_notifier.last_f1ap_msgs.size(), 2);
  ASSERT_TRUE(is_init_ul_rrc_msg_transfer_valid(cu_notifier.last_f1ap_msgs[0], to_rnti(0x4601)));
  ASSERT_TRUE(is_init_ul_rrc_msg_transfer_valid(cu_notifier.last_f1ap_msgs[1], to_rnti(0x4602)));
}

TEST_F(du_high_tester, when_ue_context_release_received_then_ue_gets_deleted)
{
  // Create UE.
  ASSERT_TRUE(add_ue(to_rnti(0x4601)));

  // Receive UE Context Release Command.
  cu_notifier.last_f1ap_msgs.clear();
  f1ap_message                    msg = generate_ue_context_release_command();
  const ue_context_release_cmd_s& cmd = msg.pdu.init_msg().value.ue_context_release_cmd();
  this->du_obj.get_f1ap_message_handler().handle_message(msg);

  const unsigned MAX_COUNT = 1000;
  for (unsigned i = 0; i != MAX_COUNT; ++i) {
    this->run_slot();

    // Handle PUCCH of the SRB message containing RRC Release.
    for (const pucch_info& pucch : this->phy.cell.last_ul_res->ul_res->pucchs) {
      if (pucch.format == srsran::pucch_format::FORMAT_1 and pucch.format_1.harq_ack_nof_bits > 0) {
        mac_uci_indication_message uci_msg;
        uci_msg.sl_rx = this->phy.cell.last_ul_res->slot;
        uci_msg.ucis  = {mac_uci_pdu{
             .rnti = to_rnti(0x4601),
             .pdu  = mac_uci_pdu::pucch_f0_or_f1_type{.harq_info = mac_uci_pdu::pucch_f0_or_f1_type::harq_information{
                                                          .harqs = {uci_pucch_f0_or_f1_harq_values::ack}}}}};
        this->du_obj.get_control_information_handler(to_du_cell_index(0)).handle_uci(uci_msg);
      }
    }

    // Stop loop, When UE Context Release complete has been sent to CU.
    if (not cu_notifier.last_f1ap_msgs.empty()) {
      break;
    }
  }
  ASSERT_EQ(cu_notifier.last_f1ap_msgs.size(), 1);
  ASSERT_TRUE(is_ue_context_release_complete_valid(cu_notifier.last_f1ap_msgs.back(),
                                                   (gnb_du_ue_f1ap_id_t)cmd->gnb_du_ue_f1ap_id->value,
                                                   (gnb_cu_ue_f1ap_id_t)cmd->gnb_cu_ue_f1ap_id->value));
}

TEST_F(du_high_tester, when_du_high_is_stopped_then_ues_are_removed)
{
  // Create UE.
  ASSERT_TRUE(add_ue(to_rnti(0x4601)));

  // Stop DU from another thread, because stop() call is blocking.
  std::atomic<bool> running{true};
  task_worker       worker("phy_worker", 2048);
  bool              ret = worker.push_task([this, &running]() {
    this->du_obj.stop();
    running = false;
  });
  ASSERT_TRUE(ret);

  // Keep running slot indications until DU-high is stopped.
  for (unsigned count = 0; count < 1000 and running; ++count) {
    this->run_slot();
  }
  ASSERT_FALSE(running) << "stop() call is hanging";
}
