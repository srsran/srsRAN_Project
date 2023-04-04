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

#include "../mac_config_interfaces.h"
#include "srsran/support/async/async_task.h"

namespace srsran {

class mac_common_config_t;
class mac_scheduler_configurator;

class mac_ue_reconfiguration_procedure
{
public:
  explicit mac_ue_reconfiguration_procedure(const mac_ue_reconfiguration_request_message& req_,
                                            mac_common_config_t&                          cfg_,
                                            mac_ul_configurator&                          mac_ul_,
                                            mac_dl_configurator&                          mac_dl_,
                                            mac_scheduler_configurator&                   sched_cfg_);

  void operator()(coro_context<async_task<mac_ue_reconfiguration_response_message>>& ctx);

  static const char* name() { return "UE Reconfiguration Request"; }

private:
  mac_ue_reconfiguration_response_message handle_result(bool result);

  mac_ue_reconfiguration_request_message req;
  mac_common_config_t&                   cfg;
  srslog::basic_logger&                  logger;
  mac_ul_configurator&                   ul_unit;
  mac_dl_configurator&                   dl_unit;
  mac_scheduler_configurator&            sched_cfg;

  bool add_ue_result = false;
};

} // namespace srsran
