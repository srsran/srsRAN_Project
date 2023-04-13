/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_deletion_procedure.h"

using namespace srsran;
using namespace srs_du;

ue_deletion_procedure::ue_deletion_procedure(const f1ap_ue_delete_request& msg_,
                                             du_ue_manager_repository&     ue_mng_,
                                             const du_manager_params&      du_params_) :
  msg(msg_),
  ue_mng(ue_mng_),
  du_params(du_params_),
  proc_logger(srslog::fetch_basic_logger("DU-MNG"), name(), msg_.ue_index)
{
}

void ue_deletion_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  proc_logger.log_proc_started();

  ue = ue_mng.find_ue(msg.ue_index);
  if (ue == nullptr) {
    proc_logger.log_proc_failure("ueId does not exist.");
    CORO_EARLY_RETURN();
  }

  // > Remove UE from MAC.
  CORO_AWAIT_VALUE(const mac_ue_delete_response_message mac_resp, launch_mac_ue_delete());
  if (not mac_resp.result) {
    proc_logger.log_proc_failure("Failed to remove UE from MAC.");
  }

  // > Remove UE from F1AP.
  du_params.f1ap.ue_mng.handle_ue_deletion_request(msg.ue_index);

  // > Remove UE object from DU UE manager.
  ue_mng.remove_ue(msg.ue_index);

  proc_logger.log_proc_completed();

  CORO_RETURN();
}

async_task<mac_ue_delete_response_message> ue_deletion_procedure::launch_mac_ue_delete()
{
  mac_ue_delete_request_message mac_msg{};
  mac_msg.ue_index   = ue->ue_index;
  mac_msg.rnti       = ue->rnti;
  mac_msg.cell_index = ue->pcell_index;
  return du_params.mac.ue_cfg.handle_ue_delete_request(mac_msg);
}
