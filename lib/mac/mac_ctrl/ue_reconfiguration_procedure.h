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

#include "../mac_config_interfaces.h"
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
