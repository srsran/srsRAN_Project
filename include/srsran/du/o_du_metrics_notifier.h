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

struct o_du_metrics;

/// O-RAN DU metrics notifier.
class o_du_metrics_notifier
{
public:
  virtual ~o_du_metrics_notifier() = default;

  /// Notifies new O-DU metrics.
  virtual void on_new_metrics(const o_du_metrics& metrics) = 0;
};

} // namespace srs_du
} // namespace srsran
