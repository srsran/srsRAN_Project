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

namespace srsran {

namespace app_services {
class metrics_notifier;
}

namespace srs_du {
class du_high_executor_mapper;
struct du_high_wrapper_config;
struct du_high_wrapper_dependencies;
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

/// Fills the given DU high wrapper configuration.
std::pair<std::vector<app_services::metrics_config>, std::vector<std::unique_ptr<app_services::application_command>>>

fill_du_high_wrapper_config(srs_du::du_high_wrapper_config&  out_cfg,
                            const du_high_unit_config&       du_high_unit_cfg,
                            unsigned                         du_idx,
                            srs_du::du_high_executor_mapper& execution_mapper,
                            srs_du::f1c_connection_client&   f1c_client_handler,
                            srs_du::f1u_du_gateway&          f1u_gw,
                            timer_manager&                   timer_mng,
                            mac_pcap&                        mac_p,
                            rlc_pcap&                        rlc_p,
                            e2_connection_client&            e2_client_handler,
                            e2_du_metrics_connector_manager& e2_metric_connectors,
                            srslog::sink&                    json_sink,
                            app_services::metrics_notifier&  metrics_notifier);

} // namespace srsran
