/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ru/ofh/ru_ofh_configuration.h"

namespace srsran {

namespace srs_du {
struct du_cell_config;
}

struct ru_ofh_unit_config;
struct worker_manager_config;

/// Converts and returns the given Open Fronthaul Radio Unit application unit configuration to a Open Fronthaul Radio
/// Unit configuration.
ru_ofh_configuration generate_ru_ofh_config(const ru_ofh_unit_config&          ru_cfg,
                                            span<const srs_du::du_cell_config> du_cells,
                                            unsigned                           max_processing_delay_slots);

/// Fills the OFH worker manager parameters of the given worker manager configuration.
void fill_ofh_worker_manager_config(worker_manager_config&             config,
                                    const ru_ofh_unit_config&          ru_cfg,
                                    span<const srs_du::du_cell_config> du_cells);

} // namespace srsran
