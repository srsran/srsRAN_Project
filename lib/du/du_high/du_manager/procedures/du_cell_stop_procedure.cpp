/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "du_cell_stop_procedure.h"
#include "../du_cell_manager.h"
#include "../du_ue/du_ue_manager.h"
#include "du_ue_reset_procedure.h"
#include "srsran/support/async/async_no_op_task.h"
#include "srsran/support/async/async_timer.h"

using namespace srsran;
using namespace srs_du;

du_cell_stop_procedure::du_cell_stop_procedure(du_ue_manager&           ue_mng_,
                                               du_cell_manager&         cell_mng_,
                                               const du_manager_params& du_params_,
                                               du_cell_index_t          cell_index_,
                                               ue_removal_mode          mode_) :
  ue_mng(ue_mng_),
  cell_mng(cell_mng_),
  du_params(du_params_),
  cell_index(cell_index_),
  mode(mode_),
  proc_logger(srslog::fetch_basic_logger("DU-MNG"), "DU Cell Stop"),
  timer(du_params.services.timers.create_unique_timer(du_params.services.du_mng_exec))
{
}

void du_cell_stop_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  proc_logger.log_proc_started();

  if (not cell_mng.is_cell_active(cell_index)) {
    // Cell is not active. Nothing to do.
    CORO_EARLY_RETURN();
  }

  // Check if there are still UEs attached to this cell that need to be released.
  CORO_AWAIT(rem_ues_with_matching_pcell());

  // Stop cell.
  CORO_AWAIT(cell_mng.stop(cell_index));

  // Wait for some slots to ensure all pending grants were flushed.
  // TODO: Move this wait to the MAC once the FAPI stop procedure is supported.
  static const std::chrono::milliseconds mac_grant_flush_timeout{50};
  CORO_AWAIT(async_wait_for(timer, mac_grant_flush_timeout));

  proc_logger.log_proc_completed();

  CORO_RETURN();
}

static void fill_ues_to_rem(std::vector<du_ue_index_t>& ues_to_rem, du_ue_manager& ue_mng, du_cell_index_t cell_index)
{
  auto& ue_db = ue_mng.get_du_ues();
  ues_to_rem.clear();
  ues_to_rem.reserve(ue_db.size());
  for (const auto& u : ue_db) {
    if (u.pcell_index == cell_index) {
      ues_to_rem.push_back(u.ue_index);
    }
  }
}

async_task<void> du_cell_stop_procedure::force_ue_removal()
{
  fill_ues_to_rem(ues_to_rem, ue_mng, cell_index);
  if (ues_to_rem.empty()) {
    // There are no UEs associated with this cell. Nothing to do.
    return launch_no_op_task();
  }
  std::optional<f1_reset_request::cause_type> cause;
  if (mode == ue_removal_mode::trigger_f1_reset) {
    cause = f1_reset_request::cause_type::cell_removal;
  }
  return launch_async<du_ue_reset_procedure>(ues_to_rem, ue_mng, du_params, cause);
}

async_task<void> du_cell_stop_procedure::rem_ues_with_matching_pcell()
{
  // Stop accepting new UEs in this cell.
  cell_mng.stop_accepting_ues(cell_index);

  if (mode == ue_removal_mode::trigger_f1_ue_release_request) {
    // Wait for UEs to be released by the CU.
    return await_cu_to_release_ues();
  }

  // Use F1 Reset procedure directly.
  return force_ue_removal();
}

async_task<void> du_cell_stop_procedure::await_cu_to_release_ues()
{
  // Implementation-defined timeout for graceful UE removal. After which, we will force UE removal.
  static const std::chrono::milliseconds periodic_check_timeout{10};
  static const unsigned                  nof_checks = std::chrono::milliseconds{500} / periodic_check_timeout;

  return launch_async([this, count = 0U](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    fill_ues_to_rem(ues_to_rem, ue_mng, cell_index);
    while (not ues_to_rem.empty()) {
      // There are UEs associated with this cell that need to be released.

      // > Request CU to release UEs via F1 UE CONTEXT RELEASE REQUEST.
      for (du_ue_index_t ue_idx : ues_to_rem) {
        du_params.f1ap.ue_mng.handle_ue_context_release_request(
            f1ap_ue_context_release_request{ue_idx, f1ap_ue_context_release_request::cause_type::other});
      }

      // > Wait for all UEs to be released.
      for (; count != nof_checks; ++count) {
        CORO_AWAIT(async_wait_for(timer, periodic_check_timeout));

        bool ues_exist = false;
        for (du_ue_index_t ue_idx : ues_to_rem) {
          if (ue_mng.find_ue(ue_idx) != nullptr) {
            // There are still UEs connected to the cell.
            ues_exist = true;
            break;
          }
        }
        if (not ues_exist) {
          // All previously requested UEs to be released have been released. We will check if there are other UEs
          // that were created in the meantime and, if so, repeat the process.
          fill_ues_to_rem(ues_to_rem, ue_mng, cell_index);
          break;
        }
      }

      if (count == nof_checks) {
        // Timeout reached and there are still UEs attached to the cell. Force their removal.
        CORO_AWAIT(force_ue_removal());
      }
    }

    CORO_RETURN();
  });
}
