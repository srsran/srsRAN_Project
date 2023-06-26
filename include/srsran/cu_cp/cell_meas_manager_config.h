/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "cu_cp_types.h"

namespace srsran {

namespace srs_cu_cp {

/// \brief Cell manager configuration.
struct cell_meas_manager_cfg {
  std::map<nr_cell_id_t, std::vector<nr_cell_id_t>> neighbor_cell_list; ///< List of neighbor cells
  optional<cu_cp_cond_event_a3> a3_event_config; // A3 event config is currently the only supported event.
};

/// \brief Validates configuration. Returns true if config is valid, false otherwise.
bool is_valid_configuration(const cell_meas_manager_cfg& cfg);

} // namespace srs_cu_cp

} // namespace srsran
