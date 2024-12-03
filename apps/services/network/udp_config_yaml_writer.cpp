/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "apps/services/network/udp_config_yaml_writer.h"
#include "apps/services/network/udp_cli11_schema.h"
#include <yaml-cpp/yaml.h>

namespace srsran {

/// Fills the UDP configuration in the given YAML node.
void fill_udp_config_in_yaml_schema(YAML::Node node, const udp_appconfig& config)
{
  node["max_rx_msgs"]    = config.rx_max_msgs;
  node["pool_threshold"] = config.pool_threshold;
}

} // namespace srsran
