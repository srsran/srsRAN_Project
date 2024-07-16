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
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/ngap/ngap_test_message_validators.h"
#include "tests/test_doubles/rrc/rrc_test_message_validators.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "tests/unittests/f1ap/common/f1ap_cu_test_messages.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/ngap/ngap_message.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class cu_cp_initial_context_setup_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_initial_context_setup_test() : cu_cp_test_environment(cu_cp_test_env_params{8, 8, 8192, create_mock_amf()})
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
    EXPECT_TRUE(connect_new_ue(du_idx, du_ue_id, crnti));
    EXPECT_TRUE(authenticate_ue(du_idx, du_ue_id, amf_ue_id_t::min));

    ue_ctx = this->find_ue_context(du_idx, du_ue_id);

    EXPECT_NE(ue_ctx, nullptr);
  }

  void send_initial_context_setup_request(bool with_pdu_sessions = false)
  {
    srsran_assert(not this->get_amf().try_pop_rx_pdu(ngap_pdu), "there are still NGAP messages to pop from AMF");
    srsran_assert(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu), "there are still F1AP DL messages to pop from DU");

    // Inject NGAP Initial Context Setup Request
    ngap_message init_ctxt_setup_req;
    if (with_pdu_sessions) {
      init_ctxt_setup_req = generate_valid_initial_context_setup_request_message_with_pdu_session(
          ue_ctx->amf_ue_id.value(), ue_ctx->ran_ue_id.value());
    } else {
      init_ctxt_setup_req =
          generate_valid_initial_context_setup_request_message(ue_ctx->amf_ue_id.value(), ue_ctx->ran_ue_id.value());
    }

    get_amf().push_tx_pdu(init_ctxt_setup_req);
  }

  void send_ue_context_setup_request_and_await_response()
  {
    // Wait for F1AP UE Context Setup Request (containing Security Mode Command).
    bool result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
    report_fatal_error_if_not(result, "Failed to receive Security Mode Command");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_setup_request(f1ap_pdu),
                              "Invalid UE Context Setup Request");
    const byte_buffer& rrc_container = test_helpers::get_rrc_container(f1ap_pdu);
    report_fatal_error_if_not(
        test_helpers::is_valid_rrc_security_mode_command(test_helpers::extract_dl_dcch_msg(rrc_container)),
        "Invalid Security Mode Command");

    // Inject UE Context Setup Response
    f1ap_message ue_ctxt_setup_response = generate_ue_context_setup_response(ue_ctx->cu_ue_id.value(), du_ue_id);
    get_du(du_idx).push_ul_pdu(ue_ctxt_setup_response);
  }

  void send_security_mode_complete_and_await_ue_capability_enquiry()
  {
    // Inject Security Mode Complete
    f1ap_message ul_rrc_msg_transfer = generate_ul_rrc_message_transfer(
        ue_ctx->cu_ue_id.value(), du_ue_id, srb_id_t::srb1, make_byte_buffer("00032a00fd5ec7ff").value());
    get_du(du_idx).push_ul_pdu(ul_rrc_msg_transfer);

    // Wait for UE Capability Enquiry
    bool result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
    report_fatal_error_if_not(result, "Failed to receive UE Capability Enquiry");
    report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu),
                              "Invalid DL RRC Message Transfer");
    const byte_buffer& rrc_container = test_helpers::get_rrc_container(f1ap_pdu);
    report_fatal_error_if_not(
        test_helpers::is_valid_rrc_ue_capability_enquiry(test_helpers::extract_dl_dcch_msg(rrc_container)),
        "Invalid UE Capability Enquiry");
  }

  void send_ue_capability_info_and_await_registration_accept_and_initial_context_setup_response()
  {
    // Inject UL RRC Message Transfer (containing UE Capability Info)
    get_du(du_idx).push_ul_pdu(test_helpers::create_ul_rrc_message_transfer(
        du_ue_id,
        ue_ctx->cu_ue_id.value(),
        srb_id_t::srb1,
        make_byte_buffer(
            "00044c821930680ce811d1968097e360e1480005824c5c00060fc2c00637fe002e00131401a0000000880058d006007"
            "a071e439f0000240400e0300000000100186c0000700809df000000000000030368000800004b2ca000a07143c001c0"
            "03c000000100200409028098a8660c")
            .value()));

    // Wait for DL RRC Message Transfer (containing NAS Registration Accept)
    bool result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
    report_fatal_error_if_not(result, "Failed to receive DL RRC Message, containing NAS Registration Accept");
    report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu),
                              "Invalid DL RRC Message Transfer");

    // Wait for Initial Context Setup Response
    result = this->wait_for_ngap_tx_pdu(ngap_pdu);
    report_fatal_error_if_not(result, "Failed to receive Initial Context Setup Response");
    report_fatal_error_if_not(test_helpers::is_valid_initial_context_setup_response(ngap_pdu),
                              "Invalid init ctxt setup");
  }

  void send_ue_capability_info_and_handle_pdu_session_resource_setup_request()
  {
    // Inject UL RRC Message Transfer (containing UE Capability Info)
    get_du(du_idx).push_ul_pdu(test_helpers::create_ul_rrc_message_transfer(
        du_ue_id,
        ue_ctx->cu_ue_id.value(),
        srb_id_t::srb1,
        make_byte_buffer(
            "00044c821930680ce811d1968097e360e1480005824c5c00060fc2c00637fe002e00131401a0000000880058d006007"
            "a071e439f0000240400e0300000000100186c0000700809df000000000000030368000800004b2ca000a07143c001c0"
            "03c000000100200409028098a8660c")
            .value()));

    // Wait for E1AP Bearer Context Setup Request
    bool result = this->wait_for_e1ap_tx_pdu(0, e1ap_pdu);
    report_fatal_error_if_not(result, "Failed to receive E1AP Bearer Context Setup Request");

    cu_cp_e1ap_id =
        int_to_gnb_cu_cp_ue_e1ap_id(e1ap_pdu.pdu.init_msg().value.bearer_context_setup_request()->gnb_cu_cp_ue_e1ap_id);
    cu_up_e1ap_id = int_to_gnb_cu_up_ue_e1ap_id(0);

    // Inject Bearer Context Setup Response and wait for F1AP UE Context Modification Request.
    get_cu_up(0).push_tx_pdu(generate_bearer_context_setup_response(cu_cp_e1ap_id, cu_up_e1ap_id));
    result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
    report_fatal_error_if_not(result, "Failed to receive F1AP UE Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_modification_request(f1ap_pdu),
                              "Invalid UE Context Modification");

    // Inject UE Context Modification Response and wait for Bearer Context Modification to be sent to CU-UP.
    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_ue_context_modification_response(du_ue_id, ue_ctx->cu_ue_id.value(), crnti));
    result = this->wait_for_e1ap_tx_pdu(0, e1ap_pdu);
    report_fatal_error_if_not(result, "Failed to receive E1AP Bearer Context Modification");

    // Inject E1AP Bearer Context Modification Response and wait for DL RRC Message (containing RRC Reconfiguration)
    get_cu_up(0).push_tx_pdu(generate_bearer_context_modification_response(cu_cp_e1ap_id, cu_up_e1ap_id));
    result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
    report_fatal_error_if_not(result, "Failed to receive F1AP DL RRC Message (containing RRC Reconfiguration)");
    report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu),
                              "Invalid DL RRC Message Transfer");

    // Make sure RRC Reconfiguration contains NAS PDU
    const byte_buffer& rrc_container = test_helpers::get_rrc_container(f1ap_pdu);
    report_fatal_error_if_not(
        test_helpers::is_valid_rrc_reconfiguration(test_helpers::extract_dl_dcch_msg(rrc_container)),
        "Invalid RRC Reconfiguration");
  }

  void send_rrc_reconfiguration_complete_and_await_initial_context_setup_response()
  {
    // Inject UL RRC Message Transfer (containing RRC Reconfiguration Complete)
    f1ap_message ul_rrc_msg_transfer = generate_ul_rrc_message_transfer(
        ue_ctx->cu_ue_id.value(), du_ue_id, srb_id_t::srb1, make_byte_buffer("00050e00a18bc2b3").value());
    get_du(du_idx).push_ul_pdu(ul_rrc_msg_transfer);

    // Wait for Initial Context Setup Response
    bool result = this->wait_for_ngap_tx_pdu(ngap_pdu);
    report_fatal_error_if_not(result, "Failed to receive Initial Context Setup Response");
    report_fatal_error_if_not(test_helpers::is_valid_initial_context_setup_response(ngap_pdu),
                              "Invalid init ctxt setup");
  }

  void await_initial_context_setup_failure()
  {
    // Wait for NGAP Initial Context Setup Failure
    bool result = this->wait_for_ngap_tx_pdu(ngap_pdu);
    report_fatal_error_if_not(result, "Failed to receive Initial Context Setup Failure");
    report_fatal_error_if_not(test_helpers::is_valid_initial_context_setup_failure(ngap_pdu),
                              "Invalid Initial Context Setup Failure");
  }

  void await_ue_capability_info_indication()
  {
    // Wait for UE Capability Info Indication
    bool result = this->wait_for_ngap_tx_pdu(ngap_pdu);
    report_fatal_error_if_not(result, "Failed to receive UE Radio Capability Info Indication");
    report_fatal_error_if_not(test_helpers::is_valid_ue_radio_capability_info_indication(ngap_pdu),
                              "Invalid UE Radio Capability Info Indication");
  }

  void await_ue_context_release_request()
  {
    // Wait for UE Context Release Request
    bool result = this->wait_for_ngap_tx_pdu(ngap_pdu);
    report_fatal_error_if_not(result, "Failed to receive UE Context Release Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_request(ngap_pdu),
                              "Invalid UE Context Release Request");
  }

  unsigned du_idx    = 0;
  unsigned cu_up_idx = 0;

  gnb_du_ue_f1ap_id_t    du_ue_id = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t                 crnti    = to_rnti(0x4601);
  gnb_cu_cp_ue_e1ap_id_t cu_cp_e1ap_id;
  gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id;

  const ue_context* ue_ctx = nullptr;

  ngap_message ngap_pdu;
  f1ap_message f1ap_pdu;
  e1ap_message e1ap_pdu;
};

TEST_F(cu_cp_initial_context_setup_test, when_ue_context_setup_fails_then_initial_context_setup_fails)
{
  // Inject Initial Context Setup Request
  send_initial_context_setup_request();

  // Inject UE Context Setup Failure
  f1ap_message ue_ctxt_setup_failure =
      generate_ue_context_setup_failure(find_ue_context(du_idx, du_ue_id)->cu_ue_id.value(), du_ue_id);
  get_du(du_idx).push_ul_pdu(ue_ctxt_setup_failure);

  // Wait for NGAP Initial Context Setup Failure
  await_initial_context_setup_failure();
}

TEST_F(cu_cp_initial_context_setup_test, when_security_mode_command_fails_then_initial_context_setup_fails)
{
  // Inject Initial Context Setup Request
  send_initial_context_setup_request();

  // Wait for F1AP UE Context Setup Request (containing Security Mode Command) and inject UE Context Setup Response
  send_ue_context_setup_request_and_await_response();

  // Inject Security Mode Failure
  f1ap_message ul_rrc_msg_transfer = generate_ul_rrc_message_transfer(
      ue_ctx->cu_ue_id.value(), du_ue_id, srb_id_t::srb1, make_byte_buffer("00033200c1bf019d").value());
  get_du(du_idx).push_ul_pdu(ul_rrc_msg_transfer);

  // Wait for NGAP Initial Context Setup Failure
  await_initial_context_setup_failure();
}

TEST_F(cu_cp_initial_context_setup_test, when_ue_capability_enquiry_fails_then_initial_context_setup_fails)
{
  // Inject Initial Context Setup Request
  send_initial_context_setup_request();

  // Wait for F1AP UE Context Setup Request (containing Security Mode Command) and inject UE Context Setup Response
  send_ue_context_setup_request_and_await_response();

  // Inject Security Mode Complete and await UE Capability Enquiry
  send_security_mode_complete_and_await_ue_capability_enquiry();

  // Fail UE Capability Enquiry (UE doesn't respond)
  ASSERT_FALSE(tick_until(std::chrono::milliseconds(this->get_cu_cp_cfg().rrc.rrc_procedure_timeout_ms),
                          [&]() { return false; }));

  // Wait for NGAP Initial Context Setup Failure
  await_initial_context_setup_failure();
}

TEST_F(cu_cp_initial_context_setup_test, when_ue_capability_enquiry_successful_then_initial_context_setup_succeeds)
{
  // Inject Initial Context Setup Request
  send_initial_context_setup_request();

  // Wait for F1AP UE Context Setup Request (containing Security Mode Command) and inject UE Context Setup Response
  send_ue_context_setup_request_and_await_response();

  // Inject Security Mode Complete and await UE Capability Enquiry
  send_security_mode_complete_and_await_ue_capability_enquiry();

  // Inject UE Capability Info and await DL RRC Message (Registration Accept) and Initial Context Setup Response
  send_ue_capability_info_and_await_registration_accept_and_initial_context_setup_response();

  // Wait for UE Capability Info Indication
  await_ue_capability_info_indication();
}

TEST_F(cu_cp_initial_context_setup_test,
       when_initial_context_setup_contains_valid_pdu_sessions_to_setup_then_initial_context_setup_succeeds)
{
  // Inject Initial Context Setup Request
  send_initial_context_setup_request(true);

  // Wait for F1AP UE Context Setup Request (containing Security Mode Command) and inject UE Context Setup Response
  send_ue_context_setup_request_and_await_response();

  // Inject Security Mode Complete and await UE Capability Enquiry
  send_security_mode_complete_and_await_ue_capability_enquiry();

  // Inject UE Capability Info and handle PDU Session Resource Setup List Context Request
  send_ue_capability_info_and_handle_pdu_session_resource_setup_request();

  // Inject RRC Reconfiguration Complete and await Initial Context Setup Response
  send_rrc_reconfiguration_complete_and_await_initial_context_setup_response();

  // Wait for UE Capability Info Indication
  await_ue_capability_info_indication();
}