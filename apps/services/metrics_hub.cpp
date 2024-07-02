/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "srsran/srslog/srslog.h"

using namespace srsran;

metrics_hub::metrics_hub(task_executor& executor_) :
  logger(srslog::fetch_basic_logger("METRICS HUB")), executor(executor_)
{
}

void metrics_hub::add_source(std::unique_ptr<metrics_hub_source> source)
{
  source->set_task_executor(&executor);
  sources.push_back(std::move(source));
}

scheduler_ue_metrics_source* metrics_hub::get_scheduler_ue_metrics_source(std::string source_name_)
{
  for (auto& source : sources) {
    if (source->source_name == source_name_) {
      return dynamic_cast<scheduler_ue_metrics_source*>(source.get());
    }
  }
  return nullptr;
}

rlc_metrics_source* metrics_hub::get_rlc_metrics_source(std::string source_name_)
{
  for (auto& source : sources) {
    if (source->source_name == source_name_) {
      return dynamic_cast<rlc_metrics_source*>(source.get());
    }
  }
  return nullptr;
}

scheduler_ue_metrics_source::scheduler_ue_metrics_source(std::string source_name_) :
  metrics_hub_source(source_name_),
  logger(srslog::fetch_basic_logger("ALL")),
  metrics_pool(metric_pool_initial_capacity)
{
}

void scheduler_ue_metrics_source::report_metrics(const scheduler_cell_metrics& metrics)
{
  srsran_assert(executor != nullptr, "Task executor must not be nullptr");

  // Fetch a metrics report object from the pool.
  // Note: We are trying to reuse the pre-existing allocated memory in the cached_metrics object.
  auto cached_metrics = metrics_pool.get();
  *cached_metrics     = metrics;

  if (not executor->execute([this, cached_metrics = std::move(cached_metrics)]() {
        for (auto& subscriber : subscribers) {
          subscriber->report_metrics(*cached_metrics);
        }
      })) {
    logger.warning("Failed to dispatch scheduler UE metrics");
  }
}

void scheduler_ue_metrics_source::add_subscriber(scheduler_metrics_notifier& subscriber)
{
  subscribers.push_back(&subscriber);
}

void rlc_metrics_source::report_metrics(const rlc_metrics& metrics)
{
  srsran_assert(executor != nullptr, "Task executor must not be nullptr");
  if (not executor->execute([this, metrics]() {
        for (auto& subscriber : subscribers) {
          subscriber->report_metrics(metrics);
        }
      })) {
    srslog::fetch_basic_logger("ALL").warning("Failed to dispatch RLC metrics");
  }
}

void rlc_metrics_source::add_subscriber(rlc_metrics_notifier& subscriber)
{
  subscribers.push_back(&subscriber);
}
