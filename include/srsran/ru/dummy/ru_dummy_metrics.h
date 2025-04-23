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
