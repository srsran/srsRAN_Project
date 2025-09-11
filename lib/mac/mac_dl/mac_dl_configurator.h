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

#include "srsran/mac/mac_cell_manager.h"
#include "srsran/mac/mac_clock_controller.h"
#include "srsran/mac/mac_metrics.h"

namespace srsran {

/// Notifier used by MAC DL to forward cell metric reports.
class mac_cell_metric_notifier
{
public:
  virtual ~mac_cell_metric_notifier() = default;

  /// \brief Polling on whether a new MAC cell metric report is required.
  virtual bool is_report_required(slot_point slot_tx) = 0;

  /// \brief Called when a new cell is activated.
  virtual void on_cell_activation() = 0;

  /// \brief Called when a cell is deactivated and provides the last report.
  virtual void on_cell_deactivation(const mac_dl_cell_metric_report& report) = 0;

  /// \brief Called when a new cell metric report is ready.
  virtual void on_cell_metric_report(const mac_dl_cell_metric_report& report) = 0;
};

/// \brief Dependencies between a MAC cell and remaining components of the MAC.
struct mac_cell_config_dependencies {
  /// Timer source for the cell.
  std::unique_ptr<mac_cell_clock_controller> timer_source;
  /// \brief Period of the metric reporting.
  std::chrono::milliseconds report_period{0};
  /// \brief Pointer to the MAC cell metric notifier.
  mac_cell_metric_notifier* notifier = nullptr;
};

/// Configurator of MAC cells in the MAC DL processor.
class mac_dl_cell_manager
{
public:
  virtual ~mac_dl_cell_manager() = default;

  /// Add new cell and set its configuration.
  virtual mac_cell_controller& add_cell(const mac_cell_creation_request& cell_cfg,
                                        mac_cell_config_dependencies     deps) = 0;

  /// Remove an existing cell configuration.
  virtual void remove_cell(du_cell_index_t cell_index) = 0;

  /// Fetch MAC cell state controller.
  virtual mac_cell_controller& get_cell_controller(du_cell_index_t cell_index) = 0;
};

} // namespace srsran
