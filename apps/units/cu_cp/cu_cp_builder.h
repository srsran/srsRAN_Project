/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "apps/services/application_command.h"
#include "srsran/cu_cp/cu_cp.h"

namespace srsran {

struct cu_cp_unit_config;
struct worker_manager;

namespace srs_cu_cp {
class n2_connection_client;
}

/// CU-CP build dependencies.
struct cu_cp_build_dependencies {
  task_executor*                                                cu_cp_executor = nullptr;
  task_executor*                                                cu_cp_e2_exec  = nullptr;
  std::vector<std::unique_ptr<srs_cu_cp::n2_connection_client>> n2_clients;
  timer_manager*                                                timers = nullptr;
};

/// Wraps the CU-CP and its supported application commands.
struct cu_cp_unit {
  std::unique_ptr<srs_cu_cp::cu_cp>                               unit;
  std::vector<std::unique_ptr<app_services::application_command>> commands;
};

/// Builds a CU-CP object with the given configuration.
cu_cp_unit build_cu_cp(const cu_cp_unit_config& cu_cp_unit_cfg, cu_cp_build_dependencies& dependencies);

} // namespace srsran
