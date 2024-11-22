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

struct du_low_unit_config;

/// Fills the given node with the DU low configuration values.
void fill_du_low_config_in_yaml_schema(YAML::Node& node, const du_low_unit_config& config);

} // namespace srsran
