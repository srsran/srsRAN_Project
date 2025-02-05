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

struct flexible_o_du_metrics;

/// Flexible O-DU metrics notifier.
class flexible_o_du_metrics_notifier
{
public:
  virtual ~flexible_o_du_metrics_notifier() = default;

  /// Notifies a new flexible O-DU metric.
  virtual void on_new_metrics(const flexible_o_du_metrics& metric) = 0;
};

} // namespace srsran
