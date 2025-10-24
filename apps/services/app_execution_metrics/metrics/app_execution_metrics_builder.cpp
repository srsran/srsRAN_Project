/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "app_execution_metrics_builder.h"
#include "app_execution_metrics_consumer.h"
#include "app_execution_metrics_producer.h"
#include "apps/helpers/metrics/metrics_config.h"
#include "apps/helpers/metrics/metrics_helpers.h"

using namespace srsran;

executor_metrics_notifier& srsran::build_app_execution_metrics_config(app_services::metrics_config&   exec_metric_cfg,
                                                                      app_services::metrics_notifier& notifier,
                                                                      const app_helpers::metrics_config& metrics_cfg)
{
  auto                       producer  = std::make_unique<app_execution_metrics_producer_impl>(notifier);
  executor_metrics_notifier& out_value = *producer;

  exec_metric_cfg.callback    = producer->get_callback();
  exec_metric_cfg.metric_name = executor_metrics_properties_impl().name();
  exec_metric_cfg.producers.push_back(std::move(producer));

  if (metrics_cfg.enable_log_metrics) {
    exec_metric_cfg.consumers.push_back(
        std::make_unique<app_execution_metrics_consumer_log>(app_helpers::fetch_logger_metrics_log_channel()));
  }

  if (metrics_cfg.enable_json_metrics) {
    exec_metric_cfg.consumers.push_back(
        std::make_unique<app_execution_metrics_consumer_json>(app_helpers::fetch_json_metrics_log_channel()));
  }

  return out_value;
}
