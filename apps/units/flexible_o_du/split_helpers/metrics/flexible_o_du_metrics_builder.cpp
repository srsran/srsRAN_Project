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
#include "apps/helpers/metrics/metrics_config.h"
#include "apps/helpers/metrics/metrics_helpers.h"
#include "apps/units/flexible_o_du/flexible_o_du_commands.h"
#include "flexible_o_du_metrics_consumers.h"
#include "flexible_o_du_metrics_producer.h"

using namespace srsran;

flexible_o_du_metrics_notifier*
srsran::build_flexible_o_du_metrics_config(std::vector<app_services::metrics_config>&                   metrics,
                                           std::vector<std::unique_ptr<app_services::cmdline_command>>& unit_commands,
                                           app_services::metrics_notifier&                              notifier,
                                           const app_helpers::metrics_config&                           metrics_cfg,
                                           std::vector<pci_t>                                           pci_cell_map)
{
  flexible_o_du_metrics_notifier* out_value = nullptr;

  auto& odu_metric = metrics.emplace_back();
  auto  producer   = std::make_unique<o_du_metrics_producer_impl>(notifier);
  out_value        = producer.get();
  odu_metric.producers.push_back(std::move(producer));
  odu_metric.metric_name = flexible_o_du_app_service_metrics_properties_impl().name();
  odu_metric.callback    = flexible_o_du_metrics_callback;

  // Create STDOUT consumer.
  auto metrics_stdout = std::make_unique<ru_metrics_consumer_stdout>(pci_cell_map);
  unit_commands.push_back(std::make_unique<toggle_stdout_ru_metrics_app_command>(*metrics_stdout));
  odu_metric.consumers.push_back(std::move(metrics_stdout));

  if (metrics_cfg.enable_log_metrics) {
    odu_metric.consumers.push_back(std::make_unique<flexible_o_du_metrics_consumer_log>(
        app_helpers::fetch_logger_metrics_log_channel(), pci_cell_map));
  }

  if (metrics_cfg.json_config.enable_json_metrics) {
    odu_metric.consumers.push_back(std::make_unique<flexible_o_du_metrics_consumer_json>(
        app_helpers::fetch_json_metrics_log_channel(), pci_cell_map));
  }

  return out_value;
}
