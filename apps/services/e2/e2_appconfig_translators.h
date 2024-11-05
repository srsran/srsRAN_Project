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

#include "srsran/gateways/sctp_network_gateway.h"

namespace srsran {

struct e2_appconfig;

/// Converts and returns the given gnb application configuration to a E2AP Network Gateway configuration.
sctp_network_connector_config generate_e2ap_nw_config(const e2_appconfig& config, int ppid);

} // namespace srsran
