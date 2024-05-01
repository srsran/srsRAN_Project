/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/upper/upper_phy_factories.h"

namespace srsran {

/// Parameters used to instantiate the DU-low.
struct du_low_config {
  /// Logger used to report state of the DU-low.
  srslog::basic_logger* logger;
  /// Parameters forwarded to DL processor factory.
  downlink_processor_factory_sw_config dl_proc_cfg;
  /// Upper PHY configuration.
  std::vector<upper_phy_config> upper_phy;
};

} // namespace srsran