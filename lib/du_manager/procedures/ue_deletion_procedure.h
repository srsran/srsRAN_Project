/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../ran/gnb_format.h"
#include "../du_ue/du_ue.h"
#include "../du_ue/ue_manager_ctrl_configurator.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/du_manager/du_manager_params.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_du {

class ue_deletion_procedure
{
public:
  ue_deletion_procedure(const f1ap_ue_delete_request& msg_,
                        mac_ue_configurator&          mac_ue_mng_,
                        ue_manager_ctrl_configurator& ue_mng_) :
    msg(msg_), mac_ue_mng(mac_ue_mng_), ue_mng(ue_mng_), logger(srslog::fetch_basic_logger("DU-MNG"))
  {
  }

  void operator()(coro_context<async_task<void>>& ctx)
  {
    CORO_BEGIN(ctx);

    log_proc_started(logger, msg.ue_index, name());

    ue = ue_mng.find_ue(msg.ue_index);
    if (ue == nullptr) {
      log_proc_failure(logger, msg.ue_index, name(), "ueId does not exist.");
      CORO_EARLY_RETURN();
    }

    // 1. Remove UE from MAC
    CORO_AWAIT_VALUE(mac_ue_delete_response_message mac_resp, launch_mac_ue_delete());
    if (not mac_resp.result) {
      log_proc_failure(logger, ue->ue_index, ue->rnti, name(), "Failed to remove UE from MAC.");
    }

    // 2. Remove UE from F1AP
    // TODO

    // 3. Remove UE object from DU UE manager
    ue_mng.remove_ue(msg.ue_index);

    log_proc_completed(logger, msg.ue_index, ue->rnti, name());
    CORO_RETURN();
  }

  static const char* name() { return "UE Delete"; }

private:
  async_task<mac_ue_delete_response_message> launch_mac_ue_delete()
  {
    mac_ue_delete_request_message mac_msg{};
    mac_msg.ue_index   = ue->ue_index;
    mac_msg.rnti       = ue->rnti;
    mac_msg.cell_index = ue->pcell_index;
    return mac_ue_mng.handle_ue_delete_request(mac_msg);
  }

  const f1ap_ue_delete_request  msg;
  mac_ue_configurator&          mac_ue_mng;
  ue_manager_ctrl_configurator& ue_mng;
  srslog::basic_logger&         logger;

  du_ue* ue = nullptr;
};

} // namespace srs_du
} // namespace srsgnb
