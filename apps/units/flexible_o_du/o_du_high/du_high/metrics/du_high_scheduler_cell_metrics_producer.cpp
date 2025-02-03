/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_high_scheduler_cell_metrics_producer.h"
#include "du_high_scheduler_cell_metrics.h"
#include "srsran/support/executors/task_executor.h"

using namespace srsran;

void scheduler_metrics_producer_impl::report_metrics(const scheduler_cell_metrics& report)
{
  notifier.on_new_metric(scheduler_cell_metrics_impl(report));
}

app_services::metrics_callback scheduler_metrics_producer_impl::get_callback()
{
  return [&](const app_services::metrics_set&      report,
             span<app_services::metrics_consumer*> consumers,
             task_executor&                        executor,
             srslog::basic_logger&                 logger) {
    const auto& metric = static_cast<const scheduler_cell_metrics_impl&>(report);

    // Fetch a metrics report object from the pool.
    // Note: We are trying to reuse the pre-existing allocated memory in the cached_metrics object.
    auto cached_metrics = metrics_pool.get();
    *cached_metrics     = metric.get_metrics();
    if (!executor.defer([sched_metric = std::move(cached_metrics), consumers]() {
          for (auto& consumer : consumers) {
            consumer->handle_metric(scheduler_cell_metrics_impl(*sched_metric));
          }
        })) {
      logger.error("Failed to dispatch the metric '{}'", report.get_properties().name());
    }
  };
}
