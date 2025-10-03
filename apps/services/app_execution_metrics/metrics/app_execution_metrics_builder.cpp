/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "app_execution_metrics_builder.h"
#include "app_execution_metrics_consumer.h"
#include "app_execution_metrics_producer.h"
#include "apps/helpers/metrics/metrics_config.h"
#include "apps/helpers/metrics/metrics_helpers.h"

using namespace srsran;

executor_metrics_notifier*
srsran::build_app_execution_metrics_config(std::vector<app_services::metrics_config>& metrics,
                                           app_services::metrics_notifier&            notifier,
                                           const app_helpers::metrics_config&         metrics_cfg)
{
  executor_metrics_notifier* out_value   = nullptr;
  auto&                      exec_metric = metrics.emplace_back();

  auto producer = std::make_unique<app_execution_metrics_producer_impl>(notifier);
  out_value     = producer.get();

  exec_metric.callback    = producer->get_callback();
  exec_metric.metric_name = executor_metrics_properties_impl().name();
  exec_metric.producers.push_back(std::move(producer));

  if (metrics_cfg.enable_log_metrics) {
    exec_metric.consumers.push_back(
        std::make_unique<app_execution_metrics_consumer_log>(app_helpers::fetch_logger_metrics_log_channel()));
  }

  if (metrics_cfg.enable_json_metrics) {
    exec_metric.consumers.push_back(
        std::make_unique<app_execution_metrics_consumer_json>(app_helpers::fetch_json_metrics_log_channel()));
  }

  return out_value;
}
