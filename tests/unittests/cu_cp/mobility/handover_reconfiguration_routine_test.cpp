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

#include "lib/cu_cp/routines/mobility/handover_reconfiguration_routine.h"
#include "mobility_test_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/async/coroutine.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class handover_reconfiguration_routine_test : public mobility_test
{
protected:
  handover_reconfiguration_routine_test() {}

  void create_ues(bool procedure_outcome, unsigned transaction_id_)
  {
    ue_index_t source_ue_index = get_ue_manager()->allocate_new_ue_index(source_du_index);
    source_ue                  = get_ue_manager()->add_ue(source_ue_index, source_pci, source_rnti);
    ASSERT_NE(source_ue, nullptr);
    source_rrc_ue_notifier.set_transaction_id(transaction_id_);
    source_ue->set_rrc_ue_notifier(source_rrc_ue_notifier);

    ue_index_t target_ue_index = get_ue_manager()->allocate_new_ue_index(target_du_index);
    target_ue                  = get_ue_manager()->add_ue(target_ue_index, target_pci, target_rnti);
    ASSERT_NE(target_ue, nullptr);
    target_rrc_ue_notifier.set_rrc_reconfiguration_outcome(procedure_outcome);
    target_ue->set_rrc_ue_notifier(target_rrc_ue_notifier);
  }

  void start_procedure()
  {
    rrc_reconfiguration_procedure_request request;

    t = launch_async<handover_reconfiguration_routine>(request, *source_ue, *target_ue, test_logger);
    t_launcher.emplace(t);
  }

  bool procedure_ready() const { return t.ready(); }

  const bool get_result() { return t.get(); }

  bool check_transaction_id(unsigned transaction_id)
  {
    return target_rrc_ue_notifier.last_transaction_id == transaction_id;
  }

private:
  // source UE parameters.
  du_index_t                                         source_du_index = uint_to_du_index(0);
  unsigned                                           source_pci      = 1;
  rnti_t                                             source_rnti     = to_rnti(0x4601);
  dummy_du_processor_rrc_ue_control_message_notifier source_rrc_ue_notifier;
  du_ue*                                             source_ue = nullptr;

  // target UE parameters.
  du_index_t                                         target_du_index = uint_to_du_index(1);
  unsigned                                           target_pci      = 2;
  rnti_t                                             target_rnti     = to_rnti(0x4601);
  dummy_du_processor_rrc_ue_control_message_notifier target_rrc_ue_notifier;
  du_ue*                                             target_ue = nullptr;

  async_task<bool>                   t;
  optional<lazy_task_launcher<bool>> t_launcher;
};

TEST_F(handover_reconfiguration_routine_test, when_reconfiguration_successful_then_return_true)
{
  unsigned transaction_id = 99;

  // Test Preamble.
  create_ues(true, transaction_id);

  // it should be ready immediately
  start_procedure();

  ASSERT_TRUE(procedure_ready());

  // Reconfiguration complete was received.
  ASSERT_TRUE(get_result());

  ASSERT_TRUE(check_transaction_id(transaction_id));
}

TEST_F(handover_reconfiguration_routine_test, when_reconfiguration_unsuccessful_then_return_false)
{
  unsigned transaction_id = 17;

  // Test Preamble.
  create_ues(false, transaction_id);

  // it should be ready immediately
  start_procedure();

  ASSERT_TRUE(procedure_ready());

  // Reconfiguration complete was received.
  ASSERT_FALSE(get_result());

  ASSERT_TRUE(check_transaction_id(transaction_id));
}
