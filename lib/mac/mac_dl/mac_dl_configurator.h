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

namespace srsran {

/// Notifier used by MAC DL to forward cell metric reports.
class mac_cell_metric_notifier
{
public:
  virtual ~mac_cell_metric_notifier() = default;

  /// \brief Called when a new cell metric report is ready.
  virtual void on_cell_metric_report(const mac_dl_cell_metric_report& report) = 0;
};

/// \brief Configuration of a MAC cell metric reporting.
struct mac_cell_metric_report_config {
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
  virtual mac_cell_controller& add_cell(const mac_cell_creation_request&     cell_cfg,
                                        const mac_cell_metric_report_config& metrics) = 0;

  /// Remove an existing cell configuration.
  virtual void remove_cell(du_cell_index_t cell_index) = 0;

  /// Fetch MAC cell state controller.
  virtual mac_cell_controller& get_cell_controller(du_cell_index_t cell_index) = 0;
};

} // namespace srsran
