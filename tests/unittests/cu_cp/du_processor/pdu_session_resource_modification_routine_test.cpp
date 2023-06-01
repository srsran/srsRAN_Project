/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

class pdu_session_resource_modification_test : public du_processor_routine_manager_test
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

  void start_procedure(const cu_cp_pdu_session_resource_modify_request& msg)
  {
    t = routine_mng->start_pdu_session_resource_modification_routine(
        msg, rrc_ue_ctrl_notifier, *rrc_ue_up_resource_manager);
    t_launcher.emplace(t);
  }

  bool procedure_ready() const { return t.ready(); }

  const cu_cp_pdu_session_resource_modify_response& result() { return t.get(); }

  // Preamble to setup a initial PDU session.
  void setup_pdu_session()
  {
    // Set expected results for sub-procedures.
    e1ap_ctrl_notifier.set_first_message_outcome({true, {1}, {}});
    f1ap_ue_ctxt_notifier.set_ue_context_modification_outcome({true});
    e1ap_ctrl_notifier.set_second_message_outcome({true});
    rrc_ue_ctrl_notifier.set_rrc_reconfiguration_outcome(true);

    // Run setup procedure.
    cu_cp_pdu_session_resource_setup_request              request = generate_pdu_session_resource_setup();
    async_task<cu_cp_pdu_session_resource_setup_response> setup_task =
        routine_mng->start_pdu_session_resource_setup_routine(
            request, security_cfg, rrc_ue_ctrl_notifier, *rrc_ue_up_resource_manager);
    lazy_task_launcher<cu_cp_pdu_session_resource_setup_response> setup_launcher(setup_task);

    // Verify successful outcome.
    ASSERT_TRUE(setup_task.ready());
    ASSERT_EQ(setup_task.get().pdu_session_res_setup_response_items.size(), 1);
    ASSERT_EQ(setup_task.get().pdu_session_res_failed_to_setup_items.size(), 0);

    // clear stored E1AP requests for next procedure
    e1ap_ctrl_notifier.reset();
  }

private:
  async_task<cu_cp_pdu_session_resource_modify_response>                   t;
  optional<lazy_task_launcher<cu_cp_pdu_session_resource_modify_response>> t_launcher;
};

TEST_F(pdu_session_resource_modification_test,
       when_modification_request_with_inactive_pdu_session_arrives_then_modification_fails)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_modify_request request = generate_pdu_session_resource_modification();

  // Start modification routine (without setting any results).
  start_procedure(request);

  // PDU session resource modification for session 1 failed.
  ASSERT_TRUE(procedure_ready());
  ASSERT_EQ(result().pdu_session_res_failed_to_modify_list.size(), 1);
  ASSERT_EQ(result().pdu_session_res_failed_to_modify_list.begin()->pdu_session_id, uint_to_pdu_session_id(1));
}

TEST_F(pdu_session_resource_modification_test, when_bearer_ctxt_modification_fails_then_pdu_session_modification_fails)
{
  // Test Preamble - Setup a single PDU session initially.
  setup_pdu_session();

  // Set expected results for sub-procedures.
  set_expected_results({false}, {}, {}, false);

  // Start modification routine.
  cu_cp_pdu_session_resource_modify_request request = generate_pdu_session_resource_modification();
  start_procedure(request);

  // Verify content of initial bearer modification request.
  ASSERT_TRUE(e1ap_ctrl_notifier.first_e1ap_request.has_value());
  ASSERT_TRUE(variant_holds_alternative<e1ap_bearer_context_modification_request>(
      e1ap_ctrl_notifier.first_e1ap_request.value()));
  const auto& bearer_ctxt_mod_req =
      variant_get<e1ap_bearer_context_modification_request>(e1ap_ctrl_notifier.first_e1ap_request.value());
  ASSERT_TRUE(bearer_ctxt_mod_req.ng_ran_bearer_context_mod_request.has_value());
  ASSERT_EQ(bearer_ctxt_mod_req.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_modify_list.size(), 1);
  ASSERT_EQ(bearer_ctxt_mod_req.ng_ran_bearer_context_mod_request.value()
                .pdu_session_res_to_modify_list.begin()
                ->pdu_session_id,
            uint_to_pdu_session_id(1));
  ASSERT_EQ(bearer_ctxt_mod_req.ng_ran_bearer_context_mod_request.value()
                .pdu_session_res_to_modify_list.begin()
                ->drb_to_setup_list_ng_ran.size(),
            1);

  // PDU session resource modification for session 1 failed.
  ASSERT_TRUE(procedure_ready());
  ASSERT_EQ(result().pdu_session_res_failed_to_modify_list.size(), 1);
  ASSERT_EQ(result().pdu_session_res_failed_to_modify_list.begin()->pdu_session_id, uint_to_pdu_session_id(1));
}

TEST_F(pdu_session_resource_modification_test, when_ue_ctxt_modification_fails_then_pdu_session_modification_fails)
{
  // Test Preamble - Setup a single PDU session initially.
  setup_pdu_session();

  // Set expected results for sub-procedures.
  bearer_context_outcome_t first_bearer_ctxt_mod_outcome;
  first_bearer_ctxt_mod_outcome.outcome                    = true;
  first_bearer_ctxt_mod_outcome.pdu_sessions_setup_list    = {};
  first_bearer_ctxt_mod_outcome.pdu_sessions_failed_list   = {};
  first_bearer_ctxt_mod_outcome.pdu_sessions_modified_list = {1};
  set_expected_results(first_bearer_ctxt_mod_outcome, {false}, {}, false);

  // Start modification routine.
  cu_cp_pdu_session_resource_modify_request request = generate_pdu_session_resource_modification();
  start_procedure(request);

  // Verify content of UE context modifcation.
  ASSERT_EQ(f1ap_ue_ctxt_notifier.get_ctxt_mod_request().drbs_to_be_setup_mod_list.size(), 1);

  // PDU session resource modification for session 1 failed.
  ASSERT_TRUE(procedure_ready());
  ASSERT_EQ(result().pdu_session_res_failed_to_modify_list.size(), 1);
  ASSERT_EQ(result().pdu_session_res_failed_to_modify_list.begin()->pdu_session_id, uint_to_pdu_session_id(1));
}

TEST_F(pdu_session_resource_modification_test,
       when_second_bearer_ctxt_modification_fails_then_pdu_session_modification_fails)
{
  // Test Preamble - Setup a single PDU session initially.
  setup_pdu_session();

  // Set expected results for sub-procedures.
  bearer_context_outcome_t first_bearer_ctxt_mod_outcome;
  first_bearer_ctxt_mod_outcome.outcome                    = true;
  first_bearer_ctxt_mod_outcome.pdu_sessions_setup_list    = {};
  first_bearer_ctxt_mod_outcome.pdu_sessions_failed_list   = {};
  first_bearer_ctxt_mod_outcome.pdu_sessions_modified_list = {1};
  ue_context_outcome_t ue_cxt_mod_outcome;
  ue_cxt_mod_outcome.outcome          = true;
  ue_cxt_mod_outcome.drb_success_list = {2}; // setup of DRB was ok
  ue_cxt_mod_outcome.drb_failed_list  = {};
  bearer_context_outcome_t second_bearer_ctxt_mod_outcome;
  second_bearer_ctxt_mod_outcome.outcome = false;
  set_expected_results(first_bearer_ctxt_mod_outcome, ue_cxt_mod_outcome, second_bearer_ctxt_mod_outcome, false);

  // Start modification routine.
  cu_cp_pdu_session_resource_modify_request request = generate_pdu_session_resource_modification();
  start_procedure(request);

  // Verify content of 2nd bearer context modification request.
  ASSERT_TRUE(e1ap_ctrl_notifier.second_e1ap_request.has_value());
  ASSERT_TRUE(e1ap_ctrl_notifier.second_e1ap_request.value().ng_ran_bearer_context_mod_request.has_value());
  ASSERT_EQ(e1ap_ctrl_notifier.second_e1ap_request.value()
                .ng_ran_bearer_context_mod_request.value()
                .pdu_session_res_to_modify_list.size(),
            1);

  // PDU session resource modification for session 1 failed.
  ASSERT_TRUE(procedure_ready());
  ASSERT_EQ(result().pdu_session_res_failed_to_modify_list.size(), 1);
  ASSERT_EQ(result().pdu_session_res_failed_to_modify_list.begin()->pdu_session_id, uint_to_pdu_session_id(1));
}

TEST_F(pdu_session_resource_modification_test, when_rrc_reconfiguration_fails_then_pdu_session_modification_fails)
{
  // Test Preamble - Setup a single PDU session initially.
  setup_pdu_session();

  // Set expected results for sub-procedures.
  bearer_context_outcome_t first_bearer_ctxt_mod_outcome;
  first_bearer_ctxt_mod_outcome.outcome                    = true;
  first_bearer_ctxt_mod_outcome.pdu_sessions_setup_list    = {};
  first_bearer_ctxt_mod_outcome.pdu_sessions_failed_list   = {};
  first_bearer_ctxt_mod_outcome.pdu_sessions_modified_list = {1};
  ue_context_outcome_t ue_cxt_mod_outcome;
  ue_cxt_mod_outcome.outcome          = true;
  ue_cxt_mod_outcome.drb_success_list = {2}; // setup of DRB was ok
  ue_cxt_mod_outcome.drb_failed_list  = {};
  bearer_context_outcome_t second_bearer_ctxt_mod_outcome;
  second_bearer_ctxt_mod_outcome.outcome                    = true;
  second_bearer_ctxt_mod_outcome.pdu_sessions_setup_list    = {};
  second_bearer_ctxt_mod_outcome.pdu_sessions_failed_list   = {};
  second_bearer_ctxt_mod_outcome.pdu_sessions_modified_list = {1};
  set_expected_results(first_bearer_ctxt_mod_outcome, ue_cxt_mod_outcome, second_bearer_ctxt_mod_outcome, false);

  // Start modification routine.
  cu_cp_pdu_session_resource_modify_request request = generate_pdu_session_resource_modification();
  start_procedure(request);

  // Verify content of UE context modifcation.
  ASSERT_EQ(f1ap_ue_ctxt_notifier.get_ctxt_mod_request().drbs_to_be_setup_mod_list.size(), 1);

  // PDU session resource modification for session 1 failed.
  ASSERT_TRUE(procedure_ready());
  ASSERT_EQ(result().pdu_session_res_failed_to_modify_list.size(), 1);
  ASSERT_EQ(result().pdu_session_res_failed_to_modify_list.begin()->pdu_session_id, uint_to_pdu_session_id(1));
}
