/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
