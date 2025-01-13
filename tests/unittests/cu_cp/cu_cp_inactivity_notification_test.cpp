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

#include "cu_cp_test_environment.h"
#include "tests/test_doubles/ngap/ngap_test_message_validators.h"
#include "tests/unittests/cu_cp/test_helpers.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/ngap/ngap_message.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class cu_cp_inactivity_notification_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_inactivity_notification_test() : cu_cp_test_environment(cu_cp_test_env_params{})
  {
    // Run NG setup to completion.
    run_ng_setup();

    // Setup DU.
    std::optional<unsigned> ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    du_idx = ret.value();
    EXPECT_TRUE(this->run_f1_setup(du_idx));

    // Setup CU-UP.
    ret = connect_new_cu_up();
    EXPECT_TRUE(ret.has_value());
    cu_up_idx = ret.value();
    EXPECT_TRUE(this->run_e1_setup(cu_up_idx));

    // Connect UE 0x4601.
    EXPECT_TRUE(attach_ue(du_idx, cu_up_idx, du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id, psi, drb_id_t::drb1, qfi));
    ue_ctx = this->find_ue_context(du_idx, du_ue_id);
    EXPECT_NE(ue_ctx, nullptr);
  }

  [[nodiscard]] bool
  setup_pdu_session(pdu_session_id_t psi_,
                    drb_id_t         drb_id_,
                    qos_flow_id_t    qfi_,
                    byte_buffer      rrc_reconfiguration_complete = make_byte_buffer("00070e00cc6fcda5").value(),
                    bool             is_initial_session_          = true)
  {
    return cu_cp_test_environment::setup_pdu_session(du_idx,
                                                     cu_up_idx,
                                                     du_ue_id,
                                                     crnti,
                                                     cu_up_e1ap_id,
                                                     psi_,
                                                     drb_id_,
                                                     qfi_,
                                                     std::move(rrc_reconfiguration_complete),
                                                     is_initial_session_);
  }

  [[nodiscard]] bool send_bearer_context_inactivity_notification(const e1ap_message& inactivity_notification)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject inactivity notification
    get_cu_up(cu_up_idx).push_tx_pdu(inactivity_notification);
    return true;
  }

  [[nodiscard]] bool send_ue_level_bearer_context_inactivity_notification_and_await_ue_context_release_request()
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject inactivity notification and wait for UE Context Release Request
    if (!send_bearer_context_inactivity_notification(generate_bearer_context_inactivity_notification_with_ue_level(
            ue_ctx->cu_cp_e1ap_id.value(), cu_up_e1ap_id))) {
      return false;
    }
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu), "Failed to receive UE Context Release Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_request(ngap_pdu),
                              "Invalid UE Context Release Request");
    return true;
  }

  [[nodiscard]] bool send_drb_level_bearer_context_inactivity_notification_and_await_ue_context_release_request()
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject inactivity notification and wait for UE Context Release Request
    if (!send_bearer_context_inactivity_notification(generate_bearer_context_inactivity_notification_with_drb_level(
            ue_ctx->cu_cp_e1ap_id.value(), cu_up_e1ap_id, {}, {drb_id_t::drb1}))) {
      return false;
    }
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu), "Failed to receive UE Context Release Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_request(ngap_pdu),
                              "Invalid UE Context Release Request");
    return true;
  }

  [[nodiscard]] bool
  send_pdu_session_level_bearer_context_inactivity_notification_and_await_ue_context_release_request()
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject inactivity notification and wait for UE Context Release Request
    if (!send_bearer_context_inactivity_notification(
            generate_bearer_context_inactivity_notification_with_pdu_session_level(
                ue_ctx->cu_cp_e1ap_id.value(), cu_up_e1ap_id, {}, {psi}))) {
      return false;
    }
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu), "Failed to receive UE Context Release Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_request(ngap_pdu),
                              "Invalid UE Context Release Request");
    return true;
  }

  unsigned du_idx    = 0;
  unsigned cu_up_idx = 0;

  gnb_du_ue_f1ap_id_t    du_ue_id      = gnb_du_ue_f1ap_id_t::min;
  rnti_t                 crnti         = to_rnti(0x4601);
  amf_ue_id_t            amf_ue_id     = amf_ue_id_t::min;
  gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id = gnb_cu_up_ue_e1ap_id_t::min;

  const ue_context* ue_ctx = nullptr;

  pdu_session_id_t psi  = uint_to_pdu_session_id(1);
  pdu_session_id_t psi2 = uint_to_pdu_session_id(2);
  qos_flow_id_t    qfi  = uint_to_qos_flow_id(1);
  qos_flow_id_t    qfi2 = uint_to_qos_flow_id(2);

  ngap_message ngap_pdu;
  e1ap_message e1ap_pdu;
};

TEST_F(cu_cp_inactivity_notification_test,
       when_unsupported_inactivity_message_received_then_ue_context_release_request_is_not_sent)
{
  // Setup second PDU session
  ASSERT_TRUE(setup_pdu_session(psi2, drb_id_t::drb2, qfi2, generate_rrc_reconfiguration_complete_pdu(0, 8), false));

  // Inject unsupported Inactivity Notification
  e1ap_message inactivity_notification = generate_bearer_context_inactivity_notification_with_drb_level(
      ue_ctx->cu_cp_e1ap_id.value(), cu_up_e1ap_id, {drb_id_t::drb1}, {drb_id_t::drb2});
  ASSERT_TRUE(send_bearer_context_inactivity_notification(inactivity_notification));

  // check that the UE Context Release Request was not sent to the AMF
  srsran_assert(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                "UE Context Release for unsupported inactivity message was sent to AMF");
}

TEST_F(cu_cp_inactivity_notification_test,
       when_ue_level_inactivity_message_received_then_ue_context_release_request_is_sent)
{
  // Inject Inactivity Notification and await UE Context Release Request
  ASSERT_TRUE(send_ue_level_bearer_context_inactivity_notification_and_await_ue_context_release_request());
}

TEST_F(cu_cp_inactivity_notification_test,
       when_drb_level_inactivity_message_with_inactivity_for_all_drbs_received_then_ue_context_release_request_is_sent)
{
  // Inject Inactivity Notification and await UE Context Release Request
  ASSERT_TRUE(send_drb_level_bearer_context_inactivity_notification_and_await_ue_context_release_request());
}

TEST_F(
    cu_cp_inactivity_notification_test,
    when_pdu_session_level_inactivity_message_with_inactivity_for_all_drbs_received_then_ue_context_release_request_is_sent)
{
  // Inject Inactivity Notification and await UE Context Release Request
  ASSERT_TRUE(send_pdu_session_level_bearer_context_inactivity_notification_and_await_ue_context_release_request());
}
