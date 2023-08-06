/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "gnb_appconfig.h"
#include "gnb_worker_manager.h"
#include "helpers/metrics_hub.h"
#include "srsran/du/du.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/e2/e2_du_metrics_manager.h"

namespace srsran {

class upper_phy_rg_gateway;
class upper_phy_rx_symbol_request_notifier;
class f1ap_message_notifier;
class gnb_console_helper;
class timer_manager;
class mac_pcap;
class e2_connection_client;

namespace srs_du {
class f1c_connection_client;
class f1u_du_gateway;
} // namespace srs_du

/// \brief Instanties a list of Distributed Unit (DU) given a gNB application configuration.
std::vector<std::unique_ptr<du>>
make_gnb_dus(const gnb_appconfig&                                 gnb_cfg,
             worker_manager&                                      workers,
             upper_phy_rg_gateway&                                rg_gateway,
             upper_phy_rx_symbol_request_notifier&                rx_symbol_request_notifier,
             srs_du::f1c_connection_client&                       f1c_client_handler,
             srs_du::f1u_du_gateway&                              f1u_gw,
             timer_manager&                                       timer_mng,
             mac_pcap&                                            mac_p,
             gnb_console_helper&                                  console_helper,
             e2_connection_client&                                e2_client_handler,
             std::vector<std::unique_ptr<e2_du_metrics_manager>>& e2_du_metric_managers,
             metrics_hub&                                         metrics_hub);

} // namespace srsran
