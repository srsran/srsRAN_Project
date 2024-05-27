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

#include "srsran/ru/ru_ofh_configuration.h"

namespace srsran {

struct du_cell_config;
struct ru_ofh_unit_config;

/// Converts and returns the given Open Fronthaul Radio Unit application unit configuration to a Open Fronthaul Radio
/// Unit configuration.
ru_ofh_configuration generate_ru_ofh_config(const ru_ofh_unit_config&  ru_cfg,
                                            span<const du_cell_config> du_cells,
                                            unsigned                   max_processing_delay_slots);

} // namespace srsran