/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "metrics_hub.h"

using namespace srsran;

metrics_hub::metrics_hub(task_executor& du_executor_) : du_executor(du_executor_) {}
void metrics_hub::report_metrics(span<const scheduler_ue_metrics> ue_metrics)
{
  std::vector<scheduler_ue_metrics> ue_metrics_copy(ue_metrics.begin(), ue_metrics.end());
  du_executor.execute([this, ue_metrics_copy]() {
    for (auto& subscriber : subscribers) {
      span<const scheduler_ue_metrics> ue_metrics_span(ue_metrics_copy);
      subscriber->report_metrics(ue_metrics_span);
    }
  });
}
void metrics_hub::add_subscriber(scheduler_ue_metrics_notifier& subscriber)
{
  subscribers.push_back(&subscriber);
}