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

#include "srsran/mac/mac_cell_manager.h"
#include "srsran/mac/mac_metrics.h"
#include "srsran/ran/slot_point_extended.h"

namespace srsran {

/// Notifier of subframe starts for a given cell.
class du_cell_timer_source
{
public:
  virtual ~du_cell_timer_source() = default;

  /// Called when a MAC cell gets deactivated and the slot indications stop being received.
  virtual void on_cell_deactivation() = 0;

  /// Called on each new slot indication for a given cell.
  virtual slot_point_extended on_slot_indication(slot_point sl_tx) = 0;

  /// Current HFN, SFN and slot number.
  slot_point_extended now() const { return cached_now; }

protected:
  slot_point_extended cached_now;
};

/// Notifier used by MAC DL to forward cell metric reports.
class mac_cell_metric_notifier
{
public:
  virtual ~mac_cell_metric_notifier() = default;

  /// \brief Polling on whether a new MAC cell metric report is required.
  virtual bool is_report_required(slot_point_extended slot_tx) = 0;

  /// \brief Called when a new cell is activated.
  virtual void on_cell_activation(slot_point_extended slot_tx) = 0;

  /// \brief Called when a cell is deactivated and provides the last report.
  virtual void on_cell_deactivation(const mac_dl_cell_metric_report& report) = 0;

  /// \brief Called when a new cell metric report is ready.
  virtual void on_cell_metric_report(const mac_dl_cell_metric_report& report) = 0;
};

/// \brief Dependencies between a MAC cell and remaining components of the MAC.
struct mac_cell_config_dependencies {
  /// Timer source for the cell.
  std::unique_ptr<du_cell_timer_source> timer_source;
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
