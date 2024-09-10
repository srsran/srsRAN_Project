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

#include "srsran/ru/ru_generic_configuration.h"

namespace srsran {

namespace srs_du {
struct du_cell_config;
}
struct ru_sdr_unit_config;

/// Converts and returns the given RU SDR application unit configuration to a SDR RU configuration.
ru_generic_configuration generate_ru_sdr_config(const ru_sdr_unit_config&          ru_cfg,
                                                span<const srs_du::du_cell_config> du_cells,
                                                unsigned                           max_processing_delay_slots);

} // namespace srsran
