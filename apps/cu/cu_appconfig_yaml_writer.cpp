/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
