/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_routine_manager_test_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class ue_context_release_test : public cu_cp_routine_manager_test
{
protected:
  void start_procedure(const cu_cp_ue_context_release_command& msg,
                       ue_context_outcome_t                    ue_context_modification_outcome,
                       bearer_context_outcome_t                bearer_context_modification_outcome)
  {
    f1ap_ue_ctxt_mng.set_ue_context_modification_outcome(ue_context_modification_outcome);
    e1ap_bearer_ctxt_mng.set_second_message_outcome(bearer_context_modification_outcome);

    t = routine_mng->start_ue_context_release_routine(msg, &e1ap_bearer_ctxt_mng, f1ap_ue_ctxt_mng, ue_removal_handler);
    t_launcher.emplace(t);
  }

  ue_index_t add_ue(pci_t pci, rnti_t c_rnti)
  {
    ue_index_t ue_index = ue_mng.add_ue(du_index_t::min);
    du_ue*     ue       = ue_mng.set_ue_du_context(ue_index, int_to_gnb_du_id(0), pci, c_rnti);
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
  ue_index_t ue_index = add_ue(MIN_PCI, rnti_t::MIN_CRNTI);

  // Generate UE context release command message
  cu_cp_ue_context_release_command ue_context_release_command = generate_ue_context_release_command(ue_index);
  this->start_procedure(ue_context_release_command, {true}, {true});

  // nothing has failed to be released
  ASSERT_TRUE(was_ue_context_release_successful());
}