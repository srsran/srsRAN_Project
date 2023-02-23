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

#include "srsran/adt/span.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/cu_cp/cu_cp_configuration_helpers.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/du/du_cell_config_validation.h"
#include "srsran/du/du_qos_config.h"
#include "srsran/phy/lower/lower_phy_configuration.h"
#include "srsran/phy/upper/upper_phy_factories.h"
#include "srsran/radio/radio_configuration.h"
#include <map>
#include <vector>

namespace srsran {

struct gnb_appconfig;

/// Converts and returns the given gnb application configuration to a CU-CP configuration.
srs_cu_cp::cu_cp_configuration generate_cu_cp_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application configuration to a DU cell configuration.
std::vector<du_cell_config> generate_du_cell_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application QoS configuration to a CU-CP configuration.
std::map<uint8_t, srs_cu_cp::cu_cp_qos_config> generate_cu_cp_qos_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application QoS configuration to a DU configuration.
std::map<uint8_t, du_qos_config> generate_du_qos_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application configuration to a scheduler expert configuration.
scheduler_expert_config generate_scheduler_expert_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application configuration to an upper PHY configuration.
std::vector<upper_phy_config> generate_du_low_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application configuration to a lower PHY configuration.
lower_phy_configuration generate_ru_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application configuration to a radio configuration and validates it.
radio_configuration::radio generate_radio_config(const gnb_appconfig&                  config,
                                                 const radio_configuration::validator& validator);

} // namespace srsran
