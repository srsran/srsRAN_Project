/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/mac/cell_configuration.h"
#include "srsran/mac/mac_ue_configurator.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

/// Start configured cell.
struct mac_cell_start {
  du_cell_index_t cell_index;
  slot_point      slot;
};

/// Network slice configuration
struct mac_slice_configuration {
  // TODO: Fill remaining fields
};

/// MAC cell positioning measurement request.
struct mac_cell_positioning_measurement_request {
  /// UE identifier in case the SRSConfig belongs to a UE connected to this DU. Otherwise, it is empty.
  std::optional<du_ue_index_t> ue_index;
  std::optional<rnti_t>        rnti;
  /// SRS resources to measure.
  srs_config srs_to_meas;
};

/// MAC cell positioning measurement response.
struct mac_cell_positioning_measurement_response {
  std::vector<phy_time_unit> ul_rtoas;
};

/// Reconfiguration of a MAC cell during its operation.
struct mac_cell_reconfig_request {
  /// If not empty, contains the new system information to broadcast.
  std::optional<mac_cell_sys_info_config> new_sys_info;
  /// If not empty, the MAC is requested to collect a new positioning measurement.
  std::optional<mac_cell_positioning_measurement_request> positioning;
};

struct mac_cell_reconfig_response {
  /// Whether a pending SI reconfiguration was successful.
  bool                                                     si_updated = false;
  std::optional<mac_cell_positioning_measurement_response> positioning;
};

/// Interface used to handle a MAC cell activation/reconfiguration/deactivation.
class mac_cell_controller
{
public:
  virtual ~mac_cell_controller() = default;

  /// Start the cell.
  virtual async_task<void> start() = 0;

  /// Stop the cell.
  virtual async_task<void> stop() = 0;

  /// Reconfigure operation cell.
  virtual async_task<mac_cell_reconfig_response> reconfigure(const mac_cell_reconfig_request& request) = 0;
};

/// Contains the mapping between slot_point and time_point.
struct mac_cell_slot_time_info {
  /// Indicates the current slot.
  slot_point sl_tx;
  /// Indicates the system time point associated to the current slot.
  std::chrono::time_point<std::chrono::system_clock> time_point;
};

/// Interface used to retrieve slot_point-to-time_point mapping.
class mac_cell_time_mapper
{
public:
  using time_point                = std::chrono::time_point<std::chrono::system_clock>;
  virtual ~mac_cell_time_mapper() = default;

  /// \brief Provides the last slot point to time point mapping.
  /// This function returns empty optional if the mapping is not available.
  /// \return Mapping between slot point and time point or empty.
  virtual std::optional<mac_cell_slot_time_info> get_last_mapping() const = 0;

  /// \brief Provides time point for the given slot point.
  /// This function returns empty optional if the mapping is not available or if the slot point is not initialized.
  /// \return Time point for the given slot point or empty.
  virtual std::optional<time_point> get_time_point(slot_point slot) const = 0;

  /// \brief Provides slot point for the given time point.
  /// This function returns empty optional if the mapping is not available or if the time point is not initialized.
  /// \return Slot point for the given time point or empty.
  virtual std::optional<slot_point> get_slot_point(time_point time) const = 0;
};

/// Class used to setup the MAC cells and slices.
class mac_cell_manager
{
public:
  virtual ~mac_cell_manager() = default;

  /// Add new cell configuration.
  virtual mac_cell_controller& add_cell(const mac_cell_creation_request& cell_cfg) = 0;

  /// Remove an existing cell configuration.
  virtual void remove_cell(du_cell_index_t cell_index) = 0;

  /// Fetch MAC cell state controller.
  virtual mac_cell_controller& get_cell_controller(du_cell_index_t cell_index) = 0;

  /// Fetch MAC cell time-slot mapper.
  virtual mac_cell_time_mapper& get_time_mapper(du_cell_index_t cell_index) = 0;
};

} // namespace srsran
