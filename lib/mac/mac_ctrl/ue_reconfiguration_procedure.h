/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../mac_config_interfaces.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/async/async_task.h"

namespace srsran {

struct mac_control_config;
class mac_scheduler_configurator;

class mac_ue_reconfiguration_procedure
{
public:
  explicit mac_ue_reconfiguration_procedure(const mac_ue_reconfiguration_request& req_,
                                            mac_control_config&                   cfg_,
                                            mac_ul_configurator&                  mac_ul_,
                                            mac_dl_configurator&                  mac_dl_,
                                            mac_scheduler_configurator&           sched_cfg_);

  void operator()(coro_context<async_task<mac_ue_reconfiguration_response>>& ctx);

  static const char* name() { return "UE Reconfiguration Request"; }

private:
  mac_ue_reconfiguration_response handle_result(bool result);

  mac_ue_reconfiguration_request req;
  mac_control_config&            cfg;
  srslog::basic_logger&          logger;
  mac_ul_configurator&           ul_unit;
  mac_dl_configurator&           dl_unit;
  mac_scheduler_configurator&    sched_cfg;

  bool add_ue_result  = false;
  bool sched_conf_res = true;
};

} // namespace srsran
