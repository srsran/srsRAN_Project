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

namespace srsran {

class mac_cell_manager;
class mac_ue_configurator;
class mac_positioning_measurement_handler;

/// Interface used by the management plane of the DU.
class mac_manager
{
public:
  virtual ~mac_manager() = default;

  /// Interface to manage the creation, reconfiguration, deletion, activation and deactivation of cells.
  virtual mac_cell_manager& get_cell_manager() = 0;

  /// Interface to manage the creation, reconfiguration and deletion of UEs in the MAC.
  virtual mac_ue_configurator& get_ue_configurator() = 0;

  /// Fetch positioning measurement handler.
  virtual mac_positioning_measurement_handler& get_positioning_handler() = 0;
};

} // namespace srsran
