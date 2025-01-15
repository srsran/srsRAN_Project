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
#include "mac_config.h"
#include "mac_scheduler_configurator.h"

namespace srsran {

class mac_ue_delete_procedure
{
public:
  explicit mac_ue_delete_procedure(const mac_ue_delete_request& msg,
                                   mac_control_config&          cfg_,
                                   mac_ctrl_configurator&       mac_ctrl_,
                                   mac_ul_configurator&         mac_ul_,
                                   mac_dl_configurator&         mac_dl_,
                                   mac_scheduler_configurator&  sched_configurator_) :
    req(msg),
    cfg(cfg_),
    logger(cfg.logger),
    ctrl_mac(mac_ctrl_),
    ul_mac(mac_ul_),
    dl_mac(mac_dl_),
    sched_configurator(sched_configurator_)
  {
  }

  void operator()(coro_context<async_task<mac_ue_delete_response>>& ctx);

  static const char* name() { return "UE Delete Request"; }

private:
  mac_ue_delete_request       req;
  mac_control_config&         cfg;
  srslog::basic_logger&       logger;
  mac_ctrl_configurator&      ctrl_mac;
  mac_ul_configurator&        ul_mac;
  mac_dl_configurator&        dl_mac;
  mac_scheduler_configurator& sched_configurator;
};

} // namespace srsran
