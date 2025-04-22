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

#include "srsran/du/du_high/du_metrics_notifier.h"
#include "srsran/du/du_high/o_du_high_metrics_notifier.h"

namespace srsran {
namespace srs_du {

/// Proxy class that listen to scheduler metrics and forwards them to the configured scheduler metrics notifier and O-DU
/// high metrics notifier.
class o_du_high_metrics_notifier_proxy : public du_metrics_notifier
{
  du_metrics_notifier*        du_notifier     = nullptr;
  o_du_high_metrics_notifier* odu_hi_notifier = nullptr;

public:
  explicit o_du_high_metrics_notifier_proxy(du_metrics_notifier* notifier_);

  // See interface for documentation.
  void on_new_metric_report(const du_metrics_report& report) override;

  /// Sets the O-DU high metrics notifier to the given one.
  void set_o_du_high_metrics_notifier(o_du_high_metrics_notifier& notifier);
};

} // namespace srs_du
} // namespace srsran
