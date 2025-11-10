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

#include "buffer_pool_config_yaml_writer.h"
#include "apps/helpers/metrics/metrics_config_yaml_writer.h"
#include "buffer_pool_appconfig.h"

using namespace srsran;
using namespace app_services;

static void fill_buffer_pool_section(YAML::Node node, const buffer_pool_appconfig& config)
{
  auto buffer_pool_node            = node["buffer_pool"];
  buffer_pool_node["nof_segments"] = config.nof_segments;
  buffer_pool_node["segment_size"] = config.segment_size;
}

void srsran::app_services::fill_buffer_pool_config_in_yaml_schema(YAML::Node& node, const buffer_pool_appconfig& config)
{
  app_helpers::fill_metrics_appconfig_in_yaml_schema(node, config.metrics_config.common_metrics_cfg);
  fill_buffer_pool_section(node, config);
}
