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
#include "srsran/du/du_cell_config.h"
#include "srsran/du/du_qos_config.h"
#include "srsran/du/du_srb_config.h"
#include "srsran/e2/e2ap_configuration.h"
#include "srsran/mac/mac_config.h"
#include "srsran/ran/five_qi.h"
#include "srsran/ran/lcid.h"

#include <map>

namespace srsran {

struct du_high_unit_config;

/// Converts and returns the given gnb application configuration to a DU cell configuration.
std::vector<du_cell_config> generate_du_cell_config(const du_high_unit_config& config);

/// Converts and returns the given gnb application QoS configuration to a DU QoS list configuration.
std::map<five_qi_t, du_qos_config> generate_du_qos_config(const du_high_unit_config& config);

/// Converts and returns the given gnb application QoS configuration to a DU SRB list configuration.
std::map<srb_id_t, du_srb_config> generate_du_srb_config(const du_high_unit_config& config);

/// Converts and returns the given gnb application configuration to a mac expert configuration.
mac_expert_config generate_mac_expert_config(const du_high_unit_config& config);

/// Converts and returns the given gnb application configuration to a scheduler expert configuration.
scheduler_expert_config generate_scheduler_expert_config(const du_high_unit_config& config);

/// Converts and returns the given gnb application configuration to a E2 configuration.
e2ap_configuration generate_e2_config(const du_high_unit_config& du_high);

/// Augments RLC parameters based on NTN configuration.
void ntn_augment_rlc_parameters(const ntn_config& ntn_cfg, std::map<srb_id_t, du_srb_config>& srb_cfgs);

} // namespace srsran