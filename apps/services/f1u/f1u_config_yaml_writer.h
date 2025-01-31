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

struct f1u_sockets_appconfig;

/// Fills the F1-U configuration in the given YAML node.
void fill_f1u_config_yaml_schema(YAML::Node& node, const f1u_sockets_appconfig& config);

} // namespace srsran
