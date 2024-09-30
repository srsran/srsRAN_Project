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

#include "apps/units/flexible_du/du_unit.h"
#include "split_8_du_unit_config.h"

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

du_unit create_split_8_du(const split_8_du_unit_config& du_8_cfg, const du_unit_dependencies& dependencies);

} // namespace srsran
