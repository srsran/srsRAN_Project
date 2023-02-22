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

    // 1. Reconfigure UE UL context and channels.
    // Note: We may need to delete bearers first from DL to avoid concurrent scheduling in erased bearers.
    CORO_AWAIT_VALUE(add_ue_result, ul_unit.reconfigure_ue(req));
    if (not add_ue_result) {
      CORO_EARLY_RETURN(handle_result(false));
    }

    // 2. Reconfigure UE DL context and channels.
    CORO_AWAIT_VALUE(add_ue_result, dl_unit.reconfigure_ue(req));

    // 3. Reconfigure UE in Scheduler.
    log_proc_started(logger, req.ue_index, req.crnti, "Sched UE Config");
    CORO_AWAIT(sched_cfg.handle_ue_reconfiguration_request(req));
    log_proc_completed(logger, req.ue_index, req.crnti, "Sched UE Config");

    // 4. After UE insertion, send response to DU manager.
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
