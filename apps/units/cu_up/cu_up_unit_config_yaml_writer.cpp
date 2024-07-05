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

#include "cu_up_unit_config_yaml_writer.h"
#include "cu_up_unit_config.h"
#include "srsran/adt/span.h"

using namespace srsran;

static void fill_cu_up_upf_section(YAML::Node node, const cu_up_unit_upf_config& config)
{
  node["bind_addr"]         = config.bind_addr;
  node["n3_bind_addr"]      = config.n3_bind_addr;
  node["n3_bind_interface"] = config.n3_bind_interface;
  node["n3_ext_addr"]       = config.n3_ext_addr;
  node["udp_max_rx_msgs"]   = config.udp_rx_max_msgs;
  node["no_core"]           = config.no_core;
}

static void fill_cu_up_metrics_section(YAML::Node node, const cu_up_unit_metrics_config& config)
{
  node["cu_up_statistics_report_period"] = config.cu_up_statistics_report_period;
  node["pdcp_report_period"]             = config.pdcp.report_period;
}

static void fill_cu_up_pcap_section(YAML::Node node, const cu_up_unit_pcap_config& config)
{
  node["n3_filename"]   = config.n3.filename;
  node["n3_enable"]     = config.n3.enabled;
  node["f1u_filename"]  = config.f1u.filename;
  node["f1u_enable"]    = config.f1u.enabled;
  node["e1ap_filename"] = config.e1ap.filename;
  node["e1ap_enable"]   = config.e1ap.enabled;
}

static void fill_cu_up_log_section(YAML::Node node, const cu_up_unit_logger_config& config)
{
  node["pdcp_level"]   = srslog::basic_level_to_string(config.pdcp_level);
  node["sdap_level"]   = srslog::basic_level_to_string(config.sdap_level);
  node["gtpu_level"]   = srslog::basic_level_to_string(config.gtpu_level);
  node["f1u_level"]    = srslog::basic_level_to_string(config.f1u_level);
  node["cu_level"]     = srslog::basic_level_to_string(config.cu_level);
  node["hex_max_size"] = config.hex_max_size;
}

static YAML::Node build_cu_up_section(const cu_up_unit_config& config)
{
  YAML::Node node;

  node["gtpu_queue_size"] = config.gtpu_queue_size;
  node["warn_on_drop"]    = config.warn_on_drop;

  return node;
}

static void fill_um_bidir_section(YAML::Node node, const cu_up_unit_qos_config& config)
{
  YAML::Node node_tx    = node["tx"];
  node_tx["queue-size"] = config.rlc_sdu_queue;
}

static void fill_am_section(YAML::Node node, const cu_up_unit_qos_config& config)
{
  YAML::Node node_tx    = node["tx"];
  node_tx["queue-size"] = config.rlc_sdu_queue;
}

static void fill_cu_up_rlc_qos_section(YAML::Node node, const cu_up_unit_qos_config& config)
{
  node["mode"] = config.mode;

  if (config.mode == "um_bidir") {
    fill_um_bidir_section(node["um_bidir"], config);
  } else if (config.mode == "am") {
    fill_am_section(node["am"], config);
  }
}

static void fill_cu_up_f1_qos_section(YAML::Node node, const cu_cp_unit_f1u_config& config)
{
  node["backoff_timer"] = config.t_notify;
}

static void fill_cu_up_qos_entry(YAML::Node node, const cu_up_unit_qos_config& config)
{
  node["five_qi"] = five_qi_to_uint(config.five_qi);
  fill_cu_up_rlc_qos_section(node["rlc"], config);
  fill_cu_up_f1_qos_section(node["f1u_cu_up"], config.f1u_cu_up);
}

static YAML::Node get_last_entry(YAML::Node node)
{
  auto it = node.begin();
  for (unsigned i = 1; i != node.size(); ++i) {
    ++it;
  }
  return *it;
}

static void fill_cu_up_qos_section(YAML::Node node, span<const cu_up_unit_qos_config> qos_cfg)
{
  auto qos_node = node["qos"];
  for (const auto& qos : qos_cfg) {
    auto node_entry = std::find_if(qos_node.begin(), qos_node.end(), [five = qos.five_qi](const YAML::Node& tmp) {
      return static_cast<uint16_t>(five) == tmp["five_qi"].as<uint16_t>();
    });
    if (node_entry != qos_node.end()) {
      YAML::Node node_five = *node_entry;
      fill_cu_up_qos_entry(node_five, qos);
    } else {
      qos_node.push_back(YAML::Node());
      fill_cu_up_qos_entry(get_last_entry(qos_node), qos);
    }
  }
}

void srsran::fill_cu_up_config_in_yaml_schema(YAML::Node& node, const cu_up_unit_config& config)
{
  node["cu_up"] = build_cu_up_section(config);
  fill_cu_up_log_section(node["log"], config.loggers);
  fill_cu_up_pcap_section(node["pcap"], config.pcap_cfg);
  fill_cu_up_metrics_section(node["metrics"], config.metrics);
  fill_cu_up_upf_section(node["amf"], config.upf_cfg);

  fill_cu_up_qos_section(node, config.qos_cfg);
}
