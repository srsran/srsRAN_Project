/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "app_resource_usage_config_yaml_writer.h"
#include "app_resource_usage_config.h"
#include "apps/helpers/metrics/metrics_config_yaml_writer.h"

using namespace srsran;

static void fill_metrics_section(YAML::Node node, const app_services::app_resource_usage_config& config)
{
  auto layers_node                = node["layers"];
  layers_node["enable_app_usage"] = config.enable_app_usage;
}

void app_services::fill_app_resource_usage_config_in_yaml_schema(YAML::Node&                      node,
                                                                 const app_resource_usage_config& config)
{
  app_helpers::fill_metrics_appconfig_in_yaml_schema(node, config.metrics_consumers_cfg);
  fill_metrics_section(node["metrics"], config);
}
