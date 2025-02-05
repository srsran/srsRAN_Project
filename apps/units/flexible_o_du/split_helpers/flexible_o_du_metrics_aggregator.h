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

#include "metrics/flexible_o_du_metrics_notifier.h"
#include "srsran/du/o_du_metrics_notifier.h"

namespace srsran {

class ru_metrics_collector;

/// Flexible O-DU metrics aggregator. It aggregates O-DU metrics and RU metrics.
class flexible_o_du_metrics_aggregator : public srs_du::o_du_metrics_notifier
{
  flexible_o_du_metrics_notifier& notifier;
  ru_metrics_collector*           ru_collector = nullptr;

public:
  explicit flexible_o_du_metrics_aggregator(flexible_o_du_metrics_notifier* notifier_);

  // See interface for documentation.
  void on_new_metrics(const srs_du::o_du_metrics& odu_metrics) override;

  /// Sets the RU metrics collector to the given one.
  void set_ru_metrics_collector(ru_metrics_collector& collector) { ru_collector = &collector; }
};

} // namespace srsran
