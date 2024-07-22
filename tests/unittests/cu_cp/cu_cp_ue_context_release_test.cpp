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
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "tests/unittests/f1ap/common/f1ap_cu_test_messages.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/f1ap/common/f1ap_message.h"
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

  void send_f1ap_ue_context_release_complete_and_await_ngap_ue_context_release_complete()
  {
    // Inject F1AP UE Context Release Complete and wait for N1AP UE Context Release Command
    get_du(du_idx).push_ul_pdu(
        generate_ue_context_release_complete(ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value()));
    bool result = this->wait_for_ngap_tx_pdu(ngap_pdu);
    report_fatal_error_if_not(result, "Failed to receive UE Context Release Complete");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_complete(ngap_pdu),
                              "Invalid UE Context Release Complete");
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
  // Inject NGAP UE Context Release Command and await F1AP UE Context Release Command
  send_ngap_ue_context_release_command_and_await_f1ap_ue_context_release_command();

  // Inject F1AP UE Context Release Complete and await NGAP UE Context Release Complete
  send_f1ap_ue_context_release_complete_and_await_ngap_ue_context_release_complete();

  // STATUS: UE should be removed at this stage
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 0) << "UE should be removed";
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
