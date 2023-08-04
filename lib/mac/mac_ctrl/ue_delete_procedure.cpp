/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_delete_procedure.h"
#include "../../ran/gnb_format.h"

using namespace srsran;

void mac_ue_delete_procedure::operator()(coro_context<async_task<mac_ue_delete_response>>& ctx)
{
  CORO_BEGIN(ctx);

  log_proc_started(logger, req.ue_index, req.rnti, name());

  // 1. Remove UE from scheduler.
  // Note: Removing the UE from the scheduler before the MAC avoids potential race conditions (assuming the scheduler
  // doesn't allocate UEs after being removed).
  CORO_AWAIT(sched_configurator.handle_ue_removal_request(req));

  // 2. Remove UE and associated DL channels from the MAC DL.
  CORO_AWAIT(dl_mac.remove_ue(req));

  // 3. Remove UE associated UL channels from the MAC UL.
  CORO_AWAIT(ul_mac.remove_ue(req));

  // 4. Enqueue UE deletion
  ctrl_mac.remove_ue(req.ue_index);

  log_proc_completed(logger, req.ue_index, req.rnti, "UE Delete Request");

  // 4. Signal end of procedure and pass response
  CORO_RETURN(mac_ue_delete_response{true});
}
