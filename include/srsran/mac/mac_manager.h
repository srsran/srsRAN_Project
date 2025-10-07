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
