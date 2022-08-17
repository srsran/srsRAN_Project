/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../common_test_helpers/f1_cu_test_helpers.h"
#include "srsgnb/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;

/// Test the f1 UE context release procedure (gNB-CU initiated)
TEST_F(f1ap_cu_test, when_ue_release_command_received_then_ue_removed)
{
  // Action 1: Add UE
  test_logger.info("Injecting Initial UL RRC message");
  f1c_message init_ul_rrc_msg = generate_valid_f1_init_ul_rrc_msg(41255);
  f1ap->handle_message(init_ul_rrc_msg);

  EXPECT_EQ(f1ap->get_nof_ues(), 1);

  // Action 2: Start UE Context Release procedure
  test_logger.info("Starting UE Context Release procedure");
  f1ap_ue_context_release_command_message f1ap_ue_ctxt_rel_cmd_msg;
  f1ap_ue_ctxt_rel_cmd_msg.ue_index = MIN_UE_INDEX;
  f1ap_ue_ctxt_rel_cmd_msg.cause.set_radio_network();

  // launch F1 UE context release procedure
  async_task<ue_index_t>         t = f1ap->handle_ue_context_release_command(f1ap_ue_ctxt_rel_cmd_msg);
  lazy_task_launcher<ue_index_t> t_launcher(t);

  // Status: Procedure not yet ready.
  EXPECT_FALSE(t.ready());

  // Action 3: Inject UE Context Release Complete message
  test_logger.info("Injecting UE Context Release Complete message");
  f1c_message ue_ctxt_rel_complete_msg = generate_f1_ue_context_release_complete_msg(0, 41255);
  f1ap->handle_message(ue_ctxt_rel_complete_msg);

  EXPECT_TRUE(t.ready());
  EXPECT_EQ(t.get(), MIN_UE_INDEX);

  EXPECT_EQ(f1ap->get_nof_ues(), 0);
}
