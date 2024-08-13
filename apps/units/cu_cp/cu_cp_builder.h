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
#include "apps/services/e2_metric_connector_manager.h"
#include "srsran/cu_cp/cu_cp.h"

namespace srsran {

class dlt_pcap;
class io_broker;
struct cu_cp_unit_config;
struct worker_manager;

namespace srs_cu_cp {
class n2_connection_client;
}

class e2_metric_connector_manager;
class e2_connection_client;
class e2_gateway_remote_connector;

/// CU-CP build dependencies.
struct cu_cp_build_dependencies {
  task_executor*               cu_cp_executor = nullptr;
  task_executor*               cu_cp_e2_exec  = nullptr;
  timer_manager*               timers         = nullptr;
  dlt_pcap*                    ngap_pcap      = nullptr;
  io_broker*                   broker         = nullptr;
  e2_gateway_remote_connector* e2_gw          = nullptr;
};

/// Wraps the CU-CP and its supported application commands.
struct cu_cp_unit {
  std::unique_ptr<srs_cu_cp::cu_cp>                               unit;
  std::vector<std::unique_ptr<app_services::application_command>> commands;
};

/// Builds a CU-CP object with the given configuration.
cu_cp_unit build_cu_cp(const cu_cp_unit_config&     cu_cp_unit_cfg,
                       cu_cp_build_dependencies&    dependencies,
                       e2_metric_connector_manager& e2_metric_connectors);

} // namespace srsran
