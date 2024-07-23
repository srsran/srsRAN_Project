/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class cu_cp_ue_context_release_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_ue_context_release_test() : cu_cp_test_environment(cu_cp_test_env_params{8, 8, 8192, create_mock_amf()})
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

  void attach_ue()
  {
    // Connect UE 0x4601.
    EXPECT_TRUE(connect_new_ue(du_idx, du_ue_id, crnti));
    EXPECT_TRUE(authenticate_ue(du_idx, du_ue_id, amf_ue_id));
    EXPECT_TRUE(setup_ue_security(du_idx, du_ue_id));
    ue_ctx = this->find_ue_context(du_idx, du_ue_id);

    EXPECT_TRUE(finish_ue_registration(du_idx, cu_up_idx, du_ue_id));

    EXPECT_NE(ue_ctx, nullptr);
  }

  void setup_ue_pdu_session()
  {
    attach_ue();
    EXPECT_TRUE(request_pdu_session_resource_setup(du_idx, cu_up_idx, du_ue_id));
    EXPECT_TRUE(setup_pdu_session(du_idx, cu_up_idx, du_ue_id, crnti, cu_up_e1ap_id));
  }

  void send_ngap_ue_context_release_command_and_await_f1ap_ue_context_release_command()
  {
    srsran_assert(not this->get_amf().try_pop_rx_pdu(ngap_pdu), "there are still NGAP messages to pop from AMF");
    srsran_assert(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu), "there are still F1AP DL messages to pop from DU");
    srsran_assert(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                  "there are still E1AP messages to pop from CU-UP");

    // Inject NGAP UE Context Release Command and wait for F1AP UE Context Release Command
    get_amf().push_tx_pdu(generate_valid_ue_context_release_command_with_amf_ue_ngap_id(ue_ctx->amf_ue_id.value()));
    bool result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
    report_fatal_error_if_not(result, "Failed to receive UE Context Release Command");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_command(f1ap_pdu),
                              "Invalid UE Context Release Command");
  }

  void send_ngap_ue_context_release_command_and_await_bearer_context_release_command()
  {
    srsran_assert(not this->get_amf().try_pop_rx_pdu(ngap_pdu), "there are still NGAP messages to pop from AMF");
    srsran_assert(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu), "there are still F1AP DL messages to pop from DU");
    srsran_assert(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                  "there are still E1AP messages to pop from CU-UP");

    // Inject NGAP UE Context Release Command and wait for Bearer Context Release Command
    get_amf().push_tx_pdu(generate_valid_ue_context_release_command_with_amf_ue_ngap_id(ue_ctx->amf_ue_id.value()));
    bool result = this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu);
    report_fatal_error_if_not(result, "Failed to receive Bearer Context Release Command");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_release_command(e1ap_pdu),
                              "Invalid Bearer Context Release Command");
  }

  void send_bearer_context_release_complete_and_await_f1ap_ue_context_release_command()
  {
    // Inject Bearer Context Release Complete and wait for F1AP UE Context Release Command
    get_cu_up(cu_up_idx).push_tx_pdu(
        generate_bearer_context_release_complete(ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value()));
    bool result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
    report_fatal_error_if_not(result, "Failed to receive UE Context Release Command");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_command(f1ap_pdu),
                              "Invalid UE Context Release Command");
  }

  void send_f1ap_ue_context_release_complete(gnb_cu_ue_f1ap_id_t cu_ue_id_, gnb_du_ue_f1ap_id_t du_ue_id_)
  {
    // Inject F1AP UE Context Release Complete
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_release_complete(cu_ue_id_, du_ue_id_));
  }

  void send_f1ap_ue_context_release_complete_and_await_ngap_ue_context_release_complete()
  {
    // Inject F1AP UE Context Release Complete and wait for N1AP UE Context Release Command
    send_f1ap_ue_context_release_complete(ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value());
    bool result = this->wait_for_ngap_tx_pdu(ngap_pdu);
    report_fatal_error_if_not(result, "Failed to receive UE Context Release Complete");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_complete(ngap_pdu),
                              "Invalid UE Context Release Complete");
  }

  void send_pdu_session_resource_setup_request()
  {
    // Inject PDU Session Resource Setup Request
    get_amf().push_tx_pdu(generate_valid_pdu_session_resource_setup_request_message(
        ue_ctx->amf_ue_id.value(),
        ue_ctx->ran_ue_id.value(),
        {{uint_to_pdu_session_id(1), {{uint_to_qos_flow_id(1), 9}}}}));
  }

  void await_error_indication()
  {
    bool result = this->wait_for_ngap_tx_pdu(ngap_pdu);
    report_fatal_error_if_not(result, "Failed to receive Error Indication");
    report_fatal_error_if_not(test_helpers::is_valid_error_indication(ngap_pdu), "Invalid Error Indication");
  }

  void send_f1ap_ue_context_release_request(gnb_cu_ue_f1ap_id_t cu_ue_id_, gnb_du_ue_f1ap_id_t du_ue_id_)
  {
    // Inject F1AP UE Context Release Request
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_release_request(cu_ue_id_, du_ue_id_));
  }

  void send_f1ap_ue_context_release_request_and_await_ngap_ue_context_release_request()
  {
    srsran_assert(not this->get_amf().try_pop_rx_pdu(ngap_pdu), "there are still NGAP messages to pop from AMF");
    srsran_assert(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu), "there are still F1AP DL messages to pop from DU");
    srsran_assert(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                  "there are still E1AP messages to pop from CU-UP");

    // Inject F1AP UE Context Release Request and wait for NGAP UE Context Release Request
    send_f1ap_ue_context_release_request(ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value());
    bool result = this->wait_for_ngap_tx_pdu(ngap_pdu);
    report_fatal_error_if_not(result, "Failed to receive NGAP UE Context Release Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_request(ngap_pdu),
                              "Invalid NGAP UE Context Release Request");
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
  attach_ue();

  // Inject NGAP UE Context Release Command and await F1AP UE Context Release Command
  send_ngap_ue_context_release_command_and_await_f1ap_ue_context_release_command();

  // Inject F1AP UE Context Release Complete and await NGAP UE Context Release Complete
  send_f1ap_ue_context_release_complete_and_await_ngap_ue_context_release_complete();

  // STATUS: UE should be removed at this stage
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 0) << "UE should be removed";
}

TEST_F(cu_cp_ue_context_release_test,
       when_pdu_session_resource_setup_request_is_received_during_release_then_error_indication_is_sent)
{
  // Setup PDU Session
  setup_ue_pdu_session();

  // Inject NGAP UE Context Release Command and await Bearer Context Release Command
  send_ngap_ue_context_release_command_and_await_bearer_context_release_command();

  // Inject Bearer Context Release Complete and await F1AP UE Context Release Command
  send_bearer_context_release_complete_and_await_f1ap_ue_context_release_command();

  // Inject PDU Session Resource Setup Request
  send_pdu_session_resource_setup_request();

  // Inject F1AP UE Context Release Complete and await NGAP UE Context Release Complete
  send_f1ap_ue_context_release_complete_and_await_ngap_ue_context_release_complete();

  // STATUS: UE should be removed at this stage
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 0) << "UE should be removed";

  // STATUS: An error indication should be sent to the AMF
  await_error_indication();
}

TEST_F(cu_cp_ue_context_release_test, when_ue_context_release_command_received_then_release_succeeds)
{
  // Setup PDU Session
  setup_ue_pdu_session();

  // Inject NGAP UE Context Release Command and await Bearer Context Release Command
  send_ngap_ue_context_release_command_and_await_bearer_context_release_command();

  // Inject Bearer Context Release Complete and await F1AP UE Context Release Command
  send_bearer_context_release_complete_and_await_f1ap_ue_context_release_command();

  // Inject F1AP UE Context Release Complete and await NGAP UE Context Release Complete
  send_f1ap_ue_context_release_complete_and_await_ngap_ue_context_release_complete();

  // STATUS: UE should be removed at this stage
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 0) << "UE should be removed";
}

TEST_F(cu_cp_ue_context_release_test,
       when_du_initiated_ue_context_release_received_then_ue_context_release_request_is_sent)
{
  // Setup PDU Session
  setup_ue_pdu_session();

  // Inject F1AP UE Context Release Request and await NGAP UE Context Release Request
  send_f1ap_ue_context_release_request_and_await_ngap_ue_context_release_request();
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
    f1ap_message init_ul_rrc_msg = generate_init_ul_rrc_message_transfer(du_ue_id, crnti);
    test_logger.info("c-rnti={} du_ue={}: Injecting Initial UL RRC message", crnti, du_ue_id);
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
    f1ap_message ul_dcch_msg =
        generate_ul_rrc_message_transfer(cu_ue_id, du_ue_id, srb_id_t::srb1, generate_rrc_setup_complete());
    // > Generate UL RRC Message (containing RRC Setup Complete) with PDCP SN=0.
    get_du(du_idx).push_ul_pdu(ul_dcch_msg);
  }

  // Inject F1AP UE Context Release Request
  send_f1ap_ue_context_release_request(cu_ue_id, du_ue_id);

  // No NGAP UE Context Release Request should be sent to the AMF
  ASSERT_NE(ngap_pdu.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);

  // Inject F1AP UE Context Release Complete
  send_f1ap_ue_context_release_complete(cu_ue_id, du_ue_id);

  // STATUS: UE should be removed at this stage
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 0) << "UE should be removed";
}
