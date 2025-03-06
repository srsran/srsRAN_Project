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
#include "srsran/ran/gnb_constants.h"

namespace srsran {

/// Dummy Radio Unit sector metrics.
struct ru_dummy_sector_metrics {
  /// Sector radio identifier.
  unsigned sector_id;
  /// Total number of UL receive requests.
  uint64_t total_ul_request_count;
  /// Total number of DL transmit requests.
  uint64_t total_dl_request_count;
  /// Total number of PRACH receive requests.
  uint64_t total_prach_request_count;
  /// Number of late UL receive request.
  uint64_t late_ul_request_count;
  /// Number of late DL transmit request.
  uint64_t late_dl_request_count;
  /// Number of late PRACH receive request.
  uint64_t late_prach_request_count;
};

struct ru_dummy_metrics {
  /// Dummy Radio Unit sector metrics.
  static_vector<ru_dummy_sector_metrics, MAX_CELLS_PER_DU> sectors;
};

} // namespace srsran
