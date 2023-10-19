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
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class ue_context_release_test : public du_processor_routine_manager_test
{
protected:
  void start_procedure(const cu_cp_ue_context_release_command& msg,
                       ue_context_outcome_t                    ue_context_modification_outcome,
                       bearer_context_outcome_t                bearer_context_modification_outcome)
  {
    f1ap_ue_ctxt_notifier.set_ue_context_modification_outcome(ue_context_modification_outcome);
    e1ap_ctrl_notifier.set_second_message_outcome(bearer_context_modification_outcome);

    t = routine_mng->start_ue_context_release_routine(msg, *cu_cp_notifier);
    t_launcher.emplace(t);
  }

  ue_index_t add_ue(pci_t pci, rnti_t c_rnti)
  {
    ue_index_t ue_index = ue_mng.allocate_new_ue_index(du_index_t::min);
    du_ue*     ue       = ue_mng.add_ue(ue_index, pci, c_rnti);
    // Set parameters from creation message
    ue->set_pcell_index(du_cell_index_t::min);
    ue->set_rrc_ue_notifier(rrc_ue_ctrl_notifier);
    ue->set_rrc_ue_srb_notifier(rrc_ue_srb_ctrl_notifier);

    return ue_index;
  }

  bool was_ue_context_release_successful()
  {
    if (not t.ready()) {
      return false;
    }

    if (t.get().ue_index == ue_index_t::invalid) {
      return false;
    }

    if (ue_mng.get_nof_ues() != 0) {
      return false;
    }

    return true;
  }

  async_task<cu_cp_ue_context_release_complete>                   t;
  optional<lazy_task_launcher<cu_cp_ue_context_release_complete>> t_launcher;
};

TEST_F(ue_context_release_test, when_ue_context_release_command_received_then_release_succeeds)
{
  // Add UE
  ue_index_t ue_index = add_ue(MIN_PCI, MIN_CRNTI);

  // Generate UE context release command message
  cu_cp_ue_context_release_command ue_context_release_command = generate_ue_context_release_command(ue_index);
  this->start_procedure(ue_context_release_command, {true}, {true});

  // nothing has failed to be released
  ASSERT_TRUE(was_ue_context_release_successful());
}
