/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_appconfig_yaml_writer.h"
#include "apps/services/f1u/f1u_config_yaml_writer.h"
#include "apps/services/logger/logger_appconfig_yaml_writer.h"
#include "du_appconfig.h"

using namespace srsran;

static void fill_du_appconfig_metrics_section(YAML::Node node, const srs_du::metrics_appconfig& config)
{
  node["addr"]                         = config.addr;
  node["port"]                         = config.port;
  node["resource_usage_report_period"] = config.rusage_report_period;
}

static void fill_du_appconfig_hal_section(YAML::Node node, const std::optional<hal_appconfig>& config)
{
  if (!config.has_value()) {
    return;
  }
  YAML::Node hal_node  = node["hal"];
  hal_node["eal_args"] = config.value().eal_args;
}

static void fill_du_appconfig_expert_execution_section(YAML::Node node, const expert_execution_appconfig& config)
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
    YAML::Node threads_node               = node["threads"];
    YAML::Node non_rt_node                = threads_node["non_rt"];
    non_rt_node["nof_non_rt_threads"]     = config.threads.non_rt_threads.nof_non_rt_threads;
    non_rt_node["non_rt_task_queue_size"] = config.threads.non_rt_threads.non_rt_task_queue_size;
  }
}

static void fill_du_appconfig_buffer_pool_section(YAML::Node node, const buffer_pool_appconfig& config)
{
  node["nof_segments"] = config.nof_segments;
  node["segment_size"] = config.segment_size;
}

static void fill_du_appconfig_remote_control_section(YAML::Node node, const remote_control_appconfig& config)
{
  node["enable"] = config.enabled;
  node["port"]   = config.port;
}

static void fill_du_appconfig_f1u_section(YAML::Node node, const srs_du::f1u_appconfig& config)
{
  node["queue_size"] = config.pdu_queue_size;
  fill_f1u_config_yaml_schema(node, config.f1u_sockets);
}

static void fill_du_appconfig_f1ap_section(YAML::Node node, const srs_du::f1ap_appconfig& config)
{
  node["cu_cp_addr"] = config.cu_cp_address;
  node["bind_addr"]  = config.bind_address;
}

void srsran::fill_du_appconfig_in_yaml_schema(YAML::Node& node, const du_appconfig& config)
{
  fill_logger_appconfig_in_yaml_schema(node, config.log_cfg);
  fill_du_appconfig_metrics_section(node["metrics"], config.metrics_cfg);
  fill_du_appconfig_hal_section(node, config.hal_config);
  fill_du_appconfig_expert_execution_section(node["expert_execution"], config.expert_execution_cfg);
  fill_du_appconfig_buffer_pool_section(node["buffer_pool"], config.buffer_pool_config);
  fill_du_appconfig_remote_control_section(node["remote_control"], config.remote_control_config);
  fill_du_appconfig_f1u_section(node["f1u"], config.f1u_cfg);
  fill_du_appconfig_f1ap_section(node["f1ap"], config.f1ap_cfg);
}
