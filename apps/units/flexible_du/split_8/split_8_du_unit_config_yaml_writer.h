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

struct split_8_du_unit_config;

/// Fills the given node with the split 8 DU configuration values.
void fill_split_8_du_unit_config_in_yaml_schema(YAML::Node& node, const split_8_du_unit_config& config);

} // namespace srsran
