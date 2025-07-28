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

#include "apps/helpers/e2/e2_metric_connector_manager.h"
#include "apps/services/metrics/metrics_config.h"
#include "apps/units/application_unit_commands.h"
#include "o_cu_cp_unit_impl.h"
#include "srsran/cu_cp/cu_cp.h"
#include "srsran/e2/e2_cu_metrics_connector.h"

namespace srsran {
namespace app_services {
class metrics_notifier;
}

class dlt_pcap;
class io_broker;
struct o_cu_cp_unit_config;
struct worker_manager;

namespace srs_cu_cp {

class n2_connection_client;
class cu_cp_executor_mapper;

} // namespace srs_cu_cp

template <typename ConnectorType, typename NotifierType, typename InterfaceType>
class e2_metric_connector_manager;
class e2_connection_client;
class e2_gateway_remote_connector;

/// O-RAN CU-CP build dependencies.
struct o_cu_cp_unit_dependencies {
  srs_cu_cp::cu_cp_executor_mapper* executor_mapper  = nullptr;
  timer_manager*                    timers           = nullptr;
  dlt_pcap*                         ngap_pcap        = nullptr;
  io_broker*                        broker           = nullptr;
  e2_connection_client*             e2_gw            = nullptr;
  app_services::metrics_notifier*   metrics_notifier = nullptr;
};

/// O-RAN CU-CP unit.
struct o_cu_cp_unit {
  std::unique_ptr<srs_cu_cp::o_cu_cp>       unit;
  application_unit_commands                 commands;
  std::vector<app_services::metrics_config> metrics;
};

/// Builds an O-RAN CU-CP unit with the given configuration and dependencies.
o_cu_cp_unit build_o_cu_cp(const o_cu_cp_unit_config& unit_cfg, o_cu_cp_unit_dependencies& dependencies);

} // namespace srsran
