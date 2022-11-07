/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/span.h"
#include "srsgnb/cu_cp/cu_cp_configuration.h"
#include "srsgnb/cu_cp/cu_cp_configuration_helpers.h"
#include "srsgnb/du/du_cell_config.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include "srsgnb/du/du_cell_config_validation.h"
#include "srsgnb/phy/lower/lower_phy_configuration.h"
#include "srsgnb/phy/upper/upper_phy_factories.h"
#include "srsgnb/radio/radio_configuration.h"
#include <vector>

namespace srsgnb {

struct gnb_appconfig;

/// Converts and returns the given gnb application configuration to a CU-CP configuration.
srs_cu_cp::cu_cp_configuration generate_cu_cp_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application configuration to a DU cell configuration.
std::vector<du_cell_config> generate_du_cell_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application configuration to an upper PHY configuration.
std::vector<upper_phy_config> generate_du_low_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application configuration to a lower PHY configuration.
lower_phy_configuration generate_ru_config(const gnb_appconfig& config);

/// Converts and returns the given gnb application configuration to a radio configuration.
radio_configuration::radio generate_radio_config(const gnb_appconfig& config);

} // namespace srsgnb
