/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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

  // Caution, the copy may become expensive when the DU metrics grow.
  metrics.du = odu_metrics;
  ru_collector->collect_metrics(metrics.ru);
  notifier.on_new_metrics(metrics);
}
