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

#include "srsran/phy/upper/upper_phy_metrics.h"

namespace srsran {
namespace srs_du {

/// DU low metrics.
struct du_low_metrics {
  /// Sector metrics.
  upper_phy_metrics upper_metrics;
};

} // namespace srs_du
} // namespace srsran
