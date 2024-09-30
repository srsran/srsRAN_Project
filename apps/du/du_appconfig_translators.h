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

#include "srsran/e2/e2ap_configuration.h"
#include "srsran/gateways/sctp_network_gateway.h"

namespace srsran {

struct du_appconfig;
struct worker_manager_config;

/// Converts and returns the given gnb application configuration to a E2AP Network Gateway configuration.
sctp_network_connector_config generate_e2ap_nw_config(const du_appconfig& config, int ppid);

/// Fills the DU worker manager parameters of the given worker manager configuration.
void fill_du_worker_manager_config(worker_manager_config& config, const du_appconfig& unit_cfg);

} // namespace srsran