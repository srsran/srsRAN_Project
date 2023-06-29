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
#include "srsran/du/du.h"
#include "srsran/du/du_cell_config.h"

namespace srsran {

class upper_phy_rg_gateway;
class upper_phy_rx_symbol_request_notifier;
class f1ap_message_notifier;
class timer_manager;
class mac_pcap;
class scheduler_ue_metrics_notifier;

namespace srs_du {
class f1u_du_gateway;
}

/// \brief Instanties a Distributed Unit (DU) given a gNB application configuration.
std::unique_ptr<du> make_gnb_du(const gnb_appconfig&                  gnb_cfg,
                                worker_manager&                       workers,
                                const std::vector<du_cell_config>&    du_cells,
                                upper_phy_rg_gateway&                 rg_gateway,
                                upper_phy_rx_symbol_request_notifier& rx_symbol_request_notifier,
                                f1ap_message_notifier&                f1ap_pdu_notifier,
                                srs_du::f1u_du_gateway&               f1u_gw,
                                timer_manager&                        timer_mng,
                                mac_pcap&                             mac_p,
                                scheduler_ue_metrics_notifier&        metrics_notifier);

} // namespace srsran