/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "tests/integrationtests/du_high/test_utils/du_high_env_simulator.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/mac/mac_test_messages.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/f1ap/f1ap_message.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;
using namespace asn1::f1ap;

class du_high_connectivity_test : public du_high_env_simulator, public testing::Test
{
protected:
  du_high_connectivity_test() : du_high_env_simulator(du_high_env_sim_params{.nof_cells = 1, .auto_start = false}) {}

  void run_f1_setup()
  {
    // Start DU and try to connect to CU.
    this->du_hi->start();

    // Ensure the result is saved in the notifier.
    run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); });

    // Clearing the F1AP messages sent during setup for further tests.
    cu_notifier.f1ap_ul_msgs.clear();
  }
};

TEST_F(du_high_connectivity_test, when_du_does_not_start_then_no_f1_setup_is_sent)
{
  // DU-high is not started, so no F1 setup should be sent.
  ASSERT_TRUE(this->cu_notifier.f1ap_ul_msgs.empty());
}

TEST_F(du_high_connectivity_test, when_du_starts_it_then_initiates_f1_setup)
{
  // Start DU and try to connect to CU.
  this->du_hi->start();

  // Ensure the F1 Setup Request is saved in the CU notifier.
  ASSERT_TRUE(run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); }));

  // Starting the DU-high initiates the F1 Setup procedure.
  ASSERT_EQ(this->cu_notifier.f1ap_ul_msgs.size(), 1);
  ASSERT_EQ(this->cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.type().value, f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(this->cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.init_msg().proc_code, ASN1_F1AP_ID_F1_SETUP);
}

TEST_F(du_high_connectivity_test, when_f1_connection_is_lost_then_du_detects_connection_loss)
{
  run_f1_setup();
  ASSERT_FALSE(cu_notifier.du_released_connection());

  // Trigger a TNL disconnection (and the channel will stay down).
  cu_notifier.set_f1_channel_state(false);

  // DU releases its F1 connection, it will attempt to establish a new one and fail.
  cu_notifier.f1ap_ul_msgs.clear();
  ASSERT_FALSE(cu_notifier.du_released_connection());
  workers.flush_pending_control_tasks();
  ASSERT_TRUE(cu_notifier.du_released_connection());

  // Ensure no F1 setup request is sent (because TNL association failed).
  ASSERT_FALSE(run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); }, 200));
}

TEST_F(du_high_connectivity_test, when_f1_connection_is_lost_and_reestablished_then_du_completes_f1_setup)
{
  run_f1_setup();

  // Signal a temporary F1 connection loss. The DU should retry the connection.
  cu_notifier.set_f1_channel_state(false);
  cu_notifier.set_f1_channel_state(true);

  ASSERT_TRUE(cu_notifier.f1ap_ul_msgs.empty());

  // DU stops and restarts the F1 setup procedure after detecting that the connection is back up.
  run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); });
  ASSERT_EQ(this->cu_notifier.f1ap_ul_msgs.size(), 1);
  ASSERT_EQ(this->cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.type().value, f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(this->cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.init_msg().proc_code, ASN1_F1AP_ID_F1_SETUP);
  test_logger.info("STATUS: DU successfully retried F1 Setup after connection loss.");
}

TEST_F(du_high_connectivity_test, when_f1_connection_is_lost_then_ues_are_removed)
{
  // Run F1 Setup
  run_f1_setup();

  // Add UE
  du_hi->get_pdu_handler().handle_rx_data_indication(test_helpers::create_ccch_message(next_slot, to_rnti(0x4601)));
  this->run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); });
  ASSERT_EQ(cu_notifier.f1ap_ul_msgs.size(), 1);
  ASSERT_TRUE(
      test_helpers::is_init_ul_rrc_msg_transfer_valid(cu_notifier.f1ap_ul_msgs.rbegin()->second, to_rnti(0x4601)));
  cu_notifier.f1ap_ul_msgs.clear();

  // Signal a temporary F1 connection loss. The DU should retry the connection.
  cu_notifier.set_f1_channel_state(false);
  cu_notifier.set_f1_channel_state(true);
  run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); });
  cu_notifier.f1ap_ul_msgs.clear();
  test_logger.info("STATUS: DU successfully retried F1 Setup after connection loss.");

  // Add new UE
  du_hi->get_pdu_handler().handle_rx_data_indication(test_helpers::create_ccch_message(next_slot, to_rnti(0x4602)));
  this->run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); });
  ASSERT_EQ(cu_notifier.f1ap_ul_msgs.size(), 1);
  ASSERT_TRUE(
      test_helpers::is_init_ul_rrc_msg_transfer_valid(cu_notifier.f1ap_ul_msgs.rbegin()->second, to_rnti(0x4602)));
}
