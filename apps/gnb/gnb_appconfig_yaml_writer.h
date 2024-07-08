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

struct gnb_appconfig;

/// Fills the given node with the gNB application configuration values.
void fill_gnb_appconfig_in_yaml_schema(YAML::Node& node, const gnb_appconfig& config);

} // namespace srsran