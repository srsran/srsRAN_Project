/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "cu_appconfig_yaml_writer.h"
#include "apps/services/logger/logger_appconfig_yaml_writer.h"
#include "cu_appconfig.h"

using namespace srsran;

static void fill_cu_appconfig_buffer_pool_section(YAML::Node node, const buffer_pool_appconfig& config)
{
  node["nof_segments"] = config.nof_segments;
  node["segment_size"] = config.segment_size;
}

static void fill_cu_appconfig_f1ap_section(YAML::Node node, const srs_cu::cu_f1ap_appconfig& config)
{
  YAML::Node cu_cp_node     = node["cu_cp"];
  YAML::Node f1ap_node      = cu_cp_node["f1ap"];
  f1ap_node["bind_address"] = config.bind_addr;
}

static void fill_cu_appconfig_nru_section(YAML::Node node, const srs_cu::cu_nru_appconfig& config)
{
  YAML::Node cu_up_node       = node["cu_up"];
  YAML::Node nru_node         = cu_up_node["nru"];
  nru_node["udp_max_rx_msgs"] = config.udp_rx_max_msgs;
  nru_node["bind_addr"]       = config.bind_addr;
}

void srsran::fill_cu_appconfig_in_yaml_schema(YAML::Node& node, const cu_appconfig& config)
{
  fill_logger_appconfig_in_yaml_schema(node, config.log_cfg);
  fill_cu_appconfig_buffer_pool_section(node["buffer_pool"], config.buffer_pool_config);
  fill_cu_appconfig_f1ap_section(node, config.f1ap_cfg);
  fill_cu_appconfig_nru_section(node, config.nru_cfg);
}
