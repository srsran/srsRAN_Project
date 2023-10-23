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
#include "du_manager_procedure_test_helpers.h"
#include "lib/du_manager/procedures/ue_creation_procedure.h"
#include "lib/du_manager/procedures/ue_deletion_procedure.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

class ue_deletion_tester : public du_manager_proc_tester, public ::testing::Test
{
protected:
  ue_deletion_tester() :
    du_manager_proc_tester(std::vector<du_cell_config>{config_helpers::make_default_du_cell_config()})
  {
    // Create UE.
    test_ue = &create_ue(to_du_ue_index(test_rgen::uniform_int<unsigned>(0, MAX_DU_UE_INDEX)));

    // Run UE Configuration Procedure to completion.
    configure_ue(create_f1ap_ue_context_update_request(test_ue->ue_index, {srb_id_t::srb2}, {drb_id_t::drb1}));
  }

  void start_procedure()
  {
    f1ap_ue_delete_request msg;
    msg.ue_index = test_ue->ue_index;
    proc         = launch_async<ue_deletion_procedure>(msg, ue_mng, params);
    proc_launcher.emplace(proc);
  }

  du_ue*                             test_ue = nullptr;
  async_task<void>                   proc;
  optional<lazy_task_launcher<void>> proc_launcher;
};

TEST_F(ue_deletion_tester, when_du_manager_receives_ue_delete_request_then_f1ap_and_mac_get_request_to_delete_ue)
{
  start_procedure();

  // UE deletion started and completed in F1AP.
  ASSERT_TRUE(this->f1ap.last_ue_deleted.has_value());
  ASSERT_EQ(this->f1ap.last_ue_deleted.value(), test_ue->ue_index);

  // Check MAC received request to delete UE with valid params.
  ASSERT_TRUE(this->mac.last_ue_delete_msg.has_value());
  EXPECT_EQ(this->mac.last_ue_delete_msg->ue_index, test_ue->ue_index);
}

TEST_F(ue_deletion_tester,
       when_du_manager_starts_ue_deletion_procedure_then_it_waits_for_mac_completion_before_finishing_procedure)
{
  start_procedure();

  // Check MAC received request to delete UE but DU manager is waiting for MAC completion before deleting UE from F1AP.
  ASSERT_TRUE(this->mac.last_ue_delete_msg.has_value());
  ASSERT_FALSE(proc.ready());

  // MAC returns response to delete UE.
  // Note: UE is going to be removed, so we save its index locally.
  this->mac.wait_ue_delete.result.result = true;
  this->mac.wait_ue_delete.ready_ev.set();

  // UE deletion procedure should have finished at this point.
  ASSERT_TRUE(proc.ready());
}

TEST_F(ue_deletion_tester, when_du_manager_is_removing_ue_from_mac_then_rlc_buffer_states_have_no_effect)
{
  // RLC buffer state updates of DRBs should reach the MAC.
  ASSERT_FALSE(mac.last_dl_bs.has_value());
  test_ue->bearers.drbs().at(drb_id_t::drb1)->connector.rlc_tx_buffer_state_notif.on_buffer_state_update(10);
  ASSERT_TRUE(mac.last_dl_bs.has_value());
  ASSERT_EQ(mac.last_dl_bs->ue_index, test_ue->ue_index);
  ASSERT_EQ(mac.last_dl_bs->lcid, lcid_t::LCID_MIN_DRB);
  ASSERT_EQ(mac.last_dl_bs->bs, 10);

  // RLC buffer state updates of SRBs should reach the MAC.
  mac.last_dl_bs.reset();
  ASSERT_FALSE(mac.last_dl_bs.has_value());
  test_ue->bearers.srbs()[srb_id_t::srb1].connector.rlc_tx_buffer_state_notif.on_buffer_state_update(100);
  ASSERT_TRUE(mac.last_dl_bs.has_value());
  ASSERT_EQ(mac.last_dl_bs->ue_index, test_ue->ue_index);
  ASSERT_EQ(mac.last_dl_bs->lcid, LCID_SRB1);
  ASSERT_EQ(mac.last_dl_bs->bs, 100);

  start_procedure();

  // RLC buffer state updates should not reach the MAC while UE is being removed.
  mac.last_dl_bs.reset();
  test_ue->bearers.drbs().at(drb_id_t::drb1)->connector.rlc_tx_buffer_state_notif.on_buffer_state_update(10);
  ASSERT_FALSE(mac.last_dl_bs.has_value());
  test_ue->bearers.srbs()[srb_id_t::srb1].connector.rlc_tx_buffer_state_notif.on_buffer_state_update(10);
  ASSERT_FALSE(mac.last_dl_bs.has_value());
}

TEST_F(ue_deletion_tester, when_du_manager_is_removing_ue_from_mac_then_rlf_notifications_have_no_effect)
{
  rlf_cause cause = static_cast<rlf_cause>(test_rgen::uniform_int<unsigned>(0, 2));

  // RLF notification should be handled.
  if (cause == rlf_cause::max_mac_kos_reached) {
    test_ue->rlf_notifier->on_rlf_detected();
  } else if (cause == rlf_cause::max_rlc_retxs_reached) {
    test_ue->rlf_notifier->on_max_retx();
  } else {
    test_ue->rlf_notifier->on_protocol_failure();
  }
  const unsigned release_timeout =
      (du_cells[0].ue_timers_and_constants.t310 + du_cells[0].ue_timers_and_constants.t311).count();
  for (unsigned i = 0; i != release_timeout; ++i) {
    timers.tick();
    worker.run_pending_tasks();
  }
  ASSERT_EQ(ue_mng.last_rlf_ue_index, test_ue->ue_index);
  ASSERT_EQ(ue_mng.last_rlf_cause, cause);

  // RLFs have no effect after the RLF timer is triggered.
  ue_mng.last_rlf_ue_index.reset();
  ue_mng.last_rlf_cause.reset();
  test_ue->rlf_notifier->on_max_retx();
  for (unsigned i = 0; i != release_timeout; ++i) {
    timers.tick();
    worker.run_pending_tasks();
  }
  ASSERT_FALSE(ue_mng.last_rlf_ue_index.has_value());
  ASSERT_FALSE(ue_mng.last_rlf_cause.has_value());

  // Start UE deletion.
  start_procedure();

  // RLF notifications should not be handled.
  test_ue->rlf_notifier->on_rlf_detected();
  for (unsigned i = 0; i != release_timeout; ++i) {
    timers.tick();
    worker.run_pending_tasks();
  }
  ASSERT_FALSE(ue_mng.last_rlf_cause.has_value());
  ASSERT_FALSE(ue_mng.last_rlf_ue_index.has_value());
}
