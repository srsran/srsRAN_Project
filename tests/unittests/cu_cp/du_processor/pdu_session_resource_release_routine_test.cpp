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
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Note: Check if UE ID is valid is done by caller. Injection of invalid ue_index results in assertion.

class pdu_session_resource_release_test : public du_processor_routine_manager_test
{
protected:
  void start_procedure(const cu_cp_pdu_session_resource_release_command& msg,
                       bool                                              ue_context_modification_outcome,
                       bearer_context_modification_outcome_t             bearer_context_modification_outcome)
  {
    f1ap_ue_ctxt_notifier.set_ue_context_modification_outcome(ue_context_modification_outcome);
    e1ap_ctrl_notifier.set_second_message_outcome(bearer_context_modification_outcome);

    t = routine_mng->start_pdu_session_resource_release_routine(msg, *rrc_ue_drb_manager);
    t_launcher.emplace(t);
  }

  bool was_pdu_session_resource_release_successful() const
  {
    if (not t.ready()) {
      return false;
    }

    if (t.get().pdu_session_res_released_list_rel_res.size() == 0) {
      return false;
    }

    return true;
  }

  async_task<cu_cp_pdu_session_resource_release_response>                   t;
  optional<lazy_task_launcher<cu_cp_pdu_session_resource_release_response>> t_launcher;
};

TEST_F(pdu_session_resource_release_test, when_ue_context_modification_failure_received_then_setup_succeeds)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_release_command command = generate_pdu_session_resource_release();

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_modification_outcome_t bearer_context_modification_outcome{false};
  this->start_procedure(command, true, bearer_context_modification_outcome);

  // nothing has failed to setup
  ASSERT_TRUE(was_pdu_session_resource_release_successful());
}

TEST_F(pdu_session_resource_release_test, when_bearer_context_modification_failure_received_then_setup_succeeds)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_release_command command = generate_pdu_session_resource_release();

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_modification_outcome_t bearer_context_modification_outcome{true};
  this->start_procedure(command, true, bearer_context_modification_outcome);

  // nothing has failed to setup
  ASSERT_TRUE(was_pdu_session_resource_release_successful());
}

/// Test handling of PDU session setup command without any setup item.
TEST_F(pdu_session_resource_release_test, when_empty_pdu_session_release_command_received_then_setup_fails)
{
  // Test Preamble.
  cu_cp_pdu_session_resource_release_command command = {}; // empty message
  command.ue_index                                   = uint_to_ue_index(0);

  // Start PDU SESSION RESOURCE SETUP routine.
  bearer_context_modification_outcome_t bearer_context_modification_outcome{true};
  this->start_procedure(command, true, bearer_context_modification_outcome);

  // it should be ready immediately
  ASSERT_TRUE(t.ready());

  // Nothing has been set up or failed
  ASSERT_FALSE(was_pdu_session_resource_release_successful());
}
