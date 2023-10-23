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

#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/cu_up/cu_up_configuration.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/du/du_qos_config.h"
#include "srsran/e2/e2ap_configuration.h"
#include "srsran/gateways/sctp_network_gateway.h"
#include "srsran/mac/mac_config.h"
#include "srsran/phy/upper/upper_phy_factories.h"
#include "srsran/ru/ru_configuration.h"
#include <map>
#include <vector>

namespace srsran {

struct gnb_appconfig;

/// This function takes the appconfig and generates the values for the parameters set to be auto-configured.
void derive_auto_params(gnb_appconfig& gnb_params);

/// Converts and returns SSB periodicity, offset and duration into a valid SSB measurement and timing configuration.
srs_cu_cp::rrc_ssb_mtc generate_rrc_ssb_mtc(unsigned period, unsigned offset, unsigned duration);

/// Converts and returns the subcarrier spacing.
subcarrier_spacing generate_subcarrier_spacing(unsigned sc_spacing);

/// Converts and returns the given gnb application configuration to a NGAP Network Gateway configuration.
srsran::sctp_network_gateway_config generate_ngap_nw_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application configuration to a CU-CP configuration.
srs_cu_cp::cu_cp_configuration generate_cu_cp_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application configuration to a CU-UP configuration.
srs_cu_up::cu_up_configuration generate_cu_up_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application configuration to a DU cell configuration.
std::vector<du_cell_config> generate_du_cell_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application QoS configuration to a CU-CP configuration.
std::map<five_qi_t, srs_cu_cp::cu_cp_qos_config> generate_cu_cp_qos_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application QoS configuration to a DU configuration.
std::map<five_qi_t, du_qos_config> generate_du_qos_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application configuration to a mac expert configuration.
mac_expert_config generate_mac_expert_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application configuration to a scheduler expert configuration.
scheduler_expert_config generate_scheduler_expert_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application configuration to an upper PHY configuration.
std::vector<upper_phy_config> generate_du_low_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application configuration to a Radio Unit configuration.
ru_configuration generate_ru_config(const gnb_appconfig& config, span<const du_cell_config> cells);

/// Converts and returns the given gnb application configuration to a E2AP Network Gateway configuration.
srsran::sctp_network_gateway_config generate_e2ap_nw_config(const gnb_appconfig& config, int ppid);

/// Converts and returns the given gnb application configuration to a E2 configuration.
e2ap_configuration generate_e2_config(const gnb_appconfig& config);

} // namespace srsran
