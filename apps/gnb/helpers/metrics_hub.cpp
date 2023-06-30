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