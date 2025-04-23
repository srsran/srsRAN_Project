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

#include "srsran/adt/static_vector.h"
#include "srsran/ran/gnb_constants.h"
#include <limits>

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

/// Radio metrics.
struct radio_metrics {
  unsigned late_count      = 0;
  unsigned underflow_count = 0;
  unsigned overflow_count  = 0;
};

/// Generic Radio Unit metrics.
struct ru_generic_metrics {
  static_vector<ru_generic_sector_metrics, MAX_CELLS_PER_DU> cells;
  radio_metrics                                              radio;
};

} // namespace srsran
