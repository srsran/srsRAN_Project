/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <yaml-cpp/yaml.h>

namespace srsran {

struct dynamic_du_unit_config;

/// Fills the given node with the dynamic DU configuration values.
void fill_dynamic_du_unit_config_in_yaml_schema(YAML::Node& node, const dynamic_du_unit_config& config);

} // namespace srsran