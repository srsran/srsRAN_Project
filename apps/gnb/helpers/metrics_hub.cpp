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
#include "srsran/support/math_utils.h"
#include <float.h>
#include <iomanip>
#include <sstream>

using namespace srsran;

metrics_hub::metrics_hub(task_executor* du_executor_)
{
  du_executor = du_executor_;
}
void metrics_hub::report_metrics(span<const scheduler_ue_metrics> ue_metrics)
{
  du_executor->execute([this, ue_metrics]() {
    for (auto& subscriber : subscribers) {
      subscriber->report_metrics(ue_metrics);
    }
  });
}
void metrics_hub::add_subscriber(scheduler_ue_metrics_notifier* subscriber)
{
  subscribers.push_back(subscriber);
}