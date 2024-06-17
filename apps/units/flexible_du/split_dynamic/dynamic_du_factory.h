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

#include "apps/gnb/gnb_appconfig.h"
#include "apps/services/application_command.h"
#include "apps/services/metrics_hub.h"
#include "apps/services/worker_manager.h"
#include "srsran/du/du.h"
#include "srsran/pcap/rlc_pcap.h"

namespace srsran {

class e2_connection_client;
class e2_metric_connector_manager;
class f1ap_message_notifier;
class console_helper;
class metrics_log_helper;
class metrics_plotter_json;
class metrics_plotter_stdout;
class mac_pcap;
class timer_manager;
class upper_phy_rg_gateway;
class upper_phy_rx_symbol_request_notifier;

namespace srs_du {
class f1c_connection_client;
class f1u_du_gateway;
} // namespace srs_du

/// Wraps the DU and its supported application commands.
struct du_unit {
  std::unique_ptr<du>                                             unit;
  std::vector<std::unique_ptr<app_services::application_command>> commands;
};

du_unit create_du(const dynamic_du_unit_config&  dyn_du_cfg,
                  worker_manager&                workers,
                  srs_du::f1c_connection_client& f1c_client_handler,
                  srs_du::f1u_du_gateway&        f1u_gw,
                  timer_manager&                 timer_mng,
                  mac_pcap&                      mac_p,
                  rlc_pcap&                      rlc_p,
                  metrics_plotter_stdout&        metrics_stdout,
                  metrics_plotter_json&          metrics_json,
                  metrics_log_helper&            metrics_logger,
                  e2_connection_client&          e2_client_handler,
                  e2_metric_connector_manager&   e2_metric_connectors,
                  rlc_metrics_notifier&          rlc_json_metrics,
                  metrics_hub&                   metrics_hub);

} // namespace srsran
