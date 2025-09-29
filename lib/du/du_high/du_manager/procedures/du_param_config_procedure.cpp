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

#include "du_param_config_procedure.h"
#include "../converters/f1ap_configuration_helpers.h"
#include "../converters/scheduler_configuration_helpers.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/async/async_no_op_task.h"

using namespace srsran;
using namespace srs_du;

du_param_config_procedure::du_param_config_procedure(const du_param_config_request& request_,
                                                     const du_manager_params&       du_params_,
                                                     du_cell_manager&               du_cells_) :
  request(request_), du_params(du_params_), du_cells(du_cells_), logger(srslog::fetch_basic_logger("DU-MNG"))
{
}

void du_param_config_procedure::operator()(coro_context<async_task<du_param_config_response>>& ctx)
{
  CORO_BEGIN(ctx);

  // Update DU cell configs.
  if (not handle_cell_config_updates()) {
    resp.success = false;
    CORO_EARLY_RETURN(resp);
  }

  for (; next_cell_idx != changed_cells.size(); ++next_cell_idx) {
    // Reconfigure cell in the MAC.
    CORO_AWAIT_VALUE(mac_cell_reconfig_response macresp, handle_mac_cell_update(changed_cells[next_cell_idx]));
    if (changed_cells[next_cell_idx].sched_notif_required and not macresp.si_updated) {
      resp.success = false;
      CORO_EARLY_RETURN(resp);
    }
  }

  // Initiate F1AP gNB-DU Configuration Update.
  CORO_AWAIT_VALUE(gnbdu_config_update_response f1resp, handle_f1_gnbdu_config_update());

  resp.success = f1resp.result;
  CORO_RETURN(resp);
}

bool du_param_config_procedure::handle_cell_config_updates()
{
  for (const du_cell_param_config_request& cell_to_update : request.cells) {
    auto result = du_cells.handle_cell_reconf_request(cell_to_update);
    if (not result.has_value()) {
      continue;
    }

    // Mark that cell was changed.
    changed_cells.push_back(result.value());
  }

  return not changed_cells.empty();
}

async_task<gnbdu_config_update_response> du_param_config_procedure::handle_f1_gnbdu_config_update()
{
  gnbdu_config_update_request f1_req{};

  f1_req.cells_to_mod.reserve(changed_cells.size());
  for (const auto& changed_cell : changed_cells) {
    if (changed_cell.cu_notif_required) {
      const du_cell_config& cell_cfg = du_cells.get_cell_cfg(changed_cell.cell_index);
      auto&                 cell     = f1_req.cells_to_mod.emplace_back();
      cell.cell_info                 = make_f1ap_du_cell_info(cell_cfg);
      cell.du_sys_info               = make_f1ap_du_sys_info(cell_cfg);
    }
  }

  if (f1_req.cells_to_mod.empty()) {
    return launch_no_op_task(gnbdu_config_update_response{true});
  }

  return du_params.f1ap.conn_mng.handle_du_config_update(f1_req);
}

async_task<mac_cell_reconfig_response>
du_param_config_procedure::handle_mac_cell_update(const du_cell_reconfig_result& changed_cell)
{
  if (not changed_cell.sched_notif_required and not changed_cell.slice_reconf_req.has_value()) {
    return launch_no_op_task(mac_cell_reconfig_response{});
  }

  mac_cell_reconfig_request req;

  if (changed_cell.sched_notif_required) {
    // Update SIB1 and SI message content.
    req.new_sys_info = du_cells.get_sys_info(changed_cell.cell_index);
  }

  if (changed_cell.slice_reconf_req.has_value()) {
    req.slice_reconf_req.emplace(changed_cell.slice_reconf_req.value());
  }

  return du_params.mac.cell_mng.get_cell_controller(changed_cell.cell_index).reconfigure(req);
}
