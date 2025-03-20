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

struct data_flow_perf_metrics;

/// Open Fronthaul downlink data flow performance metrics collector interface.
class data_flow_downlink_metrics_collector
{
public:
  virtual ~data_flow_downlink_metrics_collector() = default;

  /// Collect the performance metrics of a data flow.
  virtual void collect_metrics(data_flow_perf_metrics& metric) = 0;
};

} // namespace ofh
} // namespace srsran
