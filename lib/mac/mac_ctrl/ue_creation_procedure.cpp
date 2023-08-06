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

#include "ue_creation_procedure.h"
#include "../../ran/gnb_format.h"

using namespace srsran;

void mac_ue_create_request_procedure::operator()(coro_context<async_task<mac_ue_create_response>>& ctx)
{
  CORO_BEGIN(ctx);
  log_proc_started(logger, req.ue_index, req.crnti, name());

  // > Create UE in MAC CTRL.
  crnti_assigned = ctrl_unit.add_ue(req.ue_index, req.cell_index, req.crnti);
  if (crnti_assigned == INVALID_RNTI) {
    CORO_EARLY_RETURN(handle_mac_ue_create_result(false));
  }

  // > Update C-RNTI of the UE if it changed.
  req.crnti = crnti_assigned;

  // > Create UE UL context and channels.
  CORO_AWAIT_VALUE(add_ue_result, ul_unit.add_ue(req));
  if (not add_ue_result) {
    CORO_EARLY_RETURN(handle_mac_ue_create_result(false));
  }

  // > Create UE DL context and channels.
  CORO_AWAIT_VALUE(add_ue_result, dl_unit.add_ue(req));
  if (not add_ue_result) {
    // >> Revert creation of UE in MAC UL.
    CORO_AWAIT(ul_unit.remove_ue(mac_ue_delete_request{req.cell_index, req.ue_index, req.crnti}));

    // >> Terminate procedure.
    CORO_EARLY_RETURN(handle_mac_ue_create_result(false));
  }

  // > Create UE context in Scheduler.
  CORO_AWAIT_VALUE(add_ue_result, sched_configurator.handle_ue_creation_request(req));
  if (not add_ue_result) {
    // >> Revert creation of UE in MAC UL and MAC DL.
    CORO_AWAIT(ul_unit.remove_ue(mac_ue_delete_request{req.cell_index, req.ue_index, req.crnti}));
    CORO_AWAIT(dl_unit.remove_ue(mac_ue_delete_request{req.cell_index, req.ue_index, req.crnti}));

    // >> Terminate procedure.
    CORO_EARLY_RETURN(handle_mac_ue_create_result(false));
  }

  // > After UE insertion in scheduler, send response to DU manager.
  CORO_RETURN(handle_mac_ue_create_result(add_ue_result));
}

mac_ue_create_response mac_ue_create_request_procedure::handle_mac_ue_create_result(bool result)
{
  if (result) {
    log_proc_completed(logger, req.ue_index, req.crnti, name());
  } else {
    log_proc_failure(logger, req.ue_index, req.crnti, name());
  }

  if (not result and crnti_assigned != INVALID_RNTI) {
    // Remove created UE object
    ctrl_unit.remove_ue(req.ue_index);
  }

  // Respond back to DU manager with result
  mac_ue_create_response resp{};
  resp.ue_index        = req.ue_index;
  resp.cell_index      = req.cell_index;
  resp.allocated_crnti = result ? crnti_assigned : INVALID_RNTI;
  return resp;
}
