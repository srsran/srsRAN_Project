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

#include "srsran/adt/static_vector.h"
#include "srsran/phy/upper/upper_phy_metrics.h"
#include "srsran/ran/gnb_constants.h"

namespace srsran {
namespace srs_du {

/// DU low metrics.
struct du_low_metrics {
  /// Sector metrics.
  static_vector<upper_phy_metrics, MAX_CELLS_PER_DU> sector_metrics;
};

} // namespace srs_du
} // namespace srsran
