/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "flexible_o_du_metrics_builder.h"
#include "flexible_o_du_metrics_consumers.h"
#include "flexible_o_du_metrics_producer.h"

using namespace srsran;

flexible_o_du_metrics_notifier*
srsran::build_flexible_o_du_metrics_config(std::vector<app_services::metrics_config>& metrics,
                                           app_services::metrics_notifier&            notifier)
{
  flexible_o_du_metrics_notifier* out_value  = nullptr;
  auto&                           odu_metric = metrics.emplace_back();

  auto producer = std::make_unique<o_du_metrics_producer_impl>(notifier);
  out_value     = producer.get();
  odu_metric.producers.push_back(std::move(producer));
  odu_metric.consumers.push_back(std::make_unique<flexible_o_du_metrics_consumer_dummy>());
  odu_metric.metric_name = flexible_o_du_app_service_metrics_properties_impl().name();
  odu_metric.callback    = flexible_o_du_metrics_callback;

  return out_value;
}
