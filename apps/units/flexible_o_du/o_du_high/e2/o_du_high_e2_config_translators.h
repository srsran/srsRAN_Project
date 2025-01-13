/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

namespace srsran {

struct o_du_high_e2_config;
struct worker_manager_config;

/// Converts and returns the given gnb application configuration to a E2 configuration.
e2ap_configuration generate_e2_config(const o_du_high_e2_config& config,
                                      gnb_id_t                   gnb_id,
                                      const std::string&         plmn,
                                      std::optional<gnb_du_id_t> gnb_du_id);

/// Fills the O-RAN DU high E2 worker manager parameters of the given worker manager configuration.
void fill_o_du_high_e2_worker_manager_config(worker_manager_config& config, const o_du_high_e2_config& unit_cfg);

} // namespace srsran
