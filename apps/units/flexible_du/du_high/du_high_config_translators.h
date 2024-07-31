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

#include "du_high_config.h"
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

/// Converts and returns the given gnb application configuration to a DU slice RRM policy configuration list.
std::vector<slice_rrm_policy_config>
generate_du_slicing_rrm_policy_config(span<const std::string>                    plmns,
                                      span<const du_high_unit_cell_slice_config> slice_cfg,
                                      unsigned                                   nof_cell_crbs);

} // namespace srsran
