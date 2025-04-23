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
