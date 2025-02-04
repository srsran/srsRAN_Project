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

struct o_du_low_metrics;

/// O-RAN DU low metrics collector used for collecting metrics from the O-DU low.
class o_du_low_metrics_collector
{
public:
  virtual ~o_du_low_metrics_collector() = default;

  /// Collects the metrics from the O-DU low and writes them into the given metrics argument.
  virtual void collect_o_du_low_metrics(o_du_low_metrics& metrics) = 0;
};

} // namespace srs_du
} // namespace srsran
