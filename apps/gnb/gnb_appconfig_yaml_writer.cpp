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

#include "gnb_appconfig_yaml_writer.h"
#include "apps/services/logger/logger_appconfig_yaml_writer.h"
#include "gnb_appconfig.h"

using namespace srsran;

static void fill_gnb_appconfig_metrics_section(YAML::Node node, const metrics_appconfig& config)
{
  node["pdcp_report_period"]       = config.pdcp.report_period;
  node["enable_json_metrics"]      = config.enable_json_metrics;
  node["addr"]                     = config.addr;
  node["port"]                     = config.port;
  node["autostart_stdout_metrics"] = config.autostart_stdout_metrics;
  node["stdout_metrics_period"]    = config.stdout_metrics_period;
}

static void fill_gnb_appconfig_e2_section(YAML::Node node, const e2_appconfig& config)
{
  node["enable_du_e2"]           = config.enable_du_e2;
  node["addr"]                   = config.ip_addr;
  node["port"]                   = config.port;
  node["bind_addr"]              = config.bind_addr;
  node["sctp_rto_initial"]       = config.sctp_rto_initial;
  node["sctp_rto_min"]           = config.sctp_rto_min;
  node["sctp_rto_max"]           = config.sctp_rto_max;
  node["sctp_init_max_attempts"] = config.sctp_init_max_attempts;
  node["sctp_max_init_timeo"]    = config.sctp_max_init_timeo;
  node["e2sm_kpm_enabled"]       = config.e2sm_kpm_enabled;
  node["e2sm_rc_enabled"]        = config.e2sm_rc_enabled;
}

static void fill_gnb_appconfig_hal_section(YAML::Node node, const std::optional<hal_appconfig>& config)
{
  if (!config.has_value()) {
    return;
  }
  YAML::Node hal_node  = node["hal"];
  hal_node["eal_args"] = config.value().eal_args;
}

static void fill_gnb_appconfig_expert_execution_section(YAML::Node node, const expert_execution_appconfig& config)
{
  {
    YAML::Node affinities_node = node["affinities"];

    if (config.affinities.isolated_cpus.has_value()) {
      affinities_node["isolated_cpus"] =
          fmt::format("{:,}", span<const size_t>(config.affinities.isolated_cpus.value().get_cpu_ids()));
    }

    if (config.affinities.low_priority_cpu_cfg.mask.any()) {
      affinities_node["low_priority_cpus"] =
          fmt::format("{:,}", span<const size_t>(config.affinities.low_priority_cpu_cfg.mask.get_cpu_ids()));
    }
    affinities_node["low_priority_pinning"] = to_string(config.affinities.low_priority_cpu_cfg.pinning_policy);
  }

  {
    YAML::Node threads_node           = node["threads"];
    YAML::Node non_rt_node            = threads_node["non_rt"];
    non_rt_node["nof_non_rt_threads"] = config.threads.non_rt_threads.nof_non_rt_threads;
  }
}

static void fill_gnb_appconfig_buffer_pool_section(YAML::Node node, const buffer_pool_appconfig& config)
{
  node["nof_segments"] = config.nof_segments;
  node["segment_size"] = config.segment_size;
}

void srsran::fill_gnb_appconfig_in_yaml_schema(YAML::Node& node, const gnb_appconfig& config)
{
  node["gnb_id"]            = config.gnb_id.id;
  node["gnb_id_bit_length"] = static_cast<unsigned>(config.gnb_id.bit_length);
  node["ran_node_name"]     = config.ran_node_name;

  fill_logger_appconfig_in_yaml_schema(node, config.log_cfg);
  fill_gnb_appconfig_metrics_section(node["metrics"], config.metrics_cfg);
  fill_gnb_appconfig_e2_section(node["e2"], config.e2_cfg);
  fill_gnb_appconfig_hal_section(node, config.hal_config);
  fill_gnb_appconfig_expert_execution_section(node["expert_execution"], config.expert_execution_cfg);
  fill_gnb_appconfig_buffer_pool_section(node["buffer_pool"], config.buffer_pool_config);
}
