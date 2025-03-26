/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <yaml-cpp/yaml.h>

namespace srsran {
namespace app_services {

struct app_resource_usage_config;

/// Fills the given node with the application resource usage configuration values.
void fill_app_resource_usage_config_in_yaml_schema(YAML::Node& node, const app_resource_usage_config& config);

} // namespace app_services
} // namespace srsran
