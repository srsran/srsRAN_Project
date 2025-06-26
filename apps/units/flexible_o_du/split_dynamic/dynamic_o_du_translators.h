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
#include "srsran/adt/span.h"
#include "srsran/ru/dummy/ru_dummy_configuration.h"

namespace srsran {

namespace srs_du {
struct du_cell_config;
}

struct dynamic_o_du_unit_config;
struct ru_dummy_unit_config;
struct worker_manager_config;

/// Generates the dummy RU configuration from the given application unit configuration.
ru_dummy_configuration generate_ru_dummy_config(const ru_dummy_unit_config&                      ru_cfg,
                                                span<const flexible_o_du_ru_config::cell_config> du_cells,
                                                unsigned max_processing_delay_slots,
                                                unsigned nof_prach_ports);

/// Fills the dynamic_du worker manager parameters of the given worker manager configuration.
void fill_dynamic_du_worker_manager_config(worker_manager_config& config, const dynamic_o_du_unit_config& unit_cfg);

} // namespace srsran
