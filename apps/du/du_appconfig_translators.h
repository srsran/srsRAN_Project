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

#include "srsran/e2/e2ap_configuration.h"
#include "srsran/gateways/sctp_network_gateway.h"

namespace srsran {

struct du_appconfig;
struct worker_manager_config;
struct e2_appconfig;

/// Converts and returns the given gnb application configuration to a E2AP Network Gateway configuration.
sctp_network_connector_config generate_e2ap_nw_config(const e2_appconfig& config, int ppid);

/// Fills the DU worker manager parameters of the given worker manager configuration.
void fill_du_worker_manager_config(worker_manager_config& config, const du_appconfig& unit_cfg);

} // namespace srsran
