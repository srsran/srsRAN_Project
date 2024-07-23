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

#include "ru_ofh_config_yaml_writer.h"
#include "ru_ofh_config.h"

using namespace srsran;

static void fill_ru_ofh_log_section(YAML::Node node, const ru_ofh_unit_logger_config& config)
{
  node["ofh_level"] = srslog::basic_level_to_string(config.ofh_level);
}

static void fill_ru_ofh_expert_execution_section(YAML::Node node, const ru_ofh_unit_expert_execution_config& config)
{
  {
    YAML::Node affinities_node       = node["affinities"];
    affinities_node["ru_timing_cpu"] = fmt::format("{:,}", span<const size_t>(config.ru_timing_cpu.get_cpu_ids()));
  }

  {
    YAML::Node threads_node               = node["threads"];
    YAML::Node ofh_node                   = threads_node["ofh"];
    ofh_node["enable_dl_parallelization"] = config.threads.is_downlink_parallelized;
  }

  auto cell_affinities_node = node["cell_affinities"];
  while (config.cell_affinities.size() > cell_affinities_node.size()) {
    cell_affinities_node.push_back(YAML::Node());
  }

  unsigned index = 0;
  for (auto cell : cell_affinities_node) {
    const auto& expert = config.cell_affinities[index];

    if (expert.ru_cpu_cfg.mask.any()) {
      cell["ru_cpus"] = fmt::format("{:,}", span<const size_t>(expert.ru_cpu_cfg.mask.get_cpu_ids()));
    }
    cell["ru_pinning"] = to_string(expert.ru_cpu_cfg.pinning_policy);

    ++index;
  }
}

static void fill_ru_ofh_hal_section(YAML::Node node, const std::optional<ru_ofh_unit_hal_config>& config)
{
  if (!config.has_value()) {
    return;
  }
  YAML::Node hal_node  = node["hal"];
  hal_node["eal_args"] = config.value().eal_args;
}

static YAML::Node build_ru_ofh_cell_section(const ru_ofh_unit_cell_config& config)
{
  YAML::Node node;

  if (config.cell.ru_operating_bw.has_value()) {
    node["ru_bandwidth_MHz"] = bs_channel_bandwidth_to_MHz(config.cell.ru_operating_bw.value());
  }
  node["t1a_max_cp_dl"]              = config.cell.T1a_max_cp_dl.count();
  node["t1a_min_cp_dl"]              = config.cell.T1a_min_cp_dl.count();
  node["t1a_max_cp_ul"]              = config.cell.T1a_max_cp_ul.count();
  node["t1a_min_cp_ul"]              = config.cell.T1a_min_cp_ul.count();
  node["t1a_max_up"]                 = config.cell.T1a_max_up.count();
  node["t1a_min_up"]                 = config.cell.T1a_min_up.count();
  node["ta4_max"]                    = config.cell.Ta4_max.count();
  node["ta4_min"]                    = config.cell.Ta4_min.count();
  node["is_prach_cp_enabled"]        = config.cell.is_prach_control_plane_enabled;
  node["is_dl_broadcast_enabled"]    = config.cell.is_downlink_broadcast_enabled;
  node["ignore_ecpri_seq_id"]        = config.cell.ignore_ecpri_seq_id_field;
  node["ignore_ecpri_payload_size"]  = config.cell.ignore_ecpri_payload_size_field;
  node["warn_unreceived_ru_frames"]  = config.cell.warn_unreceived_ru_frames;
  node["compr_method_ul"]            = config.cell.compression_method_ul;
  node["compr_bitwidth_ul"]          = config.cell.compression_bitwidth_ul;
  node["compr_method_dl"]            = config.cell.compression_method_dl;
  node["compr_bitwidth_dl"]          = config.cell.compression_bitwidth_dl;
  node["compr_method_prach"]         = config.cell.compression_method_prach;
  node["compr_bitwidth_prach"]       = config.cell.compression_bitwidth_prach;
  node["enable_ul_static_compr_hdr"] = config.cell.is_uplink_static_comp_hdr_enabled;
  node["enable_dl_static_compr_hdr"] = config.cell.is_downlink_static_comp_hdr_enabled;
  node["iq_scaling"]                 = config.cell.iq_scaling;
  node["network_interface"]          = config.network_interface;
  node["enable_promiscuous"]         = config.enable_promiscuous_mode;
  node["mtu"]                        = config.mtu_size.value();
  node["ru_mac_addr"]                = config.ru_mac_address;
  node["du_mac_addr"]                = config.du_mac_address;
  node["vlan_tag_cp"]                = config.vlan_tag_cp;
  node["vlan_tag_up"]                = config.vlan_tag_up;
  for (auto id : config.ru_prach_port_id) {
    node["prach_port_id"] = id;
  }
  node["prach_port_id"].SetStyle(YAML::EmitterStyle::Flow);
  for (auto id : config.ru_dl_port_id) {
    node["dl_port_id"] = id;
  }
  node["dl_port_id"].SetStyle(YAML::EmitterStyle::Flow);
  for (auto id : config.ru_ul_port_id) {
    node["ul_port_id"] = id;
  }
  node["ul_port_id"].SetStyle(YAML::EmitterStyle::Flow);

  return node;
}

static void fill_ru_ofh_section(YAML::Node node, const ru_ofh_unit_config& config)
{
  node["gps_alpha"] = config.gps_Alpha;
  node["gps_beta"]  = config.gps_Beta;

  for (const auto& cell : config.cells) {
    node["cells"].push_back(build_ru_ofh_cell_section(cell));
  }
}

void srsran::fill_ru_ofh_config_in_yaml_schema(YAML::Node& node, const ru_ofh_unit_config& config)
{
  fill_ru_ofh_log_section(node["log"], config.loggers);
  fill_ru_ofh_expert_execution_section(node["expert_execution"], config.expert_execution_cfg);
  fill_ru_ofh_section(node["ru_ofh"], config);
  fill_ru_ofh_hal_section(node, config.hal_config);
}
