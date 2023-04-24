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
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Note: Check if UE ID is valid is done by caller. Injection of invalid ue_index results in assertion.

class pdu_session_resource_setup_test : public du_processor_routine_manager_test
{
protected:
  void start_procedure(const cu_cp_pdu_session_resource_setup_request& msg,
                       bearer_context_setup_outcome_t                  bearer_context_setup_outcome,
                       bool                                            ue_context_modification_outcome,
                       bool                                            bearer_context_modification_outcome,
                       bool                                            rrc_reconfiguration_outcome)
  {
    e1ap_ctrl_notifier.set_bearer_context_setup_outcome(bearer_context_setup_outcome);
    f1ap_ue_ctxt_notifier.set_ue_context_modification_outcome(ue_context_modification_outcome);
    e1ap_ctrl_notifier.set_bearer_context_modification_outcome(bearer_context_modification_outcome);
    rrc_ue_ctrl_notifier.set_rrc_reconfiguration_outcome(rrc_reconfiguration_outcome);

    t = routine_mng->start_pdu_session_resource_setup_routine(
        msg, security_cfg, rrc_ue_ctrl_notifier, *rrc_ue_drb_manager);
    t_launcher.emplace(t);
  }

  bool was_pdu_session_resource_setup_successful() const
  {
    if (not t.ready()) {
      return false;
    }

    if (t.get().pdu_session_res_failed_to_setup_items.size() > 0) {
      return false;
    }

    return t.get().pdu_session_res_setup_response_items.size() > 0;
  }

  size_t num_pdu_session_res_failed_to_setup_size() { return t.get().pdu_session_res_failed_to_setup_items.size(); }

  size_t num_pdu_session_res_setup_size() { return t.get().pdu_session_res_setup_response_items.size(); }

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
  bearer_context_setup_outcome_t bearer_context_setup_outcome{false, {}, {}};
  this->start_procedure(request, bearer_context_setup_outcome, false, false, false);

  // nothing has failed to setup
  ASSERT_FALSE(was_pdu_session_resource_setup_successful());
}

TEST_F(pdu_session_resource_setup_test, when_bearer_context_setup_failure_received_then_setup_fails)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup();

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_setup_outcome_t bearer_context_setup_outcome{false, {}, {}};
  this->start_procedure(request, bearer_context_setup_outcome, false, false, false);

  // nothing has failed to setup
  ASSERT_FALSE(was_pdu_session_resource_setup_successful());
}

TEST_F(pdu_session_resource_setup_test, when_ue_context_modification_failure_received_then_setup_fails)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup();

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_setup_outcome_t bearer_context_setup_outcome{true, {1}, {}};
  this->start_procedure(request, bearer_context_setup_outcome, false, false, false);

  // nothing has failed to setup
  ASSERT_FALSE(was_pdu_session_resource_setup_successful());
}

TEST_F(pdu_session_resource_setup_test, when_bearer_context_modification_failure_received_then_setup_fails)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup();

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_setup_outcome_t bearer_context_setup_outcome{true, {1}, {}};
  this->start_procedure(request, bearer_context_setup_outcome, true, false, false);

  // nothing has failed to setup
  ASSERT_FALSE(was_pdu_session_resource_setup_successful());
}

TEST_F(pdu_session_resource_setup_test, when_rrc_reconfiguration_fails_then_setup_fails)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup();

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_setup_outcome_t bearer_context_setup_outcome{true, {1}, {}};
  this->start_procedure(request, bearer_context_setup_outcome, true, true, false);

  // nothing has failed to setup
  ASSERT_FALSE(was_pdu_session_resource_setup_successful());
}

TEST_F(pdu_session_resource_setup_test, when_rrc_reconfiguration_succeeds_then_setup_succeeds)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup();

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_setup_outcome_t bearer_context_setup_outcome{true, {1}, {}};
  this->start_procedure(request, bearer_context_setup_outcome, true, true, true);

  // nothing has failed to setup
  ASSERT_TRUE(was_pdu_session_resource_setup_successful());
}

TEST_F(pdu_session_resource_setup_test, when_pdu_session_setup_for_existing_session_arrives_then_setup_fails)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup();

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_setup_outcome_t bearer_context_setup_outcome{true, {1}, {}};
  this->start_procedure(request, bearer_context_setup_outcome, true, true, true);

  // nothing has failed to setup
  ASSERT_TRUE(was_pdu_session_resource_setup_successful());

  // Inject same PDU session resource setup again.
  this->start_procedure(request, bearer_context_setup_outcome, true, true, true);

  // 2nd setup attempt failed.
  ASSERT_FALSE(was_pdu_session_resource_setup_successful());
}

/// Test handling of PDU session setup request without any setup item.
TEST_F(pdu_session_resource_setup_test, when_empty_pdu_session_setup_request_received_then_setup_fails)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = {}; // empty message
  request.ue_index                                 = uint_to_ue_index(0);

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_setup_outcome_t bearer_context_setup_outcome{true, {}, {}};
  this->start_procedure(request, bearer_context_setup_outcome, true, true, true);

  // it should be ready immediately
  ASSERT_TRUE(t.ready());

  // Nothing has been set up or failed
  ASSERT_FALSE(was_pdu_session_resource_setup_successful());
}

/// One PDU session with two QoS flows.
TEST_F(pdu_session_resource_setup_test, when_setup_for_pdu_sessions_with_two_qos_flows_received_setup_succeeds)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup(1, 2);

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_setup_outcome_t bearer_context_setup_outcome{true, {1}, {}}; // first session success, second failed
  this->start_procedure(request, bearer_context_setup_outcome, true, true, true);

  // it should be ready immediately
  ASSERT_TRUE(t.ready());

  // One PDU session failed to be setup.
  ASSERT_EQ(num_pdu_session_res_failed_to_setup_size(), 0);

  // One PDU session succeeded to be setup.
  ASSERT_EQ(num_pdu_session_res_setup_size(), 1);
}

// Test with multiple PDU sessions.
TEST_F(pdu_session_resource_setup_test,
       when_setup_for_two_pdu_sessions_is_requested_but_only_first_could_be_setup_at_cuup_setup_succeeds_with_fail_list)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup(2);

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_setup_outcome_t bearer_context_setup_outcome{true, {1}, {2}}; // first session success, second failed
  this->start_procedure(request, bearer_context_setup_outcome, true, true, true);

  // it should be ready immediately
  ASSERT_TRUE(t.ready());

  // One PDU session failed to be setup.
  ASSERT_EQ(num_pdu_session_res_failed_to_setup_size(), 1);

  // One PDU session succeeded to be setup.
  ASSERT_EQ(num_pdu_session_res_setup_size(), 1);
}

TEST_F(pdu_session_resource_setup_test, when_setup_for_two_pdu_sessions_is_requested_and_both_succeed_setup_succeeds)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_setup_request request = generate_pdu_session_resource_setup(2);

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_setup_outcome_t bearer_context_setup_outcome{true, {1, 2}, {}}; // first session success, second failed
  this->start_procedure(request, bearer_context_setup_outcome, true, true, true);

  // it should be ready immediately
  ASSERT_TRUE(t.ready());

  // Zero PDU session failed to be setup.
  ASSERT_EQ(num_pdu_session_res_failed_to_setup_size(), 0);

  // Two PDU session succeeded to be setup.
  ASSERT_EQ(num_pdu_session_res_setup_size(), 2);
}
