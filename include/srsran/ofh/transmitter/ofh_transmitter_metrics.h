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

#include "ofh_transmitter_data_flow_metrics.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul transmitter downlink processing metrics.
struct transmitter_dl_metrics {
  /// OFH transmitter downlink C-Plane processing metrics.
  data_flow_perf_metrics dl_cp_metrics;
  /// OFH transmitter U-Plane processing metrics.
  data_flow_perf_metrics dl_up_metrics;
  /// Number of late downlink resource grids received from the PHY.
  unsigned nof_late_dl_grids;
};

/// Open Fronthaul transmitter uplink requests processing metrics.
struct transmitter_ul_metrics {
  /// Open Fronthaul transmitter uplink C-Plane processing metrics.
  data_flow_perf_metrics ul_cp_metrics;
  /// Number of late uplink request.
  unsigned nof_late_ul_requests;
};

/// Open Fronthaul transmitter metrics.
struct transmitter_metrics {
  transmitter_dl_metrics dl_metrics;
  transmitter_ul_metrics ul_metrics;
};

} // namespace ofh
} // namespace srsran
