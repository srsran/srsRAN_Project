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

#include "apps/services/metrics/metrics_config.h"
#include "apps/services/metrics/metrics_notifier.h"
#include "apps/services/metrics/metrics_producer.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include "srsran/support/memory_pool/unbounded_object_pool.h"

namespace srsran {

/// Scheduler metrics producer.
class scheduler_metrics_producer_impl : public scheduler_metrics_notifier, public app_services::metrics_producer
{
  static constexpr size_t METRIC_POOL_INITIAL_CAPACITY = 3;

public:
  explicit scheduler_metrics_producer_impl(app_services::metrics_notifier& notifier_) :
    notifier(notifier_), metrics_pool(METRIC_POOL_INITIAL_CAPACITY)
  {
  }

  // See interface for documentation.
  void report_metrics(const scheduler_cell_metrics& report) override;

  /// Callback for the scheduler cell metrics.
  app_services::metrics_callback get_callback();

private:
  app_services::metrics_notifier& notifier;
  // We use the metrics pool to cache and avoid allocating memory for each report in a RT thread.
  unbounded_object_pool<scheduler_cell_metrics> metrics_pool;
};

} // namespace srsran
