/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/support/async/async_test_utils.h"
#include "unittests/e1_interface/common/e1_cu_cp_test_helpers.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;

/// Test the E1 bearer context release procedure (CU-CP initiated)
TEST_F(e1_cu_cp_test, when_bearer_release_complete_received_then_bearer_realease_successful)
{
  // Action 1: Launch Bearer Context release procedure
  e1ap_bearer_context_release_command_message release_cmd_msg = {};
  test_logger.info("Launch bearer context release procedure...");
  async_task<e1ap_bearer_context_release_complete_message> t =
      e1->handle_bearer_context_release_command(release_cmd_msg);
  lazy_task_launcher<e1ap_bearer_context_release_complete_message> t_launcher(t);

  // Status: CU-UP received Bearer Context Release Command message.
  EXPECT_EQ(msg_notifier->last_e1_msg.pdu.type().value, asn1::e1ap::e1_ap_pdu_c::types_opts::init_msg);
  EXPECT_EQ(msg_notifier->last_e1_msg.pdu.init_msg().value.type().value,
            asn1::e1ap::e1_ap_elem_procs_o::init_msg_c::types_opts::bearer_context_release_cmd);

  // Status: Procedure not yet ready.
  EXPECT_FALSE(t.ready());

  // Action 2: E1 Bearer Context Release Complete received.
  e1_message bearer_context_release_complete = generate_bearer_context_release_complete_msg(1U, 2U);
  test_logger.info("Injecting BearerContextreleaseComplete");
  e1->handle_message(bearer_context_release_complete);

  EXPECT_TRUE(t.ready());
  EXPECT_EQ(t.get().msg->gnb_cu_cp_ue_e1_ap_id.value, 1U);
}
