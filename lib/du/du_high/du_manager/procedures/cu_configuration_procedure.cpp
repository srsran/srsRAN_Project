/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_configuration_procedure.h"
#include "../du_cell_manager.h"
#include "../du_ue/du_ue_manager.h"
#include "srsran/support/async/async_no_op_task.h"
#include "srsran/support/async/async_timer.h"

using namespace srsran;
using namespace srs_du;

cu_configuration_procedure::cu_configuration_procedure(const gnbcu_config_update_request& request_,
                                                       du_cell_manager&                   cell_mng_,
                                                       du_ue_manager&                     ue_mng_,
                                                       const du_manager_params&           du_params_) :
  request(request_), cell_mng(cell_mng_), ue_mng(ue_mng_), du_params(du_params_)
{
}

void cu_configuration_procedure::operator()(coro_context<async_task<gnbcu_config_update_response>>& ctx)
{
  CORO_BEGIN(ctx);

  // Deactivate cells.
  for (list_index = 0; list_index != request.cells_to_deactivate.size(); ++list_index) {
    CORO_AWAIT(stop_cell(request.cells_to_deactivate[list_index]));
  }

  // Activate cells.
  for (list_index = 0; list_index != request.cells_to_activate.size(); ++list_index) {
    CORO_AWAIT_VALUE(bool success, start_cell(request.cells_to_activate[list_index].cgi));
    if (not success) {
      // Failed to start cell.
      resp.cells_failed_to_activate.push_back(request.cells_to_activate[list_index].cgi);
    }
  }

  CORO_RETURN(resp);
}

async_task<bool> cu_configuration_procedure::start_cell(const nr_cell_global_id_t& cgi)
{
  const du_cell_index_t cell_index = cell_mng.get_cell_index(cgi);
  if (cell_index == INVALID_DU_CELL_INDEX) {
    return launch_no_op_task(false);
  }
  return cell_mng.start(cell_index);
}

async_task<void> force_ue_release(du_ue_manager& ue_mng, du_cell_index_t cell_index)
{
  std::vector<du_ue_index_t> ues_to_force_rem;
  for (const auto& u : ue_mng.get_du_ues()) {
    if (u.pcell_index == cell_index) {
      ues_to_force_rem.push_back(u.ue_index);
    }
  }
  if (ues_to_force_rem.empty()) {
    return launch_no_op_task();
  }
  return ue_mng.handle_f1_reset_request(ues_to_force_rem);
}

async_task<void> cu_configuration_procedure::stop_cell(const nr_cell_global_id_t& cgi)
{
  const du_cell_index_t cell_index = cell_mng.get_cell_index(cgi);

  bool ues_need_to_be_released = false;

  // Check if there are still UEs attached to this cell that need to be released.
  for (const auto& u : ue_mng.get_du_ues()) {
    if (u.pcell_index == cell_index) {
      // UE PCell matches the cell being deactivated. Request UE release.
      du_params.f1ap.ue_mng.handle_ue_context_release_request(
          f1ap_ue_context_release_request{u.ue_index, f1ap_ue_context_release_request::cause_type::other});

      // Flag that UEs need to be released.
      ues_need_to_be_released = true;
    }
  }

  if (not ues_need_to_be_released) {
    // We can stop the cell immediately.
    return cell_mng.stop(cell_index);
  }

  static const std::chrono::milliseconds periodic_check_timeout{10};
  static const unsigned                  nof_checks = std::chrono::milliseconds{500} / periodic_check_timeout;
  unique_timer timer = du_params.services.timers.create_unique_timer(du_params.services.du_mng_exec);
  return launch_async(
      [this, cell_index, timer = std::move(timer), count = 0U](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        for (; count != nof_checks; ++count) {
          CORO_AWAIT(async_wait_for(timer, periodic_check_timeout));

          bool ues_exist = false;
          for (const auto& u : ue_mng.get_du_ues()) {
            if (u.pcell_index == cell_index) {
              // There are still UEs connected to the cell.
              ues_exist = true;
              break;
            }
          }
          if (not ues_exist) {
            break;
          }
        }

        if (count == nof_checks) {
          // Timeout reached and there are still UEs attached to the cell. Force their removal.
          CORO_AWAIT(force_ue_release(ue_mng, cell_index));
        }

        // Stop cell.
        CORO_AWAIT(du_params.mac.cell_mng.get_cell_controller(cell_index).stop());

        CORO_RETURN();
      });
}
