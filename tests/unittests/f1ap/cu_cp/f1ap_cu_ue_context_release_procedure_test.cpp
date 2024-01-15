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

#include "f1ap_cu_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Test the f1 UE context release procedure (gNB-CU initiated)
TEST_F(f1ap_cu_test, when_ue_release_command_received_then_procedure_succeeds)
{
  // Action 1: Add UE
  test_logger.info("Injecting Initial UL RRC message");
  f1ap_message init_ul_rrc_msg = generate_init_ul_rrc_message_transfer(int_to_gnb_du_ue_f1ap_id(41255));
  f1ap->handle_message(init_ul_rrc_msg);

  ASSERT_EQ(f1ap->get_nof_ues(), 1);

  // Action 2: Start UE Context Release procedure
  test_logger.info("Starting UE Context Release procedure");
  f1ap_ue_context_release_command f1ap_ue_ctxt_rel_cmd_msg;
  f1ap_ue_ctxt_rel_cmd_msg.ue_index = ue_index_t::min;
  f1ap_ue_ctxt_rel_cmd_msg.cause    = cause_radio_network_t::unspecified;

  // launch F1 UE context release procedure
  async_task<ue_index_t>         t = f1ap->handle_ue_context_release_command(f1ap_ue_ctxt_rel_cmd_msg);
  lazy_task_launcher<ue_index_t> t_launcher(t);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  // Action 3: Inject UE Context Release Complete message
  test_logger.info("Injecting UE Context Release Complete message");
  f1ap_message ue_ctxt_rel_complete_msg =
      generate_ue_context_release_complete(int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(41255));
  f1ap->handle_message(ue_ctxt_rel_complete_msg);

  ASSERT_TRUE(t.ready());
  ASSERT_EQ(t.get(), ue_index_t::min);
}
