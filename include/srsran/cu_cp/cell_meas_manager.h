/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
  virtual cu_cp_meas_cfg get_measurement_config(const nr_cell_id_t& cid) = 0;

  /// \brief Submit measurement report for given UE to cell manager.
  virtual void report_measurement(const cu_cp_meas_results& meas_results) = 0;
};

/// Creates an instance of an cell manager.
std::unique_ptr<cell_meas_manager> create_cell_meas_manager(const cell_meas_manager_cfg& cfg);

} // namespace srs_cu_cp
} // namespace srsran
