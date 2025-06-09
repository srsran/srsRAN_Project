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

struct split6_flexible_o_du_low_metrics;

/// Split 6 O-DU low metrics collector.
class split6_o_du_low_metrics_collector
{
public:
  virtual ~split6_o_du_low_metrics_collector() = default;

  /// Collects the split 6 flexible O-DU low metrics in the given struct.
  virtual void collect_metrics(split6_flexible_o_du_low_metrics& metrics) = 0;
};

} // namespace srsran
