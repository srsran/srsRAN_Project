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

#include "srsran/ofh/timing/ofh_timing_metrics_collector.h"
#include <atomic>

namespace srsran {
namespace ofh {

/// Open Fronthaul timing metrics collector implementation.
class timing_metrics_collector_impl : public timing_metrics_collector
{
public:
  // See interface for documentation.
  void collect_metrics(timing_metrics& metrics) override;

  /// Updates the metrics with the given parameters.
  void update_metrics(unsigned num_skipped_symbols);

private:
  std::atomic<uint64_t> nof_skipped_symbols       = {0};
  std::atomic<unsigned> skipped_symbols_max_burst = {0};
};

} // namespace ofh
} // namespace srsran
