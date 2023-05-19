/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
    t = routine_mng->start_pdu_session_resource_modification_routine(msg, *rrc_ue_up_resource_manager);
    t_launcher.emplace(t);
  }

  bool procedure_ready() const { return t.ready(); }

  const cu_cp_pdu_session_resource_modify_response& result() { return t.get(); }

private:
  async_task<cu_cp_pdu_session_resource_modify_response>                   t;
  optional<lazy_task_launcher<cu_cp_pdu_session_resource_modify_response>> t_launcher;
};

TEST_F(pdu_session_resource_modification_test,
       when_modification_request_with_inactive_pdu_session_arrives_then_modification_fails)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_modify_request request = generate_pdu_session_resource_modification();

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_outcome_t bearer_context_setup_outcome{false, {}, {}};
  bearer_context_outcome_t bearer_context_modification_outcome{false};
  set_expected_results(bearer_context_setup_outcome, {false}, bearer_context_modification_outcome, false);

  start_procedure(request);

  // PDU session resource modification for session 1 failed.
  ASSERT_TRUE(procedure_ready());
  ASSERT_EQ(result().pdu_session_res_failed_to_modify_list.size(), 1);
  ASSERT_EQ(result().pdu_session_res_failed_to_modify_list.begin()->pdu_session_id, uint_to_pdu_session_id(1));
}
