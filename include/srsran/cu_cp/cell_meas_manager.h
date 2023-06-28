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

#include "cell_meas_manager_config.h"
#include "cu_cp_types.h"

namespace srsran {
namespace srs_cu_cp {

/// Object to manage cell measurement related tasks including neighbor cell relationships between cells managed by DUs
/// connected to this CU-CP as well as external cells. The cell manager also consumes measurement reports from UEs,
/// processes them and calls the mobility manager in case a handover needs to be performed.
class cell_meas_manager
{
public:
  virtual ~cell_meas_manager() = default;

  /// \brief Retrieve the measurement config (for any UE) connected to the given serving cell.
  virtual cu_cp_meas_cfg get_measurement_config(nr_cell_id_t nci) = 0;

  /// \brief Update the measurement related parameters for the given cell id.
  virtual bool update_cell_config(nr_cell_id_t nci, const cell_meas_cfg& cfg) = 0;

  /// \brief Submit measurement report for given UE to cell manager.
  virtual void report_measurement(const cu_cp_meas_results& meas_results) = 0;
};

/// Creates an instance of an cell manager.
std::unique_ptr<cell_meas_manager> create_cell_meas_manager(const cell_meas_manager_cfg& cfg);

} // namespace srs_cu_cp
} // namespace srsran
