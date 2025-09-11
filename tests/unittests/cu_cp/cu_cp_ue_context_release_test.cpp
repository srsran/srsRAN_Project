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
#include "tests/test_doubles/ngap/ngap_test_message_validators.h"
#include "tests/unittests/cu_cp/test_helpers.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/f1ap/f1ap_ue_id_types.h"
#include "srsran/ngap/ngap_message.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class cu_cp_ue_context_release_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_ue_context_release_test() : cu_cp_test_environment(cu_cp_test_env_params{})
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
    // Connect UE 0x4601.
    if (!connect_new_ue(du_idx, du_ue_id, crnti)) {
      return false;
    }
    if (!authenticate_ue(du_idx, du_ue_id, amf_ue_id)) {
      return false;
    }
    if (!setup_ue_security(du_idx, du_ue_id)) {
      return false;
    }
    ue_ctx = this->find_ue_context(du_idx, du_ue_id);
    if (!finish_ue_registration(du_idx, cu_up_idx, du_ue_id)) {
      return false;
    }
    return ue_ctx != nullptr;
  }

  [[nodiscard]] bool setup_ue_pdu_session()
  {
    if (!attach_ue()) {
      return false;
    }
    if (!request_pdu_session_resource_setup(du_idx, cu_up_idx, du_ue_id)) {
      return false;
    }
    if (!setup_pdu_session(du_idx, cu_up_idx, du_ue_id, crnti, cu_up_e1ap_id)) {
      return false;
    }
    return true;
  }

  [[nodiscard]] bool send_ngap_ue_context_release_command_and_await_f1ap_ue_context_release_command()
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject NGAP UE Context Release Command and wait for F1AP UE Context Release Command
    get_amf().push_tx_pdu(generate_valid_ue_context_release_command_with_amf_ue_ngap_id(ue_ctx->amf_ue_id.value()));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive UE Context Release Command");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_command(f1ap_pdu),
                              "Invalid UE Context Release Command");
    return true;
  }

  [[nodiscard]] bool send_ngap_ue_context_release_command_and_await_bearer_context_release_command()
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject NGAP UE Context Release Command and wait for Bearer Context Release Command
    get_amf().push_tx_pdu(generate_valid_ue_context_release_command_with_amf_ue_ngap_id(ue_ctx->amf_ue_id.value()));
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
                              "Failed to receive Bearer Context Release Command");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_release_command(e1ap_pdu),
                              "Invalid Bearer Context Release Command");
    return true;
  }

  [[nodiscard]] bool send_bearer_context_release_complete_and_await_f1ap_ue_context_release_command()
  {
    // Inject Bearer Context Release Complete and wait for F1AP UE Context Release Command
    get_cu_up(cu_up_idx).push_tx_pdu(
        generate_bearer_context_release_complete(ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value()));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive UE Context Release Command");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_command(f1ap_pdu),
                              "Invalid UE Context Release Command");
    return true;
  }

  [[nodiscard]] bool send_f1ap_ue_context_release_complete(gnb_cu_ue_f1ap_id_t cu_ue_id_, gnb_du_ue_f1ap_id_t du_ue_id_)
  {
    // Inject F1AP UE Context Release Complete
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_release_complete(cu_ue_id_, du_ue_id_));
    return true;
  }

  [[nodiscard]] bool send_f1ap_ue_context_release_complete_and_await_ngap_ue_context_release_complete()
  {
    // Inject F1AP UE Context Release Complete and wait for N1AP UE Context Release Command
    if (!send_f1ap_ue_context_release_complete(ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value())) {
      return false;
    }
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu), "Failed to receive UE Context Release Complete");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_complete(ngap_pdu),
                              "Invalid UE Context Release Complete");
    return true;
  }

  [[nodiscard]] bool send_pdu_session_resource_setup_request()
  {
    // Inject PDU Session Resource Setup Request
    get_amf().push_tx_pdu(generate_valid_pdu_session_resource_setup_request_message(
        ue_ctx->amf_ue_id.value(),
        ue_ctx->ran_ue_id.value(),
        {{uint_to_pdu_session_id(1), {pdu_session_type_t::ipv4, {{uint_to_qos_flow_id(1), 9}}}}}));
    return true;
  }

  [[nodiscard]] bool await_error_indication()
  {
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu), "Failed to receive Error Indication");
    report_fatal_error_if_not(test_helpers::is_valid_error_indication(ngap_pdu), "Invalid Error Indication");
    return true;
  }

  [[nodiscard]] bool send_f1ap_ue_context_release_request(gnb_cu_ue_f1ap_id_t cu_ue_id_, gnb_du_ue_f1ap_id_t du_ue_id_)
  {
    // Inject F1AP UE Context Release Request
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_release_request(cu_ue_id_, du_ue_id_));
    return true;
  }

  [[nodiscard]] bool send_f1ap_ue_context_release_request_and_await_ngap_ue_context_release_request()
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject F1AP UE Context Release Request and wait for NGAP UE Context Release Request
    if (!send_f1ap_ue_context_release_request(ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value())) {
      return false;
    }
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive NGAP UE Context Release Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_request(ngap_pdu),
                              "Invalid NGAP UE Context Release Request");
    return true;
  }

  [[nodiscard]] bool send_e1ap_bearer_context_release_request(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_id_,
                                                              gnb_cu_up_ue_e1ap_id_t cu_up_ue_id_)
  {
    // Inject E1AP Bearer Context Release Request
    get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_release_request(cu_cp_ue_id_, cu_up_ue_id_));
    return true;
  }

  [[nodiscard]] bool send_e1ap_bearer_context_release_request_and_await_ngap_ue_context_release_request()
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject E1AP Bearer Context Release Request and wait for NGAP UE Context Release Request
    if (!send_e1ap_bearer_context_release_request(ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value())) {
      return false;
    }
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive NGAP UE Context Release Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_request(ngap_pdu),
                              "Invalid NGAP UE Context Release Request");
    return true;
  }

  unsigned du_idx    = 0;
  unsigned cu_up_idx = 0;

  gnb_du_ue_f1ap_id_t    du_ue_id      = gnb_du_ue_f1ap_id_t::min;
  rnti_t                 crnti         = to_rnti(0x4601);
  amf_ue_id_t            amf_ue_id     = amf_ue_id_t::min;
  gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id = gnb_cu_up_ue_e1ap_id_t::min;

  const ue_context* ue_ctx = nullptr;

  ngap_message ngap_pdu;
  f1ap_message f1ap_pdu;
  e1ap_message e1ap_pdu;
};

TEST_F(cu_cp_ue_context_release_test,
       when_ue_context_release_command_but_no_pdu_session_setup_received_then_release_succeeds)
{
  // Attach UE
  ASSERT_TRUE(attach_ue());

  // Inject NGAP UE Context Release Command and await F1AP UE Context Release Command
  ASSERT_TRUE(send_ngap_ue_context_release_command_and_await_f1ap_ue_context_release_command());

  // Inject F1AP UE Context Release Complete and await NGAP UE Context Release Complete
  ASSERT_TRUE(send_f1ap_ue_context_release_complete_and_await_ngap_ue_context_release_complete());

  // STATUS: UE should be removed at this stage
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 0) << "UE should be removed";
}

TEST_F(cu_cp_ue_context_release_test,
       when_pdu_session_resource_setup_request_is_received_during_release_then_error_indication_is_sent)
{
  // Setup PDU Session
  ASSERT_TRUE(setup_ue_pdu_session());

  // Inject NGAP UE Context Release Command and await Bearer Context Release Command
  ASSERT_TRUE(send_ngap_ue_context_release_command_and_await_bearer_context_release_command());

  // Inject Bearer Context Release Complete and await F1AP UE Context Release Command
  ASSERT_TRUE(send_bearer_context_release_complete_and_await_f1ap_ue_context_release_command());

  // Inject PDU Session Resource Setup Request
  ASSERT_TRUE(send_pdu_session_resource_setup_request());

  // Inject F1AP UE Context Release Complete and await NGAP UE Context Release Complete
  ASSERT_TRUE(send_f1ap_ue_context_release_complete_and_await_ngap_ue_context_release_complete());

  // STATUS: UE should be removed at this stage
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 0) << "UE should be removed";

  // STATUS: An error indication should be sent to the AMF
  ASSERT_TRUE(await_error_indication());
}

TEST_F(cu_cp_ue_context_release_test, when_ue_context_release_command_received_then_release_succeeds)
{
  // Setup PDU Session
  ASSERT_TRUE(setup_ue_pdu_session());

  // Inject NGAP UE Context Release Command and await Bearer Context Release Command
  ASSERT_TRUE(send_ngap_ue_context_release_command_and_await_bearer_context_release_command());

  // Inject Bearer Context Release Complete and await F1AP UE Context Release Command
  ASSERT_TRUE(send_bearer_context_release_complete_and_await_f1ap_ue_context_release_command());

  // Inject F1AP UE Context Release Complete and await NGAP UE Context Release Complete
  ASSERT_TRUE(send_f1ap_ue_context_release_complete_and_await_ngap_ue_context_release_complete());

  // STATUS: UE should be removed at this stage
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 0) << "UE should be removed";
}

TEST_F(cu_cp_ue_context_release_test,
       when_du_initiated_ue_context_release_received_then_ue_context_release_request_is_sent)
{
  // Setup PDU Session
  ASSERT_TRUE(setup_ue_pdu_session());

  // Inject F1AP UE Context Release Request and await NGAP UE Context Release Request
  ASSERT_TRUE(send_f1ap_ue_context_release_request_and_await_ngap_ue_context_release_request());
}

TEST_F(
    cu_cp_ue_context_release_test,
    when_du_initiated_ue_context_release_received_and_ue_unknown_to_amf_then_ue_is_released_without_release_request_to_amf)
{
  // RRC connect UE without sending message to AMF
  gnb_cu_ue_f1ap_id_t cu_ue_id = gnb_cu_ue_f1ap_id_t::min;
  {
    srsran_assert(not this->get_amf().try_pop_rx_pdu(ngap_pdu), "there are still NGAP messages to pop from AMF");
    srsran_assert(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu), "there are still F1AP DL messages to pop from DU");

    // Inject Initial UL RRC message
    f1ap_message init_ul_rrc_msg = test_helpers::generate_init_ul_rrc_message_transfer(du_ue_id, crnti);
    test_logger.info("c-rnti={} du_ue={}: Injecting Initial UL RRC message", crnti, fmt::underlying(du_ue_id));
    get_du(du_idx).push_ul_pdu(init_ul_rrc_msg);

    // Wait for DL RRC message transfer (containing RRC Setup)
    report_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}),
                        "Failed to receive DL RRC message");

    // Check if the DL RRC Message with Msg4 is valid.
    report_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer_with_msg4(f1ap_pdu), "invalid DL RRC message");

    // Check if the UE Id matches.
    auto& dl_rrc_msg = *f1ap_pdu.pdu.init_msg().value.dl_rrc_msg_transfer();
    report_error_if_not(int_to_gnb_du_ue_f1ap_id(dl_rrc_msg.gnb_du_ue_f1ap_id) == du_ue_id,
                        "invalid gNB-DU-UE-F1AP-ID");
    report_error_if_not(int_to_srb_id(dl_rrc_msg.srb_id) == srb_id_t::srb0, "invalid SRB-Id");

    // Send RRC Setup Complete.
    // > Generate UL DCCH message (containing RRC Setup Complete).
    f1ap_message ul_dcch_msg = test_helpers::generate_ul_rrc_message_transfer(
        du_ue_id, cu_ue_id, srb_id_t::srb1, generate_rrc_setup_complete());
    // > Generate UL RRC Message (containing RRC Setup Complete) with PDCP SN=0.
    get_du(du_idx).push_ul_pdu(ul_dcch_msg);
  }

  // Inject F1AP UE Context Release Request
  ASSERT_TRUE(send_f1ap_ue_context_release_request(cu_ue_id, du_ue_id));

  // No NGAP UE Context Release Request should be sent to the AMF
  ASSERT_NE(ngap_pdu.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);

  // Inject F1AP UE Context Release Complete
  ASSERT_TRUE(send_f1ap_ue_context_release_complete(cu_ue_id, du_ue_id));

  // STATUS: UE should be removed at this stage
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 0) << "UE should be removed";
}

TEST_F(cu_cp_ue_context_release_test,
       when_cu_up_initiated_bearer_context_release_received_then_ue_context_release_request_is_sent)
{
  // Setup PDU Session
  ASSERT_TRUE(setup_ue_pdu_session());

  // Inject E1AP Bearer Context Release Request and await NGAP UE Context Release Request
  ASSERT_TRUE(send_e1ap_bearer_context_release_request_and_await_ngap_ue_context_release_request());
}
