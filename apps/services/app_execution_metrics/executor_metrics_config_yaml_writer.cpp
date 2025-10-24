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

#include "executor_metrics_config_yaml_writer.h"
#include "apps/helpers/metrics/metrics_config_yaml_writer.h"
#include "executor_metrics_config.h"

using namespace srsran;

static void fill_exec_metrics_section(YAML::Node node, const app_services::executor_metrics_config& config)
{
  auto layers_node               = node["layers"];
  layers_node["enable_executor"] = config.enable_executor_metrics;

  auto periodicity_node                       = node["periodicity"];
  periodicity_node["executors_report_period"] = config.report_period_ms.count();
}

void app_services::fill_app_exec_metrics_config_in_yaml_schema(YAML::Node&                                  node,
                                                               const app_services::executor_metrics_config& config)
{
  app_helpers::fill_metrics_appconfig_in_yaml_schema(node, config.common_metrics_cfg);
  fill_exec_metrics_section(node["metrics"], config);
}
