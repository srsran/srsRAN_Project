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

#include "srsran/ran/gnb_constants.h"

namespace srsran {

/// Generic Radio Unit sector metrics.
struct ru_generic_sector_metrics {
  unsigned sector_id;
  float    tx_avg_power_dB;
  float    tx_peak_power_dB;
  float    tx_papr_dB;
  float    tx_clipping_prob = std::numeric_limits<double>::quiet_NaN();
  float    rx_avg_power_dB;
  float    rx_peak_power_dB;
  float    rx_papr_dB;
  double   rx_clipping_prob = std::numeric_limits<double>::quiet_NaN();
};

/// Generic Radio Unit metrics.
struct ru_generic_metrics {
  static_vector<ru_generic_sector_metrics, MAX_CELLS_PER_DU> cells;
};

} // namespace srsran
