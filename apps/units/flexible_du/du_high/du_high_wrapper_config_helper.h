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

namespace srsran {

namespace srs_du {
class f1u_du_gateway;
class f1c_connection_client;
} // namespace srs_du

class du_high_executor_mapper;
class e2_metric_connector_manager;
class e2_connection_client;
class mac_pcap;
class metrics_hub;
class metrics_log_helper;
class metrics_plotter_json;
class metrics_plotter_stdout;
class rlc_metrics_notifier;
class rlc_pcap;
class timer_manager;
struct du_high_unit_config;
struct du_high_wrapper_config;
struct du_high_wrapper_dependencies;

/// Set up sources for the DU high metrics.
void configure_du_high_metrics(const du_high_unit_config&   du_high_unit_cfg,
                               metrics_plotter_stdout&      metrics_stdout,
                               metrics_log_helper&          metrics_logger,
                               metrics_plotter_json&        metrics_json,
                               rlc_metrics_notifier&        rlc_metrics_json,
                               e2_metric_connector_manager& e2_metric_connectors,
                               metrics_hub&                 metrics_hub);

/// Fills the given DU high wrapper configuration.
void fill_du_high_wrapper_config(du_high_wrapper_config&        out_cfg,
                                 const du_high_unit_config&     du_high_unit_cfg,
                                 unsigned                       du_id,
                                 du_high_executor_mapper&       execution_mapper,
                                 srs_du::f1c_connection_client& f1c_client_handler,
                                 srs_du::f1u_du_gateway&        f1u_gw,
                                 timer_manager&                 timer_mng,
                                 mac_pcap&                      mac_p,
                                 rlc_pcap&                      rlc_p,
                                 e2_connection_client&          e2_client_handler,
                                 e2_metric_connector_manager&   e2_metric_connectors,
                                 metrics_hub&                   metrics_hub);

} // namespace srsran
