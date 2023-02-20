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
#include "../mac_config.h"
#include "../mac_config_interfaces.h"
#include "mac_scheduler_configurator.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {

class mac_ue_create_request_procedure
{
public:
  explicit mac_ue_create_request_procedure(const mac_ue_create_request_message& req_,
                                           mac_common_config_t&                 cfg_,
                                           mac_ctrl_configurator&               mac_ctrl_,
                                           mac_ul_configurator&                 mac_ul_,
                                           mac_dl_configurator&                 mac_dl_,
                                           mac_scheduler_configurator&          sched_configurator_) :
    req(req_),
    cfg(cfg_),
    logger(cfg.logger),
    ctrl_unit(mac_ctrl_),
    ul_unit(mac_ul_),
    dl_unit(mac_dl_),
    sched_configurator(sched_configurator_)
  {
  }

  void operator()(coro_context<async_task<mac_ue_create_response_message>>& ctx)
  {
    CORO_BEGIN(ctx);
    log_proc_started(logger, req.ue_index, req.crnti, "UE Create Request");

    // > Create UE in MAC CTRL.
    ctrl_ue_created = ctrl_unit.add_ue(req.ue_index, req.crnti, req.cell_index);
    if (not ctrl_ue_created) {
      CORO_EARLY_RETURN(handle_mac_ue_create_result(false));
    }

    // > Create UE UL context and channels.
    CORO_AWAIT_VALUE(add_ue_result, ul_unit.add_ue(req));
    if (not add_ue_result) {
      CORO_EARLY_RETURN(handle_mac_ue_create_result(false));
    }

    // > Create UE DL context and channels.
    CORO_AWAIT_VALUE(add_ue_result, dl_unit.add_ue(req));

    // > Create UE context in Scheduler.
    CORO_AWAIT(sched_configurator.handle_ue_creation_request(req));

    log_proc_completed(logger, req.ue_index, req.crnti, "UE Create Request");

    // > After UE insertion in scheduler, send response to DU manager.
    CORO_RETURN(handle_mac_ue_create_result(add_ue_result));
  }

  static const char* name() { return "UE Create Request"; }

private:
  mac_ue_create_response_message handle_mac_ue_create_result(bool result)
  {
    if (result) {
      log_proc_completed(logger, req.ue_index, req.crnti, "UE Create Request");
    } else {
      log_proc_failure(logger, req.ue_index, req.crnti, "UE Create Request");
    }

    if (not result and ctrl_ue_created) {
      // Remove created UE object
      ctrl_unit.remove_ue(req.ue_index);
    }

    // Respond back to DU manager with result
    mac_ue_create_response_message resp{};
    resp.ue_index   = req.ue_index;
    resp.cell_index = req.cell_index;
    resp.result     = result;
    return resp;
  }

  const mac_ue_create_request_message req;
  mac_common_config_t&                cfg;
  srslog::basic_logger&               logger;
  mac_ctrl_configurator&              ctrl_unit;
  mac_ul_configurator&                ul_unit;
  mac_dl_configurator&                dl_unit;
  mac_scheduler_configurator&         sched_configurator;

  bool ctrl_ue_created = false;
  bool add_ue_result   = false;
};

} // namespace srsgnb
