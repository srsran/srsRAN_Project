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

#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/cu_up/cu_up_configuration.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/du/du_qos_config.h"
#include "srsran/du/du_srb_config.h"
#include "srsran/e2/e2ap_configuration.h"
#include "srsran/gateways/sctp_network_gateway.h"
#include "srsran/mac/mac_config.h"
#include "srsran/phy/upper/upper_phy_factories.h"
#include "srsran/ru/ru_configuration.h"
#include <map>
#include <vector>

namespace srsran {
struct du_high_unit_cell_config;

struct cu_cp_unit_amf_config;
struct cu_cp_unit_config;
struct cu_up_unit_config;
struct du_high_unit_config;
struct du_low_unit_config;
struct dynamic_du_unit_config;
struct gnb_appconfig;
struct rlc_am_appconfig;
struct mac_lc_appconfig;

/// Converts and returns the subcarrier spacing.
subcarrier_spacing generate_subcarrier_spacing(unsigned sc_spacing);

/// Converts and returns the given gnb application configuration to a NGAP Network Gateway configuration.
srsran::sctp_network_connector_config generate_ngap_nw_config(const cu_cp_unit_amf_config& config);

/// Converts and returns the given gnb application configuration to a E2AP Network Gateway configuration.
srsran::sctp_network_connector_config generate_e2ap_nw_config(const gnb_appconfig& config, int ppid);

} // namespace srsran
