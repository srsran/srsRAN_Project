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
#include "srsran/du/du.h"
#include <memory>
#include <vector>

namespace srsran {

namespace app_services {
class metrics_notifier;
} // namespace app_services

namespace srs_du {
class f1u_du_gateway;
class f1c_connection_client;
} // namespace srs_du

class e2_connection_client;
class e2_du_metrics_connector;
class e2_du_metrics_notifier;
class e2_du_metrics_interface;
class mac_pcap;
class rlc_pcap;
class timer_manager;
struct worker_manager;
class mac_clock_controller;

/// O-DU unit.
struct o_du_unit {
  std::unique_ptr<srs_du::du>               unit;
  std::vector<app_services::metrics_config> metrics;
  application_unit_commands                 commands;
  std::unique_ptr<e2_metric_connector_manager<e2_du_metrics_connector, e2_du_metrics_notifier, e2_du_metrics_interface>>
      e2_metric_connectors;
};

/// O-RAN DU unit dependencies.
struct o_du_unit_dependencies {
  worker_manager*                 workers            = nullptr;
  srs_du::f1c_connection_client*  f1c_client_handler = nullptr;
  srs_du::f1u_du_gateway*         f1u_gw             = nullptr;
  mac_clock_controller*           timer_ctrl         = nullptr;
  mac_pcap*                       mac_p              = nullptr;
  rlc_pcap*                       rlc_p              = nullptr;
  e2_connection_client*           e2_client_handler  = nullptr;
  app_services::metrics_notifier* metrics_notifier   = nullptr;
};

} // namespace srsran
