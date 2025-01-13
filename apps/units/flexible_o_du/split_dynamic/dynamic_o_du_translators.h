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
ru_dummy_configuration generate_ru_dummy_config(const ru_dummy_unit_config&        ru_cfg,
                                                span<const srs_du::du_cell_config> du_cells,
                                                unsigned                           max_processing_delay_slots,
                                                unsigned                           nof_prach_ports);

/// Fills the dynamic_du worker manager parameters of the given worker manager configuration.
void fill_dynamic_du_worker_manager_config(worker_manager_config& config, const dynamic_o_du_unit_config& unit_cfg);

} // namespace srsran
