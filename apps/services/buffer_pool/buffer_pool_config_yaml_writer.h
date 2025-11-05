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

struct buffer_pool_appconfig;

/// Fills the given node with the buffer pool configuration values.
void fill_buffer_pool_config_in_yaml_schema(YAML::Node& node, const buffer_pool_appconfig& config);

} // namespace app_services
} // namespace srsran
