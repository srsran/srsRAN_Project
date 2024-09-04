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

#include "apps/services/worker_manager.h"
#include "apps/units/flexible_du/du_unit.h"

namespace srsran {

namespace app_services {
class metrics_notifier;
}

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

du_unit create_du(const dynamic_du_unit_config& dyn_du_cfg, du_unit_dependencies& dependencies);

} // namespace srsran
