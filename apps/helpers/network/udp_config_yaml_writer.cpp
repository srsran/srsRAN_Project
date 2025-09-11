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

#include "udp_config_yaml_writer.h"
#include "udp_appconfig.h"

using namespace srsran;

/// Fills the UDP configuration in the given YAML node.
void srsran::fill_udp_config_in_yaml_schema(YAML::Node node, const udp_appconfig& config)
{
  node["max_rx_msgs"]    = config.rx_max_msgs;
  node["pool_threshold"] = config.pool_threshold;
}
