/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
    if (not macresp.si_updated) {
      resp.success = false;
      CORO_EARLY_RETURN(resp);
    }
  }

  // Initiate F1AP gNB-DU Configuration Update
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

  f1_req.cells_to_mod.resize(changed_cells.size());
  for (unsigned i = 0, e = changed_cells.size(); i != e; ++i) {
    if (changed_cells[i].cu_notif_required) {
      const du_cell_config& cell_cfg = du_cells.get_cell_cfg(changed_cells[i].cell_index);

      f1_req.cells_to_mod[i].cell_info   = make_f1ap_du_cell_info(cell_cfg);
      f1_req.cells_to_mod[i].du_sys_info = make_f1ap_du_sys_info(cell_cfg);
    }
  }

  return du_params.f1ap.conn_mng.handle_du_config_update(f1_req);
}

async_task<mac_cell_reconfig_response>
du_param_config_procedure::handle_mac_cell_update(const du_cell_reconfig_result& changed_cell)
{
  if (not changed_cell.sched_notif_required) {
    // TODO: Support changes that do not require SI scheduling config update.
    return launch_no_op_task(mac_cell_reconfig_response{});
  }

  mac_cell_reconfig_request req;

  // Update SIB1 and SI message content.
  req.new_sys_info.emplace();
  const auto& sys_info   = du_cells.get_packed_sys_info(changed_cell.cell_index);
  req.new_sys_info->sib1 = sys_info.sib1.copy();
  for (const auto& si_msg : sys_info.si_messages) {
    req.new_sys_info->si_messages.emplace_back(si_msg.copy());
  }

  // Provide new SI scheduling config.
  req.new_sys_info->si_sched_cfg = du_cells.get_si_sched_req(changed_cell.cell_index);

  return du_params.mac.cell_mng.get_cell_controller(changed_cell.cell_index).reconfigure(req);
}
