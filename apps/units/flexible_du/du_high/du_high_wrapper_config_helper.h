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
                               metrics_plotter_json&        metrics_json,
                               metrics_log_helper&          metrics_logger,
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
                                 metrics_log_helper&            metrics_logger,
                                 e2_connection_client&          e2_client_handler,
                                 e2_metric_connector_manager&   e2_metric_connectors,
                                 rlc_metrics_notifier&          rlc_json_metrics,
                                 metrics_hub&                   metrics_hub);

} // namespace srsran
