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
  std::atomic<uint64_t> nof_skipped_symbols               = {0};
  std::atomic<unsigned> max_nof_continuous_skipped_symbol = {0};
};

} // namespace ofh
} // namespace srsran
