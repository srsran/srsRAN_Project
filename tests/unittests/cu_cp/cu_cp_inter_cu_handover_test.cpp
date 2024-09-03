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

#include "cu_cp_test_environment.h"
#include "tests/test_doubles/e1ap/e1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/ngap/ngap_test_message_validators.h"
#include "tests/unittests/cu_cp/test_helpers.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "tests/unittests/f1ap/common/f1ap_cu_test_messages.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/f1ap/common/f1ap_ue_id.h"
#include "srsran/ngap/ngap_message.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class cu_cp_inter_cu_handover_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_inter_cu_handover_test() : cu_cp_test_environment(cu_cp_test_env_params{8, 8, 8192, create_mock_amf()})
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
  }

  [[nodiscard]] bool attach_ue()
  {
    if (!cu_cp_test_environment::attach_ue(
            du_idx, cu_up_idx, du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id, psi, drb_id_t::drb1, qfi)) {
      return false;
    }
    ue_ctx = this->find_ue_context(du_idx, du_ue_id);

    return ue_ctx != nullptr;
  }

  [[nodiscard]] bool send_handover_request_and_await_bearer_context_setup_request()
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject Handover Request and wait for Bearer Context Setup Request
    get_amf().push_tx_pdu(generate_valid_handover_request(amf_ue_id));
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
                              "Failed to receive Bearer Context Setup Request");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_setup_request(e1ap_pdu),
                              "Invalid Bearer Context Setup Request");

    cu_cp_e1ap_id =
        int_to_gnb_cu_cp_ue_e1ap_id(e1ap_pdu.pdu.init_msg().value.bearer_context_setup_request()->gnb_cu_cp_ue_e1ap_id);
    return true;
  }

  [[nodiscard]] bool send_bearer_context_setup_response_and_await_ue_context_setup_request()
  {
    // Inject Bearer Context Setup Response and wait for UE Context Setup Request
    get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_setup_response(cu_cp_e1ap_id, cu_up_e1ap_id));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive UE Context Setup Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_setup_request(f1ap_pdu),
                              "Invalid UE Context Setup Request");

    cu_ue_id = int_to_gnb_cu_ue_f1ap_id(f1ap_pdu.pdu.init_msg().value.ue_context_setup_request()->gnb_cu_ue_f1ap_id);
    return true;
  }

  [[nodiscard]] bool send_ue_context_setup_response_and_await_bearer_context_modification_request()
  {
    // Inject UE Context Setup Response and wait for Bearer Context Modification Request
    get_du(du_idx).push_ul_pdu(generate_ue_context_setup_response(cu_ue_id, du_ue_id, crnti));
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
                              "Failed to receive Bearer Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_modification_request(e1ap_pdu),
                              "Invalid Bearer Context Modification Request");
    return true;
  }

  [[nodiscard]] bool send_bearer_context_modification_response_and_await_handover_request_ack()
  {
    // Inject Bearer Context Modification Response and wait for Handover Request Ack
    get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_modification_response(cu_cp_e1ap_id, cu_up_e1ap_id));
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu), "Failed to receive Handover Request Ack");
    report_fatal_error_if_not(test_helpers::is_valid_handover_request_ack(ngap_pdu), "Invalid Handover Request Ack");
    return true;
  }

  [[nodiscard]] bool send_rrc_reconfiguration_complete_and_await_handover_notify()
  {
    // Inject UL RRC Message (containing RRC Reconfiguration Complete) and wait for Handover Notify
    get_du(du_idx).push_ul_pdu(generate_ul_rrc_message_transfer(
        cu_ue_id, du_ue_id, srb_id_t::srb1, make_byte_buffer("800008004e17dae3").value()));
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu), "Failed to receive Handover Notify");
    report_fatal_error_if_not(test_helpers::is_valid_handover_notify(ngap_pdu), "Invalid Handover Notify");
    return true;
  }

  [[nodiscard]] bool send_rrc_measurement_report_and_await_handover_required()
  {
    // Inject UL RRC Message (containing RRC Measurement Report) and wait for Handover Required
    get_du(du_idx).push_ul_pdu(
        generate_ul_rrc_message_transfer(ue_ctx->cu_ue_id.value(),
                                         ue_ctx->du_ue_id.value(),
                                         srb_id_t::srb1,
                                         make_byte_buffer("000800410004015f741fe0804bf183fcaa6e9699").value()));
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu), "Failed to receive Handover Required");
    report_fatal_error_if_not(test_helpers::is_valid_handover_required(ngap_pdu), "Invalid Handover Required");
    return true;
  }

  [[nodiscard]] bool send_handover_preparation_failure()
  {
    // Inject Handover Preparation Failure
    get_amf().push_tx_pdu(generate_handover_preparation_failure(ue_ctx->amf_ue_id.value(), ue_ctx->ran_ue_id.value()));
    return true;
  }

  [[nodiscard]] bool timeout_handover_command_and_await_handover_cancel()
  {
    // Fail Handover Preparation (AMF doesn't respond) and await Handover Cancel
    if (tick_until(std::chrono::milliseconds(1000), [&]() { return false; })) {
      return false;
    }
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu), "Failed to receive Handover Cancel");
    report_fatal_error_if_not(test_helpers::is_valid_handover_cancel(ngap_pdu), "Invalid Handover Cancel");
    return true;
  }

  [[nodiscard]] bool send_handover_cancel_ack()
  {
    // Inject Handover Cancel Ack
    get_amf().push_tx_pdu(generate_handover_cancel_ack(ue_ctx->amf_ue_id.value(), ue_ctx->ran_ue_id.value()));
    return true;
  }

  unsigned du_idx    = 0;
  unsigned cu_up_idx = 0;

  gnb_du_ue_f1ap_id_t du_ue_id = gnb_du_ue_f1ap_id_t::min;
  gnb_cu_ue_f1ap_id_t cu_ue_id;
  rnti_t              crnti     = to_rnti(0x4601);
  amf_ue_id_t         amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));
  gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id = gnb_cu_up_ue_e1ap_id_t::min;
  gnb_cu_cp_ue_e1ap_id_t cu_cp_e1ap_id;

  const ue_context* ue_ctx = nullptr;

  pdu_session_id_t psi = uint_to_pdu_session_id(1);
  qos_flow_id_t    qfi = uint_to_qos_flow_id(1);

  ngap_message ngap_pdu;
  f1ap_message f1ap_pdu;
  e1ap_message e1ap_pdu;
};

TEST_F(cu_cp_inter_cu_handover_test, when_handover_request_received_then_handover_notify_is_sent)
{
  // Inject Handover Request and await Bearer Context Setup Request
  ASSERT_TRUE(send_handover_request_and_await_bearer_context_setup_request());

  // Inject Bearer Context Setup Response and await UE Context Setup Request
  ASSERT_TRUE(send_bearer_context_setup_response_and_await_ue_context_setup_request());

  // Inject UE Context Setup Response and await Bearer Context Modification Request
  ASSERT_TRUE(send_ue_context_setup_response_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Response and await Handover Request Ack
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_handover_request_ack());

  // Inject RRC Reconfiguration Complete and await Handover Notify
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_handover_notify());
}

TEST_F(cu_cp_inter_cu_handover_test, when_handover_preparation_failure_is_received_then_handover_fails)
{
  // Attach UE
  ASSERT_TRUE(attach_ue());

  // Inject RRC Measurement Report and await Handover Required
  ASSERT_TRUE(send_rrc_measurement_report_and_await_handover_required());

  // Inject Handover Preparation Failure
  ASSERT_TRUE(send_handover_preparation_failure());

  // STATUS: Handover Preparation failed and no further messages are sent to the AMF
  report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                            "there are still NGAP messages to pop from AMF");
}

TEST_F(cu_cp_inter_cu_handover_test, when_handover_command_times_out_then_handover_cancel_is_sent)
{
  // Attach UE
  ASSERT_TRUE(attach_ue());

  // Inject RRC Measurement Report and await Handover Required
  ASSERT_TRUE(send_rrc_measurement_report_and_await_handover_required());

  // timeout Handover Command and await Handover Cancel
  ASSERT_TRUE(timeout_handover_command_and_await_handover_cancel());

  // Inject Handover Cancel Ack
  ASSERT_TRUE(send_handover_cancel_ack());
}
