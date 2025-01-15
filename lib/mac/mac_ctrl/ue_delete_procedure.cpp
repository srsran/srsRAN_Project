/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_delete_procedure.h"
#include "proc_logger.h"

using namespace srsran;

void mac_ue_delete_procedure::operator()(coro_context<async_task<mac_ue_delete_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("{}: started...", mac_log_prefix(req.ue_index, req.rnti, name()));

  // > Remove UE from scheduler.
  // Note: Removing the UE from the scheduler before the MAC avoids potential race conditions (assuming the scheduler
  // doesn't allocate UEs after being removed).
  CORO_AWAIT(sched_configurator.handle_ue_removal_request(req));

  // > Remove UE and associated DL channels from the MAC DL.
  CORO_AWAIT(dl_mac.remove_ue(req));

  // > Remove UE associated UL channels from the MAC UL.
  CORO_AWAIT(ul_mac.remove_ue(req));

  // > Enqueue UE deletion
  ctrl_mac.remove_ue(req.ue_index);

  logger.info("{}: finished successfully", mac_log_prefix(req.ue_index, req.rnti, name()));

  // 4. Signal end of procedure and pass response
  CORO_RETURN(mac_ue_delete_response{true});
}
