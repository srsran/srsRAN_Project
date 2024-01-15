/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "mobility_manager.h"

namespace srsran {
namespace srs_cu_cp {

/// Methods used by cell measurement manager to signal measurement events to the mobility manager.
class cell_meas_mobility_manager_notifier
{
public:
  virtual ~cell_meas_mobility_manager_notifier() = default;

  /// \brief Notifies that a neighbor cell became stronger than the current serving cell.
  virtual void on_neighbor_better_than_spcell(ue_index_t ue_index, pci_t neighbor_pci) = 0;
};

/// Object to manage cell measurement related tasks including neighbor cell relationships between cells managed by DUs
/// connected to this CU-CP as well as external cells. The cell manager also consumes measurement reports from UEs,
/// processes them and calls the mobility manager in case a handover needs to be performed.
class cell_meas_manager
{
public:
  virtual ~cell_meas_manager() = default;

  /// \brief Retrieve the measurement config (for any UE) connected to the given serving cell.
  /// \param[in] nci The cell id of the serving cell to update.
  /// \param[in] current_meas_config The current meas config of the UE (if applicable).
  virtual optional<rrc_meas_cfg> get_measurement_config(nr_cell_id_t           nci,
                                                        optional<rrc_meas_cfg> current_meas_config = {}) = 0;

  /// \brief Returns the cell config for the given cell id.
  virtual optional<cell_meas_config> get_cell_config(nr_cell_id_t nci) = 0;

  /// \brief Update the measurement related parameters for the given cell id.
  /// \param[in] nci The cell id of the serving cell to update.
  /// \param[in] serv_cell_cfg_ The serving cell meas config to update.
  /// \param[in] ncells_ Optional neigbor cells to replace the current neighbor cells with.
  virtual void update_cell_config(nr_cell_id_t                           nci,
                                  const serving_cell_meas_config&        serv_cell_cfg_,
                                  std::vector<neighbor_cell_meas_config> ncells_ = {}) = 0;

  /// \brief Submit measurement report for given UE to cell manager.
  virtual void report_measurement(const ue_index_t ue_index, const rrc_meas_results& meas_results) = 0;
};

/// Creates an instance of an cell manager.
std::unique_ptr<cell_meas_manager> create_cell_meas_manager(const cell_meas_manager_cfg&         cfg,
                                                            cell_meas_mobility_manager_notifier& mobility_mng_notfier_);

} // namespace srs_cu_cp
} // namespace srsran
