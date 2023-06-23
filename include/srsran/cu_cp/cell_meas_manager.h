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
  cu_cp_cond_event_a3 event_config; // A3 event config is currently the only supported event.
};

/// Object to manage cell measurement related tasks including neighbor cell relationships between cells managed by DUs
/// connected to this CU-CP as well as external cells. The cell manager also consumes measurement reports from UEs,
/// processes them and calls the mobility manager in case a handover needs to be performed.
class cell_meas_manager
{
public:
  virtual ~cell_meas_manager() = default;

  /// \brief Retrieve the measurement config (for any UE) connected to the given serving cell.
  virtual cu_cp_meas_cfg get_measurement_config(const nr_cell_id_t& cid) = 0;

  /// \brief Submit measurement report for given UE to cell manager.
  virtual void report_measurement(/* add report here*/) = 0;
};

/// Creates an instance of an cell manager.
std::unique_ptr<cell_meas_manager> create_cell_meas_manager(const cell_meas_manager_cfg& cfg_);

} // namespace srs_cu_cp

} // namespace srsran
