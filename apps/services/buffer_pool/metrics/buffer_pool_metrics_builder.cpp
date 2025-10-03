/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "apps/services/buffer_pool/metrics/buffer_pool_metrics_builder.h"
#include "apps/helpers/metrics/metrics_config.h"
#include "apps/helpers/metrics/metrics_helpers.h"
#include "apps/services/buffer_pool/metrics/buffer_pool_metrics_consumer.h"
#include "apps/services/buffer_pool/metrics/buffer_pool_metrics_producer.h"

using namespace srsran;
using namespace app_services;

metrics_config app_services::build_buffer_pool_metrics_config(metrics_notifier&                  notifier,
                                                              const app_helpers::metrics_config& metrics_cfg)
{
  metrics_config config;

  config.metric_name = buffer_pool_metrics_properties_impl().name();
  config.callback    = buffer_pool_metrics_callback;
  config.producers.emplace_back(std::make_unique<buffer_pool_metrics_producer_impl>(notifier));

  if (metrics_cfg.enable_log_metrics) {
    config.consumers.push_back(
        std::make_unique<buffer_pool_metrics_consumer_log>(app_helpers::fetch_logger_metrics_log_channel()));
  }

  if (metrics_cfg.enable_json_metrics) {
    config.consumers.push_back(
        std::make_unique<buffer_pool_metrics_consumer_json>(app_helpers::fetch_json_metrics_log_channel()));
  }

  return config;
}
