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
#include "srsran/srslog/srslog.h"
#include "srsran/support/async/async_no_op_task.h"

using namespace srsran;
using namespace srs_du;

static error_type<std::string> validate_du_param_config_request(const du_param_config_request& req)
{
  // There must be at least a cell config request.
  if (req.cells.empty()) {
    return make_unexpected("At least one cell configuration must be present");
  }

  for (const auto& cell : req.cells) {
    if (not cell.ssb_pwr_mod.has_value() and cell.rrm_policy_ratio_list.empty()) {
      return make_unexpected("Cell config requests must specify either SSB power mod. or rrm_policy_ratio_list");
    }
  }

  if (req.cells.size() == 1) {
    if (not req.cells.front().nr_cgi.has_value()) {
      // When the NR-CGI is not specified, then the request cannot be for SSB pwr modification.
      if (req.cells.front().ssb_pwr_mod.has_value()) {
        return make_unexpected("SSB power modification requires NR-CGI");
      }
    }
  }
  // If there are config requests for 2 or more cells, then each of them must specify the NR-CGI.
  else {
    for (const auto& cell : req.cells) {
      if (not cell.nr_cgi.has_value()) {
        return make_unexpected("Missing NR-CGI for one of the cell config requests");
      }
    }
  }

  return {};
}

du_param_config_procedure::du_param_config_procedure(const du_param_config_request& request_,
                                                     const du_manager_params&       du_params_,
                                                     du_cell_manager&               du_cells_) :
  request(request_),
  du_params(du_params_),
  du_cells(du_cells_),
  req_validation_outcome(validate_du_param_config_request(request)),
  logger(srslog::fetch_basic_logger("DU-MNG"))
{
}

void du_param_config_procedure::operator()(coro_context<async_task<du_param_config_response>>& ctx)
{
  CORO_BEGIN(ctx);

  if (not req_validation_outcome.has_value()) {
    logger.warning("Invalid DU parameters config request. Cause: {}", req_validation_outcome.error());
    resp.success = false;
    CORO_EARLY_RETURN(resp);
  }

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
  if (resp.success) {
    logger.info("DU reconfiguration procedure for {} completed",
                request.cells.front().ssb_pwr_mod.has_value() ? "SSB power modification" : "RRM policy update");
  }
  CORO_RETURN(resp);
}

bool du_param_config_procedure::handle_cell_config_updates()
{
  // When the request has no NR-CGI set, then it is for the RRC policy update to be propagated to all cells.
  // NOTE: the request has been validated by one of the callers.
  const bool broadcast_change_to_all_cells = not request.cells.front().nr_cgi.has_value();
  if (broadcast_change_to_all_cells) {
    for (unsigned cell_idx = MIN_DU_CELL_INDEX; cell_idx != du_cells.nof_cells(); ++cell_idx) {
      if (const auto cell_index = to_du_cell_index(cell_idx); du_cells.is_cell_active(cell_index)) {
        const nr_cell_global_id_t nr_cgi = du_cells.get_cell_cfg(cell_index).nr_cgi;
        auto                      result = du_cells.handle_cell_reconf_request(
            du_cell_param_config_request(nr_cgi, std::nullopt, request.cells.front().rrm_policy_ratio_list));
        if (not result.has_value()) {
          continue;
        }
        // Mark that cell was changed.
        changed_cells.push_back(result.value());
      }
    }
  } else {
    for (const du_cell_param_config_request& cell_to_update : request.cells) {
      auto result = du_cells.handle_cell_reconf_request(cell_to_update);
      if (not result.has_value()) {
        continue;
      }

      // Mark that cell was changed.
      changed_cells.push_back(result.value());
    }
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
du_param_config_procedure::handle_mac_cell_update(const du_cell_reconfig_result& changed_cell) const
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

  return du_params.mac.mgr.get_cell_manager().get_cell_controller(changed_cell.cell_index).reconfigure(req);
}
