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

struct sector_metrics;

/// Open Fronthaul sector metrics collector interface used to collect metrics from a sector.
class metrics_collector
{
public:
  virtual ~metrics_collector() = default;

  /// Collect the metrics of an Open Fronthaul sector.
  virtual void collect_metrics(sector_metrics& metric) = 0;
};

} // namespace ofh
} // namespace srsran
