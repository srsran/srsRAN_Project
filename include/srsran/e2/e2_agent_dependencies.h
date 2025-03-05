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
