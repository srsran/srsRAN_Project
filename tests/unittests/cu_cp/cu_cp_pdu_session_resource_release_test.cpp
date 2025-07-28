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
#include "tests/test_doubles/e1ap/e1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/ngap/ngap_test_message_validators.h"
#include "tests/test_doubles/rrc/rrc_test_message_validators.h"
#include "tests/unittests/cu_cp/test_helpers.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/ngap/ngap_message.h"
#include "srsran/ran/cu_types.h"
#include <gtest/gtest.h>
#include <optional>

using namespace srsran;
using namespace srs_cu_cp;

class cu_cp_pdu_session_resource_release_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_pdu_session_resource_release_test() : cu_cp_test_environment(cu_cp_test_env_params{})
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

  ngap_message ngap_pdu;
  f1ap_message f1ap_pdu;
  e1ap_message e1ap_pdu;

  [[nodiscard]] bool setup_second_pdu_session()
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject NGAP PDU Session Resource Setup Request and await Bearer Context Modification Request
    get_amf().push_tx_pdu(generate_valid_pdu_session_resource_setup_request_message(
        ue_ctx->amf_ue_id.value(),
        ue_ctx->ran_ue_id.value(),
        {{psi2, {pdu_session_type_t::ipv4, {{uint_to_qos_flow_id(2), 7}}}}}));
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
                              "Failed to receive Bearer Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_modification_request(e1ap_pdu),
                              "Invalid Bearer Context Modification Request");

    // Inject Bearer Context Modification Response and await UE Context Modification Request
    get_cu_up(cu_up_idx).push_tx_pdu(
        generate_bearer_context_modification_response(ue_ctx->cu_cp_e1ap_id.value(),
                                                      cu_up_e1ap_id,
                                                      {{psi2, drb_test_params{drb_id_t::drb2, uint_to_qos_flow_id(2)}}},
                                                      {}));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive UE Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_modification_request(f1ap_pdu),
                              "Invalid UE Context Modification Request");

    // Inject UE Context Modification Response and await Bearer Context Modification Request
    if (!cu_cp_test_environment::send_ue_context_modification_response_and_await_bearer_context_modification_request(
            du_idx, cu_up_idx, du_ue_id, crnti)) {
      return false;
    }

    // Inject Bearer Context Modification Response and await DL RRC Message Transfer containing RRC Reconfiguration
    get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_modification_response(
        ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value(), {}, {{psi2, drb_id_t::drb2}}));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive F1AP DL RRC Message (containing RRC Reconfiguration)");
    report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu),
                              "Invalid DL RRC Message Transfer");
    {
      const byte_buffer& rrc_container = test_helpers::get_rrc_container(f1ap_pdu);
      report_fatal_error_if_not(
          test_helpers::is_valid_rrc_reconfiguration(test_helpers::extract_dl_dcch_msg(rrc_container),
                                                     true,
                                                     std::vector<srb_id_t>{},
                                                     std::vector<drb_id_t>{drb_id_t::drb2}),
          "Invalid RRC Reconfiguration");
    }

    // Inject RRC Reconfiguration Complete and await successful PDU Session Resource Setup Response
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
        du_ue_id, ue_ctx->cu_ue_id.value(), srb_id_t::srb1, generate_rrc_reconfiguration_complete_pdu(0, 8)));
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive PDU Session Resource Setup Response");
    report_fatal_error_if_not(test_helpers::is_valid_pdu_session_resource_setup_response(ngap_pdu),
                              "Invalid PDU Session Resource Setup Response");
    report_fatal_error_if_not(test_helpers::is_expected_pdu_session_resource_setup_response(ngap_pdu, {psi2}, {}),
                              "Unsuccessful PDU Session Resource Setup Response");
    return true;
  }

  [[nodiscard]] bool send_pdu_session_release_command_and_await_bearer_context_modification_request()
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject Bearer Context Setup Response and wait for UE Context Modification Request
    get_amf().push_tx_pdu(
        generate_valid_pdu_session_resource_release_command(amf_ue_id, ue_ctx->ran_ue_id.value(), psi));
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
                              "Failed to receive Bearer Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_modification_request(e1ap_pdu),
                              "Invalid Bearer Context Modification Request");
    return true;
  }

  [[nodiscard]] bool send_pdu_session_release_command_and_await_bearer_context_release_command()
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject Bearer Context Setup Response and wait for UE Context Release Command
    get_amf().push_tx_pdu(
        generate_valid_pdu_session_resource_release_command(amf_ue_id, ue_ctx->ran_ue_id.value(), psi));
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
                              "Failed to receive Bearer Context Release Command");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_release_command(e1ap_pdu),
                              "Invalid Bearer Context Release Command");
    return true;
  }

  [[nodiscard]] bool send_bearer_context_modification_failure_and_await_ue_context_modification_request()
  {
    // Inject Bearer Context Modification Failure and wait for UE Context Modification Request
    get_cu_up(cu_up_idx).push_tx_pdu(
        generate_bearer_context_modification_failure(ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value()));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive UE Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_modification_request(f1ap_pdu),
                              "Invalid UE Context Modification Request");
    return true;
  }

  [[nodiscard]] bool send_bearer_context_modification_response_and_await_ue_context_modification_request()
  {
    // Inject Bearer Context Modification Response and wait for UE Context Modification Request
    get_cu_up(cu_up_idx).push_tx_pdu(
        generate_bearer_context_modification_response(ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value()));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive UE Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_modification_request(f1ap_pdu),
                              "Invalid UE Context Modification Request");
    return true;
  }

  [[nodiscard]] bool send_bearer_context_release_complete_and_await_ue_context_modification_request()
  {
    // Inject Bearer Context Release Complete and wait for UE Context Modification Request
    get_cu_up(cu_up_idx).push_tx_pdu(
        generate_bearer_context_release_complete(ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value()));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive UE Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_modification_request(f1ap_pdu),
                              "Invalid UE Context Modification Request");
    return true;
  }

  [[nodiscard]] bool send_ue_context_modification_response_and_await_rrc_reconfiguration()
  {
    // Inject UE Context Modification Response and wait for DL RRC Message (containing RRC Reconfiguration)
    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_ue_context_modification_response(du_ue_id, ue_ctx->cu_ue_id.value(), crnti));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive F1AP DL RRC Message (containing RRC Reconfiguration)");
    report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu),
                              "Invalid DL RRC Message Transfer");
    {
      const byte_buffer& rrc_container = test_helpers::get_rrc_container(f1ap_pdu);
      report_fatal_error_if_not(
          test_helpers::is_valid_rrc_reconfiguration(test_helpers::extract_dl_dcch_msg(rrc_container), {}, {}),
          "Invalid RRC Reconfiguration");
    }
    return true;
  }

  [[nodiscard]] bool send_ue_context_modification_failure_and_await_rrc_reconfiguration()
  {
    // Inject UE Context Modification Failure and wait for DL RRC Message (containing RRC Reconfiguration)
    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_ue_context_modification_failure(ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value()));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive F1AP DL RRC Message (containing RRC Reconfiguration)");
    report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu),
                              "Invalid DL RRC Message Transfer");
    {
      const byte_buffer& rrc_container = test_helpers::get_rrc_container(f1ap_pdu);
      report_fatal_error_if_not(
          test_helpers::is_valid_rrc_reconfiguration(test_helpers::extract_dl_dcch_msg(rrc_container), {}, {}),
          "Invalid RRC Reconfiguration");
    }
    return true;
  }

  [[nodiscard]] bool
  send_rrc_reconfiguration_complete_and_await_pdu_session_setup_response(unsigned rrc_recfg_transaction_id = 1,
                                                                         uint8_t  rrc_recfg_count          = 9)
  {
    // Inject UL RRC Message (containing RRC Reconfiguration Complete) and wait for PDU Session Resource Release
    // Response
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
        du_ue_id,
        ue_ctx->cu_ue_id.value(),
        srb_id_t::srb1,
        generate_rrc_reconfiguration_complete_pdu(rrc_recfg_transaction_id, rrc_recfg_count)));
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds(10000)),
                              "Failed to receive PDU Session Resource Release Response");
    report_fatal_error_if_not(test_helpers::is_valid_pdu_session_resource_release_response(ngap_pdu),
                              "Invalid PDU Session Resource Setup Response");
    return true;
  }
};

TEST_F(cu_cp_pdu_session_resource_release_test, when_bearer_context_modification_failure_received_then_release_succeeds)
{
  // Add second PDU session
  ASSERT_TRUE(setup_second_pdu_session());

  // Inject NGAP PDU Session Resource Release Command and await Bearer Context Modification Request
  ASSERT_TRUE(send_pdu_session_release_command_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Failure and await UE Context Modification Request
  ASSERT_TRUE(send_bearer_context_modification_failure_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response and await RRC Reconfiguration
  ASSERT_TRUE(send_ue_context_modification_response_and_await_rrc_reconfiguration());

  // Inject RRC Reconfiguration Complete and await PDU Session Resource Release Response
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_pdu_session_setup_response());
}

TEST_F(cu_cp_pdu_session_resource_release_test, when_ue_context_modification_failure_received_then_release_succeeds)
{
  // Add second PDU session
  ASSERT_TRUE(setup_second_pdu_session());

  // Inject NGAP PDU Session Resource Release Command and await Bearer Context Modification Request
  ASSERT_TRUE(send_pdu_session_release_command_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Response and await UE Context Modification Request
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Failure and await RRC Reconfiguration
  ASSERT_TRUE(send_ue_context_modification_failure_and_await_rrc_reconfiguration());

  // Inject RRC Reconfiguration Complete and await PDU Session Resource Release Response
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_pdu_session_setup_response());
}

TEST_F(cu_cp_pdu_session_resource_release_test, when_all_sub_actions_succeed_then_release_succeeds)
{
  // Add second PDU session
  ASSERT_TRUE(setup_second_pdu_session());

  // Inject NGAP PDU Session Resource Release Command and await Bearer Context Modification Request
  ASSERT_TRUE(send_pdu_session_release_command_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Response and await UE Context Modification Request
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response and await RRC Reconfiguration
  ASSERT_TRUE(send_ue_context_modification_response_and_await_rrc_reconfiguration());

  // Inject RRC Reconfiguration Complete and await PDU Session Resource Release Response
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_pdu_session_setup_response());
}

TEST_F(cu_cp_pdu_session_resource_release_test, when_only_pdu_session_released_then_bearer_context_release_command_sent)
{
  // Inject NGAP PDU Session Resource Release Command and await Bearer Context Release Command
  ASSERT_TRUE(send_pdu_session_release_command_and_await_bearer_context_release_command());

  // Inject Bearer Context Release Complete and await UE Context Modification Request
  ASSERT_TRUE(send_bearer_context_release_complete_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response and await RRC Reconfiguration
  ASSERT_TRUE(send_ue_context_modification_response_and_await_rrc_reconfiguration());

  // Inject RRC Reconfiguration Complete and await PDU Session Resource Release Response
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_pdu_session_setup_response(0, 8));
}
