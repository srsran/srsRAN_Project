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

#include "apps/services/metrics/metrics_config.h"
#include "apps/units/application_unit_commands.h"
#include "srsran/du/du_high/o_du_high.h"
#include "srsran/du/du_high/o_du_high_config.h"

namespace srsran {
struct o_du_high_unit_config;

namespace app_services {
class metrics_notifier;
}

namespace srs_du {
class du_high_executor_mapper;
struct o_du_high_config;
struct o_du_high_dependencies;
class f1c_connection_client;
class f1u_du_gateway;
} // namespace srs_du

class du_high_executor_mapper;
template <typename ConnectorType, typename NotifierType, typename InterfaceType>
class e2_metric_connector_manager;
class e2_du_metrics_connector;
class e2_du_metrics_notifier;
class e2_du_metrics_interface;
class e2_connection_client;
class mac_pcap;
class rlc_pcap;
class timer_manager;
struct du_high_unit_config;
struct du_high_wrapper_config;
struct du_high_wrapper_dependencies;
using e2_du_metrics_connector_manager =
    e2_metric_connector_manager<e2_du_metrics_connector, e2_du_metrics_notifier, e2_du_metrics_interface>;

/// Prints basic DU info in the stdout and in the GNB logs.
void announce_du_high_cells(const du_high_unit_config& du_high_unit_cfg);

/// O-RAN DU high unit.
struct o_du_high_unit {
  std::unique_ptr<srs_du::o_du_high>        o_du_hi;
  application_unit_commands                 commands;
  std::vector<app_services::metrics_config> metrics;
};

/// O-RAN DU high unit dependencies.
struct o_du_high_unit_dependencies {
  srs_du::du_high_executor_mapper& execution_mapper;
  srs_du::f1c_connection_client&   f1c_client_handler;
  srs_du::f1u_du_gateway&          f1u_gw;
  mac_clock_controller&            timer_ctrl;
  mac_pcap&                        mac_p;
  rlc_pcap&                        rlc_p;
  e2_connection_client&            e2_client_handler;
  e2_du_metrics_connector_manager& e2_metric_connectors;
  app_services::metrics_notifier&  metrics_notifier;
  srs_du::o_du_high_dependencies   o_du_hi_dependencies;
};

/// Creates the O-RAN DU high unit with the given configuration.
o_du_high_unit make_o_du_high_unit(const o_du_high_unit_config&  o_du_high_unit_cfg,
                                   o_du_high_unit_dependencies&& dependencies);

} // namespace srsran
