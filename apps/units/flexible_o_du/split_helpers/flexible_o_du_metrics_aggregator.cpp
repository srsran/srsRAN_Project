/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "flexible_o_du_metrics_aggregator.h"
#include "metrics/flexible_o_du_metrics.h"
#include "srsran/ru/ru_metrics_collector.h"

using namespace srsran;

namespace {

/// Fexible O-DU metrics notifier dummy implementation.
class flexible_o_du_metrics_notifier_dummy : public flexible_o_du_metrics_notifier
{
public:
  // See interface for documentation.
  void on_new_metrics(const flexible_o_du_metrics& metric) override {}
};

/// Radio Unit metrics collector dummy implementation.
class ru_metrics_collector_dummy : public ru_metrics_collector
{
public:
  // See interface for documentation.
  void collect_metrics(ru_metrics& metrics) override {}
};

} // namespace

/// Dummy notifier and collector.
static flexible_o_du_metrics_notifier_dummy dummy_notifier;
static ru_metrics_collector_dummy           dummy_collector;

flexible_o_du_metrics_aggregator::flexible_o_du_metrics_aggregator(flexible_o_du_metrics_notifier* notifier_) :
  notifier(notifier_ ? *notifier_ : dummy_notifier), ru_collector(&dummy_collector)
{
}

void flexible_o_du_metrics_aggregator::on_new_metrics(const srs_du::o_du_metrics& odu_metrics)
{
  flexible_o_du_metrics metrics;

  ru_collector->collect_metrics(metrics.ru);
  notifier.on_new_metrics(metrics);
}
