/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

void scheduler_ue_metrics_source::report_metrics(span<const scheduler_ue_metrics> ue_metrics)
{
  srsran_assert(executor != nullptr, "Task executor must not be nullptr");
  std::vector<scheduler_ue_metrics> ue_metrics_copy(ue_metrics.begin(), ue_metrics.end());
  if (not executor->execute([this, ue_metrics_copy]() {
        for (auto& subscriber : subscribers) {
          span<const scheduler_ue_metrics> ue_metrics_span(ue_metrics_copy);
          subscriber->report_metrics(ue_metrics_span);
        }
      })) {
    srslog::fetch_basic_logger("ALL").warning("Failed to dispatch scheduler UE metrics");
  }
}

void scheduler_ue_metrics_source::add_subscriber(scheduler_ue_metrics_notifier& subscriber)
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
