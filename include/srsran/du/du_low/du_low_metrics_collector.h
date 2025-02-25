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
namespace srs_du {

struct du_low_metrics;

/// DU low metrics collector.
class du_low_metrics_collector
{
public:
  virtual ~du_low_metrics_collector() = default;

  /// Collect the metrics of this DU low.
  virtual void collect_metrics(srs_du::du_low_metrics& metrics) = 0;
};

} // namespace srs_du
} // namespace srsran
