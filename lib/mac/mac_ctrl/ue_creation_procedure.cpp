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

#include "ue_creation_procedure.h"
#include "proc_logger.h"

using namespace srsran;

void mac_ue_create_request_procedure::operator()(coro_context<async_task<mac_ue_create_response>>& ctx)
{
  CORO_BEGIN(ctx);
  logger.debug("{}: started...", mac_log_prefix(req.ue_index, req.crnti, name()));

  // > Create UE in MAC CTRL.
  crnti_assigned = ctrl_unit.add_ue(req.ue_index, req.cell_index, req.crnti);
  if (crnti_assigned == rnti_t::INVALID_RNTI) {
    CORO_AWAIT(cancel_ue_creation());
    CORO_EARLY_RETURN(handle_mac_ue_create_result(false));
  }

  // > Update C-RNTI of the UE if it changed.
  req.crnti = crnti_assigned;

  // > Create UE UL context and channels.
  CORO_AWAIT_VALUE(add_ul_ue_result, ul_unit.add_ue(req));
  if (not add_ul_ue_result) {
    CORO_AWAIT(cancel_ue_creation());
    CORO_EARLY_RETURN(handle_mac_ue_create_result(false));
  }
  logger.debug("{}: UE UL context created successfully", mac_log_prefix(req.ue_index, req.crnti, name()));

  // > Create UE DL context and channels.
  CORO_AWAIT_VALUE(add_dl_ue_result, dl_unit.add_ue(req));
  if (not add_dl_ue_result) {
    // >> Terminate procedure.
    CORO_AWAIT(cancel_ue_creation());
    CORO_EARLY_RETURN(handle_mac_ue_create_result(false));
  }
  logger.debug("{}: UE DL context created successfully", mac_log_prefix(req.ue_index, req.crnti, name()));

  // > Create UE context in Scheduler.
  CORO_AWAIT_VALUE(add_sched_ue_result, sched_configurator.handle_ue_creation_request(req));
  if (not add_sched_ue_result) {
    // >> Terminate procedure.
    CORO_AWAIT(cancel_ue_creation());
    CORO_EARLY_RETURN(handle_mac_ue_create_result(false));
  }

  // > After UE insertion in scheduler, send response to DU manager.
  CORO_RETURN(handle_mac_ue_create_result(true));
}

async_task<void> mac_ue_create_request_procedure::cancel_ue_creation()
{
  return launch_async([this](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    if (add_dl_ue_result) {
      // > Revert creation of UE in MAC DL.
      CORO_AWAIT(dl_unit.remove_ue(mac_ue_delete_request{req.cell_index, req.ue_index, req.crnti}));
    }

    if (add_ul_ue_result) {
      // > Revert creation of UE in MAC UL.
      CORO_AWAIT(ul_unit.remove_ue(mac_ue_delete_request{req.cell_index, req.ue_index, req.crnti}));
    }

    if (crnti_assigned != rnti_t::INVALID_RNTI) {
      // > Revert creation of UE in MAC CTRL.
      ctrl_unit.remove_ue(req.ue_index);
    }

    CORO_RETURN();
  });
}

mac_ue_create_response mac_ue_create_request_procedure::handle_mac_ue_create_result(bool result)
{
  if (result) {
    logger.info("{}: finished successfully", mac_log_prefix(req.ue_index, req.crnti, name()));
  } else {
    logger.warning("{}: failed", mac_log_prefix(req.ue_index, req.crnti, name()));
  }

  // Respond back to DU manager with result
  mac_ue_create_response resp{};
  resp.ue_index        = req.ue_index;
  resp.cell_index      = req.cell_index;
  resp.allocated_crnti = result ? crnti_assigned : rnti_t::INVALID_RNTI;
  return resp;
}
