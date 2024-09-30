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

class e2_metric_connector_manager;
class e2_connection_client;
class mac_pcap;
class rlc_pcap;
class timer_manager;
struct du_high_unit_config;

/// Set up sources for the DU high metrics.
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
                            e2_metric_connector_manager&     e2_metric_connectors,
                            srslog::sink&                    json_sink,
                            app_services::metrics_notifier&  metrics_notifier);

} // namespace srsran
