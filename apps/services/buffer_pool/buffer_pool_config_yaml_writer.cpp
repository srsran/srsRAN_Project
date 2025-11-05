/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
