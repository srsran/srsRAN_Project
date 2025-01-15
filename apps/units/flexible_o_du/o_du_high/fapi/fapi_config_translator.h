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

namespace srsran {

struct fapi_unit_config;
struct worker_manager_config;

/// Fills the FAPI worker manager parameters of the given worker manager configuration.
void fill_fapi_worker_manager_config(worker_manager_config&  config,
                                     const fapi_unit_config& unit_cfg,
                                     unsigned                nof_cells);

} // namespace srsran
