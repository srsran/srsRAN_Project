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
#include "apps/services/metrics/metrics_config.h"
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
template <typename ConnectorType, typename NotifierType, typename InterfaceType>
class e2_metric_connector_manager;
class e2_du_metrics_connector;
class e2_du_metrics_notifier;
class e2_du_metrics_interface;
class mac_pcap;
class rlc_pcap;
class timer_manager;
struct worker_manager;

/// Wraps the DU and its supported application commands.
struct du_unit {
  std::unique_ptr<srs_du::du>                                     unit;
  std::vector<std::unique_ptr<app_services::application_command>> commands;
  std::vector<app_services::metrics_config>                       metrics;
  std::unique_ptr<e2_metric_connector_manager<e2_du_metrics_connector, e2_du_metrics_notifier, e2_du_metrics_interface>>
    e2_metric_connectors;
};

/// DU unit dependencies.
struct du_unit_dependencies {
  worker_manager*                 workers              = nullptr;
  srs_du::f1c_connection_client*  f1c_client_handler   = nullptr;
  srs_du::f1u_du_gateway*         f1u_gw               = nullptr;
  timer_manager*                  timer_mng            = nullptr;
  mac_pcap*                       mac_p                = nullptr;
  rlc_pcap*                       rlc_p                = nullptr;
  e2_connection_client*           e2_client_handler    = nullptr;
  srslog::sink*                   json_sink            = nullptr;
  app_services::metrics_notifier* metrics_notifier     = nullptr;
};

} // namespace srsran
