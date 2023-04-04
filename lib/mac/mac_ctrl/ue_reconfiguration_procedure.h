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
#include "srsran/support/async/async_task.h"

namespace srsran {

class mac_ue_reconfiguration_procedure
{
public:
  explicit mac_ue_reconfiguration_procedure(const mac_ue_reconfiguration_request_message& req_,
                                            mac_common_config_t&                          cfg_,
                                            mac_ul_configurator&                          mac_ul_,
                                            mac_dl_configurator&                          mac_dl_,
                                            mac_scheduler_configurator&                   sched_cfg_) :
    req(req_), cfg(cfg_), logger(cfg.logger), ul_unit(mac_ul_), dl_unit(mac_dl_), sched_cfg(sched_cfg_)
  {
  }

  void operator()(coro_context<async_task<mac_ue_reconfiguration_response_message>>& ctx)
  {
    CORO_BEGIN(ctx);
    log_proc_started(logger, req.ue_index, req.crnti, "UE Create Request");

    // If there are bearers to add or modify.
    if (not req.bearers_to_addmod.empty()) {
      // > Add/Mod logical channels in the UL direction.
      CORO_AWAIT_VALUE(add_ue_result, ul_unit.addmod_bearers(req.ue_index, req.bearers_to_addmod));
      if (not add_ue_result) {
        CORO_EARLY_RETURN(handle_result(false));
      }
    }

    // > Reconfigure UE DL context and channels.
    CORO_AWAIT_VALUE(add_ue_result, dl_unit.reconfigure_ue(req));

    // > Reconfigure UE in Scheduler.
    log_proc_started(logger, req.ue_index, req.crnti, "Sched UE Config");
    CORO_AWAIT(sched_cfg.handle_ue_reconfiguration_request(req));
    log_proc_completed(logger, req.ue_index, req.crnti, "Sched UE Config");

    // If there are bearers to remove.
    if (not req.bearers_to_rem.empty()) {
      // > Remove logical channels in the UL direction.
      CORO_AWAIT_VALUE(add_ue_result, ul_unit.remove_bearers(req.ue_index, req.bearers_to_rem));
      if (not add_ue_result) {
        CORO_EARLY_RETURN(handle_result(false));
      }
    }

    // > After UE insertion, send response to DU manager.
    CORO_RETURN(handle_result(add_ue_result));
  }

  static const char* name() { return "UE Reconfiguration Request"; }

private:
  mac_ue_reconfiguration_response_message handle_result(bool result)
  {
    if (result) {
      log_proc_completed(logger, req.ue_index, req.crnti, name());
    } else {
      log_proc_failure(logger, req.ue_index, req.crnti, name());
    }

    // Respond back to DU manager with result
    mac_ue_reconfiguration_response_message resp{};
    resp.ue_index = req.ue_index;
    resp.result   = result;
    return resp;
  }

  mac_ue_reconfiguration_request_message req;
  mac_common_config_t&                   cfg;
  srslog::basic_logger&                  logger;
  mac_ul_configurator&                   ul_unit;
  mac_dl_configurator&                   dl_unit;
  mac_scheduler_configurator&            sched_cfg;

  bool add_ue_result = false;
};

} // namespace srsran
