/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#pragma once

#include "srsran/adt/span.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

class metrics_hub : public scheduler_ue_metrics_notifier
{
public:
  metrics_hub(task_executor& du_executor_);
  void report_metrics(span<const scheduler_ue_metrics> ue_metrics) override;
  void add_subscriber(scheduler_ue_metrics_notifier& subscriber);

private:
  std::vector<scheduler_ue_metrics_notifier*> subscribers;
  task_executor&                              du_executor;
};

} // namespace srsran
