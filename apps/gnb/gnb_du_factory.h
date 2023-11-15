/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "gnb_appconfig.h"
#include "gnb_worker_manager.h"
#include "helpers/metrics_hub.h"
#include "srsran/du/du.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/pcap/rlc_pcap.h"

namespace srsran {

class upper_phy_rg_gateway;
class upper_phy_rx_symbol_request_notifier;
class f1ap_message_notifier;
class gnb_console_helper;
class timer_manager;
class mac_pcap;
class e2_connection_client;
class e2_metric_connector_manager;

namespace srs_du {
class f1c_connection_client;
class f1u_du_gateway;
} // namespace srs_du

/// \brief Instanties a list of Distributed Unit (DU) given a gNB application configuration.
std::vector<std::unique_ptr<du>> make_gnb_dus(const gnb_appconfig&                  gnb_cfg,
                                              span<du_cell_config>                  du_cells,
                                              worker_manager&                       workers,
                                              upper_phy_rg_gateway&                 rg_gateway,
                                              upper_phy_rx_symbol_request_notifier& rx_symbol_request_notifier,
                                              srs_du::f1c_connection_client&        f1c_client_handler,
                                              srs_du::f1u_du_gateway&               f1u_gw,
                                              timer_manager&                        timer_mng,
                                              mac_pcap&                             mac_p,
                                              rlc_pcap&                             rlc_p,
                                              gnb_console_helper&                   console_helper,
                                              e2_connection_client&                 e2_client_handler,
                                              e2_metric_connector_manager&          e2_metric_connectors,
                                              metrics_hub&                          metrics_hub);

} // namespace srsran
