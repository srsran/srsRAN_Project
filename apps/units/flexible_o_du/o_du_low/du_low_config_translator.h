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

#include "o_du_low_unit_factory_config.h"
#include "srsran/adt/span.h"
#include "srsran/du/du_low/o_du_low_config.h"

namespace srsran {

namespace srs_du {
struct du_cell_config;
struct du_low_config;
} // namespace srs_du

struct du_low_unit_config;
struct worker_manager_config;

/// Generates O-DU low configuration from the given parameters.
void generate_o_du_low_config(srs_du::o_du_low_config&                        out_config,
                              const du_low_unit_config&                       du_low_unit_cfg,
                              span<const o_du_low_unit_config::du_low_config> cells);

/// Fills the DU low worker manager parameters of the given worker manager configuration.
void fill_du_low_worker_manager_config(worker_manager_config&    config,
                                       const du_low_unit_config& unit_cfg,
                                       unsigned                  is_blocking_mode_active,
                                       span<const unsigned>      nof_dl_antennas,
                                       span<const unsigned>      nof_ul_antennas);

} // namespace srsran
