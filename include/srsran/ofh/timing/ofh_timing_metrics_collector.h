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

namespace srsran {
namespace ofh {

struct timing_metrics;

/// Open Fronthaul timing metrics collector.
class timing_metrics_collector
{
public:
  virtual ~timing_metrics_collector() = default;

  /// Collects the timing metrics and writes them into the given metrics structure.
  virtual void collect_metrics(timing_metrics& metrics) = 0;
};

} // namespace ofh
} // namespace srsran
