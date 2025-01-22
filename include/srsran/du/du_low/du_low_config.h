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

#include "srsran/phy/upper/upper_phy_factories.h"

namespace srsran {
namespace srs_du {

/// DU low cell configuration
struct du_low_cell_config {
  /// Parameters forwarded to DL processor factory.
  downlink_processor_factory_sw_config dl_proc_cfg;
  /// Upper PHY configuration.
  upper_phy_config upper_phy_cfg;
};

/// Parameters used to instantiate the DU-low.
struct du_low_config {
  /// Logger used to report state of the DU-low.
  srslog::basic_logger* logger;
  /// Cell configuration.
  std::vector<du_low_cell_config> cells;
};

/// Dependencies necessary to instantiate a DU-low.
struct du_low_dependencies {
  /// Upper physical layer metric producer. Set to \c nullptr to disable PHY metrics.
  upper_phy_metrics_notifiers* metric_notifier = nullptr;
};

} // namespace srs_du
} // namespace srsran
