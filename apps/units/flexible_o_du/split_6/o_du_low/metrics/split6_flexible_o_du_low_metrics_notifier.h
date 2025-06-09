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

/// Split 6 flexible O-DU low metrics notifier.
class split6_flexible_o_du_low_metrics_notifier
{
public:
  virtual ~split6_flexible_o_du_low_metrics_notifier() = default;

  /// Notifies new split 6 flexible O-DU low metrics.
  virtual void on_new_metrics(const split6_flexible_o_du_low_metrics& metrics) = 0;
};

} // namespace srsran
