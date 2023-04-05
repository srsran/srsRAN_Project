/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/span.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

class metrics_hub : public scheduler_ue_metrics_notifier
{
public:
  metrics_hub(task_executor* du_executor_);
  void report_metrics(span<const scheduler_ue_metrics> ue_metrics) override;
  void add_subscriber(scheduler_ue_metrics_notifier* subscriber);

private:
  std::vector<scheduler_ue_metrics_notifier*> subscribers;
  task_executor*                              du_executor;
};

} // namespace srsran
