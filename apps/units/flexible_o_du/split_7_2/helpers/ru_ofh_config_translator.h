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

#include "apps/units/flexible_o_du/split_helpers/flexible_o_du_configs.h"
#include "srsran/ru/ofh/ru_ofh_configuration.h"

namespace srsran {

struct ru_ofh_unit_config;
struct worker_manager_config;

/// Converts and returns the given Open Fronthaul Radio Unit application unit configuration to a Open Fronthaul Radio
/// Unit configuration.
ru_ofh_configuration generate_ru_ofh_config(const ru_ofh_unit_config&                        ru_cfg,
                                            span<const flexible_o_du_ru_config::cell_config> cells,
                                            unsigned max_processing_delay_slots);

/// Fills the OFH worker manager parameters of the given worker manager configuration.
void fill_ofh_worker_manager_config(worker_manager_config& config, const ru_ofh_unit_config& ru_cfg);

} // namespace srsran
