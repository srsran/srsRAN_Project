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

#include "srsran/du/du_cell_config.h"
#include "srsran/scheduler/scheduler_configurator.h"

namespace srsran {
namespace config_helpers {

/// Builds the list of PUCCH guardbands.
std::vector<sched_grid_resource> build_pucch_guardbands_list(const srs_du::pucch_builder_params& ul_cfg,
                                                             unsigned                            bwp_size);

unsigned compute_tot_nof_monitored_pdcch_candidates_per_slot(const serving_cell_config& ue_cell_cfg,
                                                             const dl_config_common&    dl_cfg_common);
} // namespace config_helpers
} // namespace srsran
