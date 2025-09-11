/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "e2ap_configuration.h"
#include "e2sm/e2sm.h"
#include "srsran/support/timers.h"
#include <vector>

namespace srsran {

class e2_connection_client;

/// E2SM Service Module to be added into E2 agent.
struct e2sm_module {
  uint32_t                        ran_func_id;
  std::string                     oid;
  std::unique_ptr<e2sm_handler>   e2sm_packer;
  std::unique_ptr<e2sm_interface> e2sm_iface;
};

/// O-RAN E2 Agent dependencies.
struct e2_agent_dependencies {
  srslog::basic_logger*    logger;
  e2ap_configuration       cfg;
  e2_connection_client*    e2_client;
  timer_factory*           timers;
  task_executor*           task_exec;
  std::vector<e2sm_module> e2sm_modules;
};

} // namespace srsran
