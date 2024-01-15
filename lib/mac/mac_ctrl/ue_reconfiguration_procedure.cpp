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

#include "ue_reconfiguration_procedure.h"
#include "../../ran/gnb_format.h"
#include "mac_config.h"
#include "mac_scheduler_configurator.h"

using namespace srsran;

mac_ue_reconfiguration_procedure::mac_ue_reconfiguration_procedure(const mac_ue_reconfiguration_request& req_,
                                                                   mac_control_config&                   cfg_,
                                                                   mac_ul_configurator&                  mac_ul_,
                                                                   mac_dl_configurator&                  mac_dl_,
                                                                   mac_scheduler_configurator&           sched_cfg_) :
  req(req_), cfg(cfg_), logger(cfg.logger), ul_unit(mac_ul_), dl_unit(mac_dl_), sched_cfg(sched_cfg_)
{
}

void mac_ue_reconfiguration_procedure::operator()(coro_context<async_task<mac_ue_reconfiguration_response>>& ctx)
{
  CORO_BEGIN(ctx);
  log_proc_started(logger, req.ue_index, req.crnti, "UE Reconfiguration Request");

  // If there are bearers to add or modify.
  if (not req.bearers_to_addmod.empty()) {
    // > Add/Mod logical channels in the DEMUX.
    CORO_AWAIT_VALUE(add_ue_result, ul_unit.addmod_bearers(req.ue_index, req.bearers_to_addmod));
    if (not add_ue_result) {
      CORO_EARLY_RETURN(handle_result(false));
    }

    // > Add/Mod logical channels in the MUX.
    CORO_AWAIT_VALUE(add_ue_result, dl_unit.addmod_bearers(req.ue_index, req.pcell_index, req.bearers_to_addmod));
    if (not add_ue_result) {
      CORO_EARLY_RETURN(handle_result(false));
    }
  }

  // > Reconfigure UE in Scheduler.
  log_proc_started(logger, req.ue_index, req.crnti, "Sched UE Config");
  CORO_AWAIT_VALUE(sched_conf_res, sched_cfg.handle_ue_reconfiguration_request(req));
  log_proc_completed(logger, req.ue_index, req.crnti, "Sched UE Config");
  if (not sched_conf_res) {
    CORO_EARLY_RETURN(handle_result(false));
  }

  // If there are bearers to remove.
  if (not req.bearers_to_rem.empty()) {
    // > Remove logical channels in the DEMUX.
    CORO_AWAIT_VALUE(add_ue_result, ul_unit.remove_bearers(req.ue_index, req.bearers_to_rem));
    if (not add_ue_result) {
      CORO_EARLY_RETURN(handle_result(false));
    }

    // > Remove logical channels in the MUX.
    CORO_AWAIT_VALUE(add_ue_result, dl_unit.remove_bearers(req.ue_index, req.pcell_index, req.bearers_to_rem));
    if (not add_ue_result) {
      CORO_EARLY_RETURN(handle_result(false));
    }
  }

  // > After UE insertion, send response to DU manager.
  CORO_RETURN(handle_result(true));
}

mac_ue_reconfiguration_response mac_ue_reconfiguration_procedure::handle_result(bool result)
{
  if (result) {
    log_proc_completed(logger, req.ue_index, req.crnti, name());
  } else {
    log_proc_failure(logger, req.ue_index, req.crnti, name());
  }

  // Respond back to DU manager with result
  mac_ue_reconfiguration_response resp{};
  resp.ue_index = req.ue_index;
  resp.result   = result;
  return resp;
}
