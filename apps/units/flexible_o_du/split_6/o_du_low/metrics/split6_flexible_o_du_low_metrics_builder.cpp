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

#include "split6_flexible_o_du_low_metrics_builder.h"
#include "apps/helpers/metrics/metrics_config.h"
#include "apps/helpers/metrics/metrics_helpers.h"
#include "split6_flexible_o_du_low_app_service_metrics.h"
#include "split6_flexible_o_du_low_metrics_consumers.h"
#include "split6_flexible_o_du_low_metrics_producer.h"

using namespace srsran;

split6_flexible_o_du_low_metrics_notifier*
srsran::build_split6_flexible_o_du_low_metrics_config(std::vector<app_services::metrics_config>& metrics,
                                                      app_services::metrics_notifier&            notifier,
                                                      const app_helpers::metrics_config&         metrics_cfg,
                                                      const std::vector<pci_t>&                  pci_cell_map,
                                                      std::chrono::nanoseconds                   symbol_duration)
{
  split6_flexible_o_du_low_metrics_notifier* output = nullptr;
  if (!metrics_cfg.enabled()) {
    return output;
  }

  auto& odu_metric = metrics.emplace_back();
  auto  producer   = std::make_unique<split6_o_du_low_metrics_producer_impl>(notifier);
  output           = producer.get();
  odu_metric.producers.push_back(std::move(producer));
  odu_metric.metric_name = split6_flexible_o_du_low_app_service_metrics_properties_impl().name();
  odu_metric.callback    = split6_flexible_o_du_low_metrics_callback;

  if (metrics_cfg.enable_log_metrics) {
    odu_metric.consumers.push_back(std::make_unique<split6_flexible_o_du_low_metrics_consumer_log>(
        app_helpers::fetch_logger_metrics_log_channel(), pci_cell_map, metrics_cfg.enable_verbose, symbol_duration));
  }

  if (metrics_cfg.enable_json_metrics) {
    odu_metric.consumers.push_back(std::make_unique<split6_flexible_o_du_low_metrics_consumer_json>(
        app_helpers::fetch_json_metrics_log_channel(), pci_cell_map, symbol_duration));
  }

  return output;
}
