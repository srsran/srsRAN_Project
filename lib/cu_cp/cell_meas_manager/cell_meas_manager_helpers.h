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

#include "measurement_context.h"
#include "srsran/cu_cp/cell_meas_manager_config.h"
#include "srsran/rrc/meas_types.h"
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

/// \brief Log configured cells.
void log_cells(const srslog::basic_logger& logger, const cell_meas_manager_cfg& cfg);

/// \brief Log measurement objects.
void log_meas_objects(const srslog::basic_logger&                                 logger,
                      const std::unordered_map<ssb_frequency_t, rrc_meas_obj_nr>& meas_objects);

/// \brief Add old measurement configuration to the remove list of the new configuration.
/// \param[in] old_cfg The old configuration.
/// \param[out] new_cfg The new configuration.
void add_old_meas_config_to_rem_list(const rrc_meas_cfg& old_cfg, rrc_meas_cfg& new_cfg);

/// \brief Generate measurement objects for the given cell configuration.
/// \param[in] cfg The cell configuration.
/// \param[in] nci The cell id.
/// \returns A vector of SSB frequencies that correlate to measurement objects.
std::vector<ssb_frequency_t> generate_measurement_object_list(const cell_meas_manager_cfg& cfg,
                                                              nr_cell_identity             serving_nci);

/// \brief Generate report configuration for the given cell configuration.
/// \param[in] cfg The cell configuration.
/// \param[in] nci The cell id.
/// \param[in] report_cfg_id The report configuration id.
/// \param[out] meas_cfg The resulting measurement configuration.
/// \param[out] ue_meas_context The UE measurement context.
void generate_report_config(const cell_meas_manager_cfg&  cfg,
                            const nr_cell_identity        nci,
                            const report_cfg_id_t         report_cfg_id,
                            rrc_meas_cfg&                 meas_cfg,
                            cell_meas_manager_ue_context& ue_meas_context);

rrc_meas_obj_nr generate_measurement_object(const serving_cell_meas_config& cfg);

/// \brief Check whether the given measurement objects are the same.
bool is_duplicate(const rrc_meas_obj_nr& obj_1, const rrc_meas_obj_nr& obj_2);

} // namespace srs_cu_cp
} // namespace srsran
