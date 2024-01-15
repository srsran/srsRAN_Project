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

#include "du_processor_routine_manager_test_helpers.h"
#include "lib/e1ap/cu_cp/e1ap_cu_cp_asn1_helpers.h"
#include "pdu_session_resource_routine_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Note: Check if UE ID is valid is done by caller. Injection of invalid ue_index results in assertion.

class pdu_session_resource_setup_test : public pdu_session_resource_routine_test
{
protected:
  void set_expected_results(const bearer_context_outcome_t& first_e1ap_message_outcome,
                            const ue_context_outcome_t&     ue_context_modification_outcome,
                            const bearer_context_outcome_t& second_e1ap_message_outcome,
                            bool                            rrc_reconfiguration_outcome)
  {
    e1ap_ctrl_notifier.set_first_message_outcome(first_e1ap_message_outcome);
    f1ap_ue_ctxt_notifier.set_ue_context_modification_outcome(ue_context_modification_outcome);
    e1ap_ctrl_notifier.set_second_message_outcome(second_e1ap_message_outcome);
    rrc_ue_ctrl_notifier.set_rrc_reconfiguration_outcome(rrc_reconfiguration_outcome);
  }

  void start_procedure(const cu_cp_pdu_session_resource_setup_request& msg)
  {
    t = routine_mng->start_pdu_session_resource_setup_routine(
        msg, security_cfg, rrc_ue_ctrl_notifier, *rrc_ue_up_resource_manager);
    t_launcher.emplace(t);
  }

  bool procedure_ready() const { return t.ready(); }

  // Return SRB IDs that were requested to be added/modified in RRC reconfiguration.
  std::list<unsigned> rrc_srbs_to_add_mod_in_reconf()
  {
    std::list<unsigned> srb_list = {};
    if (not rrc_ue_ctrl_notifier.last_radio_bearer_cfg.has_value()) {
      return srb_list;
    }
    for (const auto& item : rrc_ue_ctrl_notifier.last_radio_bearer_cfg->srb_to_add_mod_list) {
      srb_list.push_back(srb_id_to_uint(item.srb_id));
    }
    return srb_list;
  }

  // Return PDU session IDs that failed to be setup.
  std::list<unsigned> pdu_session_res_failed_to_setup()
  {
    std::list<unsigned> failed_list;
    for (const auto& item : t.get().pdu_session_res_failed_to_setup_items) {
      failed_list.push_back(pdu_session_id_to_uint(item.pdu_session_id));
    }
    return failed_list;
  }

  // Return PDU session IDs that succeeded to be setup.
  std::list<unsigned> pdu_session_res_setup()
  {
    std::list<unsigned> setup_list;
    for (const auto& item : t.get().pdu_session_res_setup_response_items) {
      setup_list.push_back(pdu_session_id_to_uint(item.pdu_session_id));
    }
    return setup_list;
  }

private:
  async_task<cu_cp_pdu_session_resource_setup_response>                   t;
  optional<lazy_task_launcher<cu_cp_pdu_session_resource_setup_response>> t_launcher;
};

TEST_F(pdu_session_resource_setup_test, when_pdu_session_setup_request_with_unconfigured_fiveqi_received_setup_fails)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup();
  // Tweak 5QI of first QoS flow
  request.pdu_session_res_setup_items.begin()
      ->qos_flow_setup_request_items.begin()
      ->qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi.value()
      .five_qi = uint_to_five_qi(99);

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_outcome_t bearer_context_setup_outcome{false, {}, {}};
  ue_context_outcome_t     ue_context_modification_outcome{true};
  bearer_context_outcome_t bearer_context_modification_outcome{false};
  set_expected_results(
      bearer_context_setup_outcome, ue_context_modification_outcome, bearer_context_modification_outcome, true);

  // it should be ready immediately
  start_procedure(request);

  // nothing has failed to setup
  VERIFY_EQUAL(pdu_session_res_failed_to_setup(), {1});

  // SRB2 should not be setup
  VERIFY_EQUAL(rrc_srbs_to_add_mod_in_reconf(), {});
}

TEST_F(pdu_session_resource_setup_test, when_bearer_context_setup_failure_received_then_setup_fails)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup();

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_outcome_t bearer_context_setup_outcome{false, {}, {}};
  bearer_context_outcome_t bearer_context_modification_outcome{false};
  set_expected_results(bearer_context_setup_outcome, {false}, bearer_context_modification_outcome, false);

  start_procedure(request);

  // PDU session resource setup failed.
  VERIFY_EQUAL(pdu_session_res_failed_to_setup(), {1});

  // SRB2 should not be setup
  VERIFY_EQUAL(rrc_srbs_to_add_mod_in_reconf(), {});
}

TEST_F(pdu_session_resource_setup_test, when_ue_context_modification_failure_received_then_setup_fails)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup();

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_outcome_t bearer_context_setup_outcome{true, {1}, {}};
  bearer_context_outcome_t bearer_context_modification_outcome{false};
  set_expected_results(bearer_context_setup_outcome, {false}, bearer_context_modification_outcome, false);
  start_procedure(request);

  // PDU session resource setup failed.
  VERIFY_EQUAL(pdu_session_res_failed_to_setup(), {1});

  // SRB2 should not be setup
  VERIFY_EQUAL(rrc_srbs_to_add_mod_in_reconf(), {});
}

TEST_F(pdu_session_resource_setup_test, when_bearer_context_modification_failure_received_then_setup_fails)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup();

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_outcome_t bearer_context_setup_outcome{true, {1}, {}};
  ue_context_outcome_t     ue_context_modification_outcome{true, {1}};
  bearer_context_outcome_t bearer_context_modification_outcome{false, {}, {1}};
  set_expected_results(
      bearer_context_setup_outcome, ue_context_modification_outcome, bearer_context_modification_outcome, false);
  start_procedure(request);

  // Verify content of bearer modification request.
  ASSERT_TRUE(e1ap_ctrl_notifier.second_e1ap_request.has_value());
  const auto& bearer_ctxt_mod_req = e1ap_ctrl_notifier.second_e1ap_request.value();
  ASSERT_TRUE(bearer_ctxt_mod_req.ng_ran_bearer_context_mod_request.has_value());
  ASSERT_EQ(bearer_ctxt_mod_req.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_modify_list.size(), 1);
  ASSERT_EQ(bearer_ctxt_mod_req.ng_ran_bearer_context_mod_request.value()
                .pdu_session_res_to_modify_list.begin()
                ->pdu_session_id,
            uint_to_pdu_session_id(1));
  ASSERT_EQ(bearer_ctxt_mod_req.ng_ran_bearer_context_mod_request.value()
                .pdu_session_res_to_modify_list.begin()
                ->drb_to_modify_list_ng_ran.size(),
            1);

  // PDU session resource setup failed.
  VERIFY_EQUAL(pdu_session_res_failed_to_setup(), {1});

  // SRB2 should not be setup
  VERIFY_EQUAL(rrc_srbs_to_add_mod_in_reconf(), {});
}

TEST_F(pdu_session_resource_setup_test, when_rrc_reconfiguration_fails_then_setup_fails)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup();

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_outcome_t bearer_context_setup_outcome{true, {1}, {}};
  ue_context_outcome_t     ue_context_modification_outcome{true};
  bearer_context_outcome_t bearer_context_modification_outcome{true};
  set_expected_results(
      bearer_context_setup_outcome, ue_context_modification_outcome, bearer_context_modification_outcome, false);
  start_procedure(request);

  // PDU session resource setup failed.
  VERIFY_EQUAL(pdu_session_res_setup(), {});
  VERIFY_EQUAL(pdu_session_res_failed_to_setup(), {1});
}

TEST_F(pdu_session_resource_setup_test, when_rrc_reconfiguration_succeeds_then_setup_succeeds)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup();

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_outcome_t bearer_context_setup_outcome{true, {1}, {}};
  ue_context_outcome_t     ue_context_modification_outcome{true};
  bearer_context_outcome_t bearer_context_modification_outcome{true};
  set_expected_results(
      bearer_context_setup_outcome, ue_context_modification_outcome, bearer_context_modification_outcome, true);

  start_procedure(request);

  // Verify validity of Bearer context setup.
  is_valid_e1ap_message(variant_get<e1ap_bearer_context_setup_request>(e1ap_ctrl_notifier.first_e1ap_request.value()));

  // Verify content of UE context modification which should include the setup of DRB1.
  const auto& ue_ctxt_mod_req = f1ap_ue_ctxt_notifier.get_ctxt_mod_request();
  ASSERT_EQ(ue_ctxt_mod_req.drbs_to_be_setup_mod_list.size(), 1);
  ASSERT_EQ(ue_ctxt_mod_req.drbs_to_be_setup_mod_list.begin()->drb_id, uint_to_drb_id(1));
  ASSERT_EQ(ue_ctxt_mod_req.drbs_to_be_setup_mod_list.begin()->qos_info.flows_mapped_to_drb_list.size(), 1);

  // Verify generated messages can be packed into valid ASN.1 encoded messages
  is_valid_f1ap_message(f1ap_ue_ctxt_notifier.get_ctxt_mod_request());

  // Verify content of Bearer modification request.
  ASSERT_TRUE(e1ap_ctrl_notifier.second_e1ap_request.has_value());
  const auto& bearer_context_mod_req = e1ap_ctrl_notifier.second_e1ap_request.value();
  ASSERT_TRUE(bearer_context_mod_req.ng_ran_bearer_context_mod_request.has_value());
  // The second bearer context modification includes a PDU session modification, but no setup.
  ASSERT_EQ(bearer_context_mod_req.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_modify_list.size(), 1);
  ASSERT_EQ(bearer_context_mod_req.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_setup_mod_list.size(),
            0);

  // nothing has failed to setup
  VERIFY_EQUAL(pdu_session_res_setup(), {1});
  VERIFY_EQUAL(pdu_session_res_failed_to_setup(), {});

  // SRB2 should be setup
  VERIFY_EQUAL(rrc_srbs_to_add_mod_in_reconf(), {2});
}

TEST_F(pdu_session_resource_setup_test, when_pdu_session_setup_for_existing_session_arrives_then_setup_fails)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup();

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_outcome_t bearer_context_setup_outcome{true, {1}, {}};
  ue_context_outcome_t     ue_context_modification_outcome{true};
  bearer_context_outcome_t bearer_context_modification_outcome{true};
  set_expected_results(
      bearer_context_setup_outcome, ue_context_modification_outcome, bearer_context_modification_outcome, true);
  start_procedure(request);

  // nothing has failed to setup
  VERIFY_EQUAL(pdu_session_res_setup(), {1});

  // SRB2 should be setup
  VERIFY_EQUAL(rrc_srbs_to_add_mod_in_reconf(), {2});

  // Reset stored SRB information
  rrc_ue_ctrl_notifier.reset();

  // Inject same PDU session resource setup again.
  start_procedure(request);

  // 2nd setup attempt failed.
  VERIFY_EQUAL(pdu_session_res_failed_to_setup(), {1});

  // SRB2 should not be setup now
  VERIFY_EQUAL(rrc_srbs_to_add_mod_in_reconf(), {});
}

/// Test handling of PDU session setup request without any setup item.
TEST_F(pdu_session_resource_setup_test, when_empty_pdu_session_setup_request_received_then_setup_fails)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = {}; // empty message
  request.ue_index                                 = uint_to_ue_index(0);

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_outcome_t bearer_context_setup_outcome{true, {}, {}};
  ue_context_outcome_t     ue_context_modification_outcome{true};
  bearer_context_outcome_t bearer_context_modification_outcome{true};
  set_expected_results(
      bearer_context_setup_outcome, ue_context_modification_outcome, bearer_context_modification_outcome, true);
  start_procedure(request);

  // it should be ready immediately
  ASSERT_TRUE(procedure_ready());

  // Empy success and fail list.
  VERIFY_EQUAL(pdu_session_res_setup(), {});
  VERIFY_EQUAL(pdu_session_res_failed_to_setup(), {});

  // SRB2 should not be setup
  VERIFY_EQUAL(rrc_srbs_to_add_mod_in_reconf(), {});
}

/// One PDU session with two QoS flows.
TEST_F(pdu_session_resource_setup_test, when_setup_for_pdu_sessions_with_two_qos_flows_received_setup_succeeds)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup(1, 2);

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_outcome_t bearer_context_setup_outcome{true, {1}, {}}; // first session success, second failed
  ue_context_outcome_t     ue_context_modification_outcome{true};
  bearer_context_outcome_t bearer_context_modification_outcome{true};
  set_expected_results(
      bearer_context_setup_outcome, ue_context_modification_outcome, bearer_context_modification_outcome, true);
  start_procedure(request);

  // it should be ready immediately
  ASSERT_TRUE(procedure_ready());

  // Verify Bearer Context Setup request for two DRBs with one QoS flow each.
  ASSERT_TRUE(
      variant_holds_alternative<e1ap_bearer_context_setup_request>(e1ap_ctrl_notifier.first_e1ap_request.value()));
  const auto& context_setup_req =
      variant_get<e1ap_bearer_context_setup_request>(e1ap_ctrl_notifier.first_e1ap_request.value());
  ASSERT_EQ(context_setup_req.pdu_session_res_to_setup_list.size(), 1);
  ASSERT_EQ(context_setup_req.pdu_session_res_to_setup_list.begin()->drb_to_setup_list_ng_ran.size(), 2);
  ASSERT_EQ(context_setup_req.pdu_session_res_to_setup_list.begin()
                ->drb_to_setup_list_ng_ran.begin()
                ->qos_flow_info_to_be_setup.size(),
            1);

  // Failed list empty.
  VERIFY_EQUAL(pdu_session_res_failed_to_setup(), {});

  // One PDU session succeeded to be setup.
  VERIFY_EQUAL(pdu_session_res_setup(), {1});

  // One SRB2 should be setup
  VERIFY_EQUAL(rrc_srbs_to_add_mod_in_reconf(), {2});
}

/// Test with multiple PDU sessions in same request.
TEST_F(pdu_session_resource_setup_test,
       when_setup_for_two_pdu_sessions_is_requested_but_only_first_could_be_setup_at_cuup_setup_succeeds_with_fail_list)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup(2);

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_outcome_t bearer_context_setup_outcome{true, {1}, {2}}; // first session success, second failed
  ue_context_outcome_t     ue_context_modification_outcome{true};
  bearer_context_outcome_t bearer_context_modification_outcome{true};
  set_expected_results(
      bearer_context_setup_outcome, ue_context_modification_outcome, bearer_context_modification_outcome, true);
  this->start_procedure(request);

  // it should be ready immediately
  ASSERT_TRUE(procedure_ready());

  // One PDU session succeeded to be setup.
  VERIFY_EQUAL(pdu_session_res_setup(), {1});

  // One PDU session failed to be setup.
  VERIFY_EQUAL(pdu_session_res_failed_to_setup(), {2});

  // One SRB2 should be setup
  VERIFY_EQUAL(rrc_srbs_to_add_mod_in_reconf(), {2});
}

TEST_F(pdu_session_resource_setup_test, when_setup_for_two_pdu_sessions_is_requested_and_both_succeed_setup_succeeds)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup(2);

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_outcome_t bearer_context_setup_outcome{true, {1, 2}, {}}; // first session success, second failed
  ue_context_outcome_t     ue_context_modification_outcome{true};
  bearer_context_outcome_t bearer_context_modification_outcome{true};
  set_expected_results(
      bearer_context_setup_outcome, ue_context_modification_outcome, bearer_context_modification_outcome, true);
  start_procedure(request);

  // it should be ready immediately
  ASSERT_TRUE(procedure_ready());

  // Two PDU session succeeded to be setup.
  VERIFY_EQUAL(pdu_session_res_setup(), {1, 2});

  // Zero PDU session failed to be setup.
  VERIFY_EQUAL(pdu_session_res_failed_to_setup(), {});

  // One SRB2 should be setup
  VERIFY_EQUAL(rrc_srbs_to_add_mod_in_reconf(), {2});
}

/// Test with two consecutive requests. Both with one PDU session.
TEST_F(pdu_session_resource_setup_test, when_two_consecutive_setups_arrive_bearer_setup_and_modification_succeed)
{
  // Initial PDU SESSION RESOURCE SETUP procdure using default executor.
  {
    cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup(1);

    bearer_context_outcome_t bearer_context_setup_outcome{true, {1}, {}}; // PDU session 1 setup success, no failure
    ue_context_outcome_t     ue_context_modification_outcome{true, {1}};
    bearer_context_outcome_t bearer_context_modification_outcome{true};
    set_expected_results(
        bearer_context_setup_outcome, ue_context_modification_outcome, bearer_context_modification_outcome, true);

    // it should be ready immediately
    start_procedure(request);
    ASSERT_TRUE(procedure_ready());

    // Failed list empty.
    VERIFY_EQUAL(pdu_session_res_failed_to_setup(), {});

    // First PDU session setup succeeded.
    VERIFY_EQUAL(pdu_session_res_setup(), {1});

    // One SRB2 should be setup
    VERIFY_EQUAL(rrc_srbs_to_add_mod_in_reconf(), {2});

    // Verify generated messages can be packed into valid ASN.1 encoded messages
    is_valid_e1ap_message(
        variant_get<e1ap_bearer_context_setup_request>(e1ap_ctrl_notifier.first_e1ap_request.value()));
    is_valid_e1ap_message(e1ap_ctrl_notifier.second_e1ap_request.value());
  }

  // clear stored E1AP requests/RRC reconf for next procedure
  e1ap_ctrl_notifier.reset();
  rrc_ue_ctrl_notifier.reset();

  {
    // Generate 2nd request for different PDU session ID (we generate a request for two sessions and delete the first)
    cu_cp_pdu_session_resource_setup_request request2 = generate_pdu_session_resource_setup(2, 1);
    request2.pdu_session_res_setup_items.erase(uint_to_pdu_session_id(1));

    // Modify 5QI such that a new DRB is going to be created.
    request2.pdu_session_res_setup_items[uint_to_pdu_session_id(2)]
        .qos_flow_setup_request_items.begin()
        ->qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi.value()
        .five_qi = uint_to_five_qi(7);

    bearer_context_outcome_t first_bearer_context_modification_outcome{
        true, {2}, {}}; // PDU session 2 setup success, no failure
    ue_context_outcome_t     ue_context_modification_outcome{true, {2}};
    bearer_context_outcome_t second_bearer_context_modification_outcome{true, {2}}; // Modification for DRB2

    set_expected_results(first_bearer_context_modification_outcome,
                         ue_context_modification_outcome,
                         second_bearer_context_modification_outcome,
                         true);

    start_procedure(request2);
    ASSERT_TRUE(procedure_ready());

    // Verify content of first bearer context modifications which should be the setup of a new PDU session.
    ASSERT_TRUE(variant_holds_alternative<e1ap_bearer_context_modification_request>(
        e1ap_ctrl_notifier.first_e1ap_request.value()));
    const auto& context_mod_req =
        variant_get<e1ap_bearer_context_modification_request>(e1ap_ctrl_notifier.first_e1ap_request.value())
            .ng_ran_bearer_context_mod_request;
    ASSERT_TRUE(context_mod_req.has_value());
    ASSERT_EQ(context_mod_req.value().pdu_session_res_to_setup_mod_list.size(), 1);
    ASSERT_EQ(context_mod_req.value().pdu_session_res_to_modify_list.size(), 0);

    // Verify generated messages can be packed into valid ASN.1 encoded messages
    is_valid_e1ap_message(
        variant_get<e1ap_bearer_context_modification_request>(e1ap_ctrl_notifier.first_e1ap_request.value()));

    // Verify content of UE context modification which should include the setup of DRB2.
    const auto& ue_ctxt_mod_req = f1ap_ue_ctxt_notifier.get_ctxt_mod_request();
    ASSERT_EQ(ue_ctxt_mod_req.drbs_to_be_setup_mod_list.size(), 1);
    ASSERT_EQ(ue_ctxt_mod_req.drbs_to_be_setup_mod_list.begin()->drb_id, uint_to_drb_id(2));

    // Verify content of second bearer modification request.
    ASSERT_TRUE(e1ap_ctrl_notifier.second_e1ap_request.has_value());
    const auto& second_context_mod_req = e1ap_ctrl_notifier.second_e1ap_request.value();
    ASSERT_TRUE(second_context_mod_req.ng_ran_bearer_context_mod_request.has_value());
    // The second bearer context modification includes a PDU session modification again.
    ASSERT_EQ(second_context_mod_req.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_modify_list.size(),
              1);
    ASSERT_EQ(second_context_mod_req.ng_ran_bearer_context_mod_request.value()
                  .pdu_session_res_to_modify_list.begin()
                  ->pdu_session_id,
              uint_to_pdu_session_id(2));
    ASSERT_EQ(second_context_mod_req.ng_ran_bearer_context_mod_request.value()
                  .pdu_session_res_to_modify_list.begin()
                  ->drb_to_modify_list_ng_ran.size(),
              1);

    // Verify generated messages can be packed into valid ASN.1 encoded messages
    is_valid_e1ap_message(e1ap_ctrl_notifier.second_e1ap_request.value());

    // PDU session setup for ID=2 succeeded.
    VERIFY_EQUAL(pdu_session_res_setup(), {2});

    // SRB2 should not be setup again
    VERIFY_EQUAL(rrc_srbs_to_add_mod_in_reconf(), {});
  }
}
