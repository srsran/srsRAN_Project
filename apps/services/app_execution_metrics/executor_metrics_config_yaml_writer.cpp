/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
