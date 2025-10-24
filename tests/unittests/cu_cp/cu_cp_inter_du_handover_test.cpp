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
#include "tests/test_doubles/rrc/rrc_test_message_validators.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/f1ap/f1ap_ue_id_types.h"
#include "srsran/ngap/ngap_message.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class cu_cp_inter_du_handover_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_inter_du_handover_test() : cu_cp_test_environment(cu_cp_test_env_params{})
  {
    // Run NG setup to completion.
    run_ng_setup();

    // Setup source DU.
    std::optional<unsigned> ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    source_du_idx = ret.value();
    EXPECT_TRUE(this->run_f1_setup(source_du_idx, int_to_gnb_du_id(0x11), {source_cell_info}));

    // Setup target DU.
    ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    target_du_idx = ret.value();
    EXPECT_TRUE(this->run_f1_setup(target_du_idx, int_to_gnb_du_id(0x22), {target_cell_info}));

    // Setup CU-UP.
    ret = connect_new_cu_up();
    EXPECT_TRUE(ret.has_value());
    cu_up_idx = ret.value();
    EXPECT_TRUE(this->run_e1_setup(cu_up_idx));

    // Connect UE 0x4601.
    EXPECT_TRUE(cu_cp_test_environment::attach_ue(
        source_du_idx, cu_up_idx, du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id, psi, drb_id_t::drb1, qfi));
    ue_ctx = this->find_ue_context(source_du_idx, du_ue_id);

    EXPECT_NE(ue_ctx, nullptr);
  }

  [[nodiscard]] bool send_rrc_measurement_report_and_await_ue_context_setup_request()
  {
    report_fatal_error_if_not(not this->get_du(source_du_idx).try_pop_dl_pdu(source_f1ap_pdu),
                              "there are still F1AP DL messages to pop from source DU");

    // Inject UL RRC Message (containing RRC Measurement Report) and wait for UE Context Setup Request.
    get_du(source_du_idx)
        .push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
            ue_ctx->du_ue_id.value(),
            ue_ctx->cu_ue_id.value(),
            srb_id_t::srb1,
            make_byte_buffer("000800410004015f741fe0804bf183fcaa6e9699").value()));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(target_du_idx, target_f1ap_pdu),
                              "Failed to receive UE Context Setup Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_setup_request_with_ue_capabilities(target_f1ap_pdu),
                              "Invalid UE Context Setup Request");

    report_fatal_error_if_not(target_f1ap_pdu.pdu.init_msg().value.ue_context_setup_request()->serving_cell_mo_present,
                              "ServingCellMO not present in UE Context Setup Request");

    serving_cell_mo = target_f1ap_pdu.pdu.init_msg().value.ue_context_setup_request()->serving_cell_mo;

    return true;
  }

  [[nodiscard]] bool send_ue_context_setup_failure()
  {
    report_fatal_error_if_not(not this->get_du(target_du_idx).try_pop_dl_pdu(target_f1ap_pdu),
                              "there are still F1AP DL messages to pop from target DU");

    // Inject UE Context Setup Failure.
    get_du(target_du_idx)
        .push_ul_pdu(
            test_helpers::generate_ue_context_setup_failure(ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value()));
    return true;
  }

  [[nodiscard]] bool trigger_handover_manually_and_await_ue_context_setup_request()
  {
    get_cu_cp().get_command_handler().get_mobility_command_handler().trigger_handover(
        source_cell_info.pci, ue_ctx->crnti, target_cell_info.pci);

    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(target_du_idx, target_f1ap_pdu),
                              "Failed to receive UE Context Setup Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_setup_request_with_ue_capabilities(target_f1ap_pdu),
                              "Invalid UE Context Setup Request");
    return true;
  }

  [[nodiscard]] bool trigger_handover_via_controller_and_await_ue_context_setup_request()
  {
    get_cu_cp().get_command_handler().get_mobility_command_handler().trigger_handover(
        source_cell_info.pci, ue_ctx->crnti, target_cell_info.pci);

    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(target_du_idx, target_f1ap_pdu),
                              "Failed to receive UE Context Setup Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_setup_request_with_ue_capabilities(target_f1ap_pdu),
                              "Invalid UE Context Setup Request");
    return true;
  }

  [[nodiscard]] bool send_ue_context_setup_response_and_await_ue_context_modification_request()
  {
    report_fatal_error_if_not(not this->get_du(source_du_idx).try_pop_dl_pdu(source_f1ap_pdu),
                              "there are still F1AP DL messages to pop from source DU");
    report_fatal_error_if_not(not this->get_du(target_du_idx).try_pop_dl_pdu(target_f1ap_pdu),
                              "there are still F1AP DL messages to pop from target DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU UP");

    // Inject UE Context Setup Response and await UE Context Modification Request.
    get_du(target_du_idx)
        .push_ul_pdu(test_helpers::generate_ue_context_setup_response(
            ue_ctx->cu_ue_id.value(),
            ue_ctx->du_ue_id.value(),
            crnti,
            make_byte_buffer(
                "5c04c00604b0701f00811a0f020001273b8c02692f30004d25e24040008c8040a26418d6d8d76006e08040000101000083446a"
                "48d802692f1200000464e35b63224f80b0664abff0124e9106e28dc61b8e372c6fbf56c70eb00442c0680182c4602c02052100"
                "4930b2a0003fe00000000060dc210800078059402830000002068c080a28500071c48000133557c841c001040c2050c1c9c48a"
                "163068e1e408800044280004045a8000860428000c605a80010064280014065a800185e428001c5e5a80020084280024588240"
                "a1620a02c5882c0c1620c034588340e1620e03c0e8201003a090440e829024b92a4a1814388e8acf1379343e9041e2efc0c10e"
                "000000131ffacc46a9aae502c0000432ec0000000000000000002108ad5450047001004082000e21009c400e0208108001c420"
                "138401c081041000388402708038182042000710804e18070404104000e21009c300080000008218081018201c1a0001c71000"
                "000080100020180020240088029800008c40089c72018000")
                .value()));

    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(source_du_idx, source_f1ap_pdu),
                              "Failed to receive UE Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_modification_request(source_f1ap_pdu),
                              "Invalid UE Context Modification Request");
    {
      // Check if tx_action_ind is set to stop.
      report_fatal_error_if_not(source_f1ap_pdu.pdu.init_msg().value.ue_context_mod_request()->tx_action_ind_present &&
                                    source_f1ap_pdu.pdu.init_msg().value.ue_context_mod_request()->tx_action_ind ==
                                        asn1::f1ap::tx_action_ind_e::stop,
                                "Invalid TxActionInd");

      const byte_buffer& rrc_container = test_helpers::get_rrc_container(source_f1ap_pdu);
      report_fatal_error_if_not(
          test_helpers::is_valid_rrc_reconfiguration(
              test_helpers::extract_dl_dcch_msg(rrc_container), false, {}, {}, {}, serving_cell_mo),
          "Invalid RRC Reconfiguration");
    }
    return true;
  }

  [[nodiscard]] bool send_bearer_context_modification_failure_and_await_bearer_context_release_command()
  {
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject Bearer Context Modification Failure and wait for Bearer Context Release Command.
    get_cu_up(cu_up_idx).push_tx_pdu(
        generate_bearer_context_modification_failure(ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value()));
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
                              "Failed to receive Bearer Context Release Command");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_release_command(e1ap_pdu),
                              "Invalid Bearer Context Release Command");
    return true;
  }

  [[nodiscard]] bool send_bearer_context_release_complete_and_await_ue_context_release_command()
  {
    report_fatal_error_if_not(not this->get_du(target_du_idx).try_pop_dl_pdu(target_f1ap_pdu),
                              "there are still F1AP DL messages to pop from target DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject Bearer Context Release Complete and wait for UE Context Release Command.
    get_cu_up(cu_up_idx).push_tx_pdu(
        generate_bearer_context_release_complete(ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value()));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(target_du_idx, target_f1ap_pdu),
                              "Failed to receive UE Context Release Command");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_command(target_f1ap_pdu),
                              "Invalid UE Context Release Command");
    return true;
  }

  [[nodiscard]] bool send_bearer_context_modification_response_and_await_ue_context_modification_request()
  {
    report_fatal_error_if_not(not this->get_du(target_du_idx).try_pop_dl_pdu(target_f1ap_pdu),
                              "there are still F1AP DL messages to pop from target DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject Bearer Context Modification Response and wait for RRC Reconfiguration.
    get_cu_up(cu_up_idx).push_tx_pdu(
        generate_bearer_context_modification_response(ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value()));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(target_du_idx, target_f1ap_pdu),
                              "Failed to receive UE Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_modification_request(target_f1ap_pdu),
                              "Invalid UE Context Modification Request");
    return true;
  }

  [[nodiscard]] bool send_ue_context_modification_response(unsigned du_idx)
  {
    // Inject UE Context Modification Response.
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_modification_response(
        ue_ctx->du_ue_id.value(), ue_ctx->cu_ue_id.value(), crnti));
    return true;
  }

  [[nodiscard]] bool send_rrc_reconfiguration_complete_and_await_bearer_context_modification_request()
  {
    report_fatal_error_if_not(not this->get_du(target_du_idx).try_pop_dl_pdu(target_f1ap_pdu),
                              "there are still F1AP DL messages to pop from target DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    get_du(target_du_idx)
        .push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(ue_ctx->du_ue_id.value(),
                                                                    ue_ctx->cu_ue_id.value(),
                                                                    srb_id_t::srb1,
                                                                    make_byte_buffer("80000800db659eb2").value()));
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
                              "Failed to receive Bearer Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_modification_request(e1ap_pdu),
                              "Invalid Bearer Context Modification Request");
    return true;
  }

  [[nodiscard]] bool send_ue_context_modification_response_and_await_ue_context_release_command()
  {
    report_fatal_error_if_not(not this->get_du(source_du_idx).try_pop_dl_pdu(source_f1ap_pdu),
                              "there are still F1AP DL messages to pop from source DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject UE Context Modification Response and wait for UE Context Release Command.
    get_du(target_du_idx)
        .push_ul_pdu(test_helpers::generate_ue_context_modification_response(
            ue_ctx->du_ue_id.value(), ue_ctx->cu_ue_id.value(), crnti));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(source_du_idx, source_f1ap_pdu),
                              "Failed to receive UE Context Release Command");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_command(source_f1ap_pdu),
                              "Invalid UE Context Release Command");
    return true;
  }

  [[nodiscard]] bool send_f1ap_ue_context_release_complete(unsigned du_idx)
  {
    // Inject F1AP UE Context Release Complete.
    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_ue_context_release_complete(ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value()));
    return true;
  }

  [[nodiscard]] bool send_ue_level_bearer_context_inactivity_notification_and_await_ue_context_release_request()
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject inactivity notification and wait for UE Context Release Request.
    get_cu_up(cu_up_idx).push_tx_pdu(
        generate_bearer_context_inactivity_notification_with_ue_level(ue_ctx->cu_cp_e1ap_id.value(), cu_up_e1ap_id));
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu), "Failed to receive UE Context Release Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_request(ngap_pdu),
                              "Invalid UE Context Release Request");
    return true;
  }

  unsigned source_du_idx = 0;
  unsigned target_du_idx = 0;
  unsigned cu_up_idx     = 0;

  test_helpers::served_cell_item_info source_cell_info = test_helpers::served_cell_item_info{};
  test_helpers::served_cell_item_info target_cell_info = {nr_cell_identity::create(gnb_id_t{411, 22}, 1).value(), 2};

  gnb_du_ue_f1ap_id_t du_ue_id = gnb_du_ue_f1ap_id_t::min;
  gnb_cu_ue_f1ap_id_t cu_ue_id;
  rnti_t              crnti     = to_rnti(0x4601);
  amf_ue_id_t         amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));
  gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id = gnb_cu_up_ue_e1ap_id_t::min;
  gnb_cu_cp_ue_e1ap_id_t cu_cp_e1ap_id;

  std::optional<uint8_t> serving_cell_mo = std::nullopt;

  const ue_context* ue_ctx = nullptr;

  pdu_session_id_t psi = uint_to_pdu_session_id(1);
  qos_flow_id_t    qfi = uint_to_qos_flow_id(1);

  ngap_message ngap_pdu;
  f1ap_message source_f1ap_pdu;
  f1ap_message target_f1ap_pdu;
  e1ap_message e1ap_pdu;
};

TEST_F(cu_cp_inter_du_handover_test, when_ue_context_setup_fails_then_ho_fails)
{
  // Inject Measurement Report and await F1AP UE Context Setup Request.
  ASSERT_TRUE(send_rrc_measurement_report_and_await_ue_context_setup_request());

  // Inject UE Context Setup Failure.
  ASSERT_TRUE(send_ue_context_setup_failure());

  // STATUS: UE should be removed from target DU.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 1) << "UE should be removed";
}

TEST_F(cu_cp_inter_du_handover_test, when_bearer_context_modification_fails_then_ho_fails)
{
  // Inject Measurement Report and await F1AP UE Context Setup Request.
  ASSERT_TRUE(send_rrc_measurement_report_and_await_ue_context_setup_request());

  // Inject UE Context Setup Response and await Bearer Context Modification Request.
  ASSERT_TRUE(send_ue_context_setup_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response.
  ASSERT_TRUE(send_ue_context_modification_response(source_du_idx));

  // Inject RRC Reconfiguration Complete and await Bearer Context Modification Request.
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Failure and await Bearer Context Release Command.
  ASSERT_TRUE(send_bearer_context_modification_failure_and_await_bearer_context_release_command());

  // Inject Bearer Context Release Complete and await F1AP UE Context Release Command.
  ASSERT_TRUE(send_bearer_context_release_complete_and_await_ue_context_release_command());

  // Inject F1AP UE Context Release Complete.
  ASSERT_TRUE(send_f1ap_ue_context_release_complete(target_du_idx));

  // STATUS: UE should be removed from target DU.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 1) << "UE should be removed";
}

TEST_F(cu_cp_inter_du_handover_test, when_ho_succeeds_then_source_ue_is_removed)
{
  // Check that the metrics report doesn't contain a requested/successful handover execution.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_handover_executions_requested, 0U);
  ASSERT_EQ(report.mobility.nof_successful_handover_executions, 0U);

  // Inject Measurement Report and await F1AP UE Context Setup Request.
  ASSERT_TRUE(send_rrc_measurement_report_and_await_ue_context_setup_request());

  // Inject UE Context Setup Response and await Bearer Context Modification Request.
  ASSERT_TRUE(send_ue_context_setup_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response.
  ASSERT_TRUE(send_ue_context_modification_response(source_du_idx));

  // Check that the metrics report contains a requested handover execution.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_handover_executions_requested, 1U);

  // Inject RRC Reconfiguration Complete and await Bearer Context Modification Request.
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Response and await UE Context Modification Request.
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response and await UE Context Release Command.
  ASSERT_TRUE(send_ue_context_modification_response_and_await_ue_context_release_command());

  // Inject F1AP UE Context Release Complete.
  ASSERT_TRUE(send_f1ap_ue_context_release_complete(source_du_idx));

  // Check that the metrics report contains a successful handover execution.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_successful_handover_executions, 1U);

  // STATUS: UE should be removed from source DU.
  ASSERT_EQ(report.ues.size(), 1) << "UE should be removed";
}

TEST_F(cu_cp_inter_du_handover_test, when_manually_triggered_ho_succeeds_then_source_ue_is_removed)
{
  // Check that the metrics report doesn't contain a requested/successful handover execution.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_handover_executions_requested, 0U);
  ASSERT_EQ(report.mobility.nof_successful_handover_executions, 0U);

  // Trigger handover manually and await UE Context Setup Request.
  ASSERT_TRUE(trigger_handover_manually_and_await_ue_context_setup_request());

  // Inject UE Context Setup Response and await Bearer Context Modification Request.
  ASSERT_TRUE(send_ue_context_setup_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response.
  ASSERT_TRUE(send_ue_context_modification_response(source_du_idx));
  // Check that the metrics report contains a requested handover execution.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();

  ASSERT_EQ(report.mobility.nof_handover_executions_requested, 1U);

  // Inject RRC Reconfiguration Complete and await Bearer Context Modification Request.
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Response and await UE Context Modification Request.
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response and await UE Context Release Command.
  ASSERT_TRUE(send_ue_context_modification_response_and_await_ue_context_release_command());

  // Inject F1AP UE Context Release Complete.
  ASSERT_TRUE(send_f1ap_ue_context_release_complete(source_du_idx));

  // Check that the metrics report contains a successful handover execution.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_successful_handover_executions, 1U);

  // STATUS: UE should be removed from source DU.
  ASSERT_EQ(report.ues.size(), 1) << "UE should be removed";
}

TEST_F(cu_cp_inter_du_handover_test, when_controller_triggered_ho_succeeds_then_source_ue_is_removed)
{
  // Check that the metrics report doesn't contain a requested/successful handover execution.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_handover_executions_requested, 0U);
  ASSERT_EQ(report.mobility.nof_successful_handover_executions, 0U);

  // Trigger handover manually and await UE Context Setup Request.
  ASSERT_TRUE(trigger_handover_via_controller_and_await_ue_context_setup_request());

  // Inject UE Context Setup Response and await Bearer Context Modification Request.
  ASSERT_TRUE(send_ue_context_setup_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response
  ASSERT_TRUE(send_ue_context_modification_response(source_du_idx));
  // Check that the metrics report contains a requested handover execution.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();

  ASSERT_EQ(report.mobility.nof_handover_executions_requested, 1U);

  // Inject RRC Reconfiguration Complete and await Bearer Context Modification Request.
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Response and await UE Context Modification Request.
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response and await UE Context Release Command.
  ASSERT_TRUE(send_ue_context_modification_response_and_await_ue_context_release_command());

  // Inject F1AP UE Context Release Complete.
  ASSERT_TRUE(send_f1ap_ue_context_release_complete(source_du_idx));

  // Check that the metrics report contains a successful handover execution.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_successful_handover_executions, 1U);

  // STATUS: UE should be removed from source DU.
  ASSERT_EQ(report.ues.size(), 1) << "UE should be removed";
}

TEST_F(cu_cp_inter_du_handover_test, when_ho_succeeds_then_target_ue_is_connected_to_amf)
{
  // Check that the metrics report doesn't contain a requested/successful handover execution.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_handover_executions_requested, 0U);
  ASSERT_EQ(report.mobility.nof_successful_handover_executions, 0U);

  // Inject Measurement Report and await F1AP UE Context Setup Request.
  ASSERT_TRUE(send_rrc_measurement_report_and_await_ue_context_setup_request());

  // Inject UE Context Setup Response and await Bearer Context Modification Request.
  ASSERT_TRUE(send_ue_context_setup_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response.
  ASSERT_TRUE(send_ue_context_modification_response(source_du_idx));

  // Check that the metrics report contains a requested handover execution.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_handover_executions_requested, 1U);

  // Inject RRC Reconfiguration Complete and await Bearer Context Modification Request.
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Response and await UE Context Modification Request.
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response and await UE Context Release Command.
  ASSERT_TRUE(send_ue_context_modification_response_and_await_ue_context_release_command());

  // Inject F1AP UE Context Release Complete.
  ASSERT_TRUE(send_f1ap_ue_context_release_complete(source_du_idx));

  // Check that the metrics report contains a successful handover execution.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_successful_handover_executions, 1U);

  // STATUS: UE should be removed from source DU.
  ASSERT_EQ(report.ues.size(), 1) << "UE should be removed";

  // Check that the UE is connected to the AMF by injecting a deregistration request and make sure its forwarded.
  f1ap_message ul_rrc_msg_transfer = test_helpers::generate_ul_rrc_message_transfer(
      du_ue_id,
      ue_ctx->cu_ue_id.value(),
      srb_id_t::srb1,
      make_byte_buffer("00013a0c3f016f19764701bf0022808005f900788801002060000306802bffe479").value());
  get_du(target_du_idx).push_ul_pdu(ul_rrc_msg_transfer);
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_ul_nas_transport_message(ngap_pdu));
}
