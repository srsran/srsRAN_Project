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

#include "du_appconfig_cli11_schema.h"
#include "apps/services/buffer_pool/buffer_pool_appconfig_cli11_schema.h"
#include "apps/services/logger/logger_appconfig_cli11_schema.h"
#include "du_appconfig.h"
#include "srsran/adt/interval.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;

template <typename Integer>
static expected<Integer, std::string> parse_int(const std::string& value)
{
  try {
    return std::stoi(value);
  } catch (const std::invalid_argument& e) {
    return make_unexpected(e.what());
  } catch (const std::out_of_range& e) {
    return make_unexpected(e.what());
  }
}

static void configure_cli11_metrics_args(CLI::App& app, srs_du::metrics_appconfig& metrics_params)
{
  add_option(app, "--enable_json_metrics", metrics_params.enable_json_metrics, "Enable JSON metrics reporting")
      ->always_capture_default();

  app.add_option("--addr", metrics_params.addr, "Metrics address.")->capture_default_str()->check(CLI::ValidIPV4);
  app.add_option("--port", metrics_params.port, "Metrics UDP port.")
      ->capture_default_str()
      ->check(CLI::Range(0, 65535));

  app.add_option(
         "--autostart_stdout_metrics", metrics_params.autostart_stdout_metrics, "Autostart stdout metrics reporting")
      ->capture_default_str();

  add_option(app,
             "--stdout_metrics_period",
             metrics_params.stdout_metrics_period,
             "DU statistics report period in milliseconds. This metrics sets the console output period.")
      ->capture_default_str();
}

static void configure_cli11_e2_args(CLI::App& app, e2_appconfig& e2_params)
{
  add_option(app, "--enable_du_e2", e2_params.enable_du_e2, "Enable DU E2 agent")->capture_default_str();
  add_option(app, "--addr", e2_params.ip_addr, "RIC IP address")->capture_default_str();
  add_option(app, "--port", e2_params.port, "RIC port")->capture_default_str()->check(CLI::Range(20000, 40000));
  add_option(app, "--bind_addr", e2_params.bind_addr, "Local IP address to bind for RIC connection")
      ->capture_default_str()
      ->check(CLI::ValidIPV4);
  add_option(app, "--sctp_rto_initial", e2_params.sctp_rto_initial, "SCTP initial RTO value")->capture_default_str();
  add_option(app, "--sctp_rto_min", e2_params.sctp_rto_min, "SCTP RTO min")->capture_default_str();
  add_option(app, "--sctp_rto_max", e2_params.sctp_rto_max, "SCTP RTO max")->capture_default_str();
  add_option(app, "--sctp_init_max_attempts", e2_params.sctp_init_max_attempts, "SCTP init max attempts")
      ->capture_default_str();
  add_option(app, "--sctp_max_init_timeo", e2_params.sctp_max_init_timeo, "SCTP max init timeout")
      ->capture_default_str();
  add_option(app, "--e2sm_kpm_enabled", e2_params.e2sm_kpm_enabled, "Enable KPM service module")->capture_default_str();
  add_option(app, "--e2sm_rc_enabled", e2_params.e2sm_rc_enabled, "Enable RC service module")->capture_default_str();
}

static error_type<std::string> is_valid_cpu_index(unsigned cpu_idx)
{
  std::string error_message = fmt::format("Invalid CPU core selected '{}'. Valid CPU ids: {}",
                                          cpu_idx,
                                          os_sched_affinity_bitmask::available_cpus().get_cpu_ids());

  os_sched_affinity_bitmask one_cpu_mask;
  if (cpu_idx >= one_cpu_mask.size()) {
    return make_unexpected(error_message);
  }
  one_cpu_mask.set(cpu_idx);
  if (not one_cpu_mask.subtract(os_sched_affinity_bitmask::available_cpus()).empty()) {
    return make_unexpected(error_message);
  }
  return default_success_t();
}

static expected<unsigned, std::string> parse_one_cpu(const std::string& value)
{
  expected<int, std::string> result = parse_int<int>(value);

  if (not result.has_value()) {
    return make_unexpected(fmt::format("Could not parse '{}' string as a CPU index", value));
  }

  error_type<std::string> validation_result = is_valid_cpu_index(result.value());
  if (not validation_result.has_value()) {
    return make_unexpected(validation_result.error());
  }

  return result.value();
}

static expected<interval<unsigned, true>, std::string> parse_cpu_range(const std::string& value)
{
  std::vector<unsigned> range;
  std::stringstream     ss(value);
  while (ss.good()) {
    std::string str;
    getline(ss, str, '-');
    auto parse_result = parse_one_cpu(str);
    if (not parse_result.has_value()) {
      return make_unexpected(fmt::format("{}. Could not parse '{}' as a range", parse_result.error(), value));
    }

    range.push_back(parse_result.value());
  }

  // A range is defined by two numbers.
  if (range.size() != 2) {
    return make_unexpected(fmt::format("Could not parse '{}' as a range", value));
  }

  if (range[1] <= range[0]) {
    return make_unexpected(fmt::format("Invalid CPU core range detected [{}-{}]", range[0], range[1]));
  }

  return interval<unsigned, true>(range[0], range[1]);
}

static void
parse_affinity_mask(os_sched_affinity_bitmask& mask, const std::string& value, const std::string& property_name)
{
  std::stringstream ss(value);

  while (ss.good()) {
    std::string str;
    getline(ss, str, ',');
    if (str.find('-') != std::string::npos) {
      auto range = parse_cpu_range(str);
      if (not range.has_value()) {
        report_error("{} in the '{}' property", range.error(), property_name);
      }

      // Add 1 to the stop value as the fill method excludes the end position.
      mask.fill(range.value().start(), range.value().stop() + 1);
    } else {
      auto cpu_idx = parse_one_cpu(str);
      if (not cpu_idx.has_value()) {
        report_error("{} in the '{}' property", cpu_idx.error(), property_name);
      }

      mask.set(cpu_idx.value());
    }
  }
}

static void configure_cli11_cpu_affinities_args(CLI::App& app, cpu_affinities_appconfig& config)
{
  auto parsing_isolated_cpus_fcn = [](std::optional<os_sched_affinity_bitmask>& isolated_cpu_cfg,
                                      const std::string&                        value,
                                      const std::string&                        property_name) {
    isolated_cpu_cfg.emplace();
    parse_affinity_mask(*isolated_cpu_cfg, value, property_name);

    if (isolated_cpu_cfg->all()) {
      report_error("Error in '{}' property: can not assign all available CPUs to the gNB app", property_name);
    }
  };

  add_option_function<std::string>(
      app,
      "--isolated_cpus",
      [&config, &parsing_isolated_cpus_fcn](const std::string& value) {
        parsing_isolated_cpus_fcn(config.isolated_cpus, value, "isolated_cpus");
      },
      "CPU cores isolated for gNB application");

  add_option_function<std::string>(
      app,
      "--low_priority_cpus",
      [&config](const std::string& value) {
        parse_affinity_mask(config.low_priority_cpu_cfg.mask, value, "low_priority_cpus");
      },
      "CPU cores assigned to low priority tasks");

  add_option_function<std::string>(
      app,
      "--low_priority_pinning",
      [&config](const std::string& value) {
        config.low_priority_cpu_cfg.pinning_policy = to_affinity_mask_policy(value);
        if (config.low_priority_cpu_cfg.pinning_policy == sched_affinity_mask_policy::last) {
          report_error("Incorrect value={} used in {} property", value, "low_priority_pinning");
        }
      },
      "Policy used for assigning CPU cores to low priority tasks");
}

static void configure_cli11_non_rt_threads_args(CLI::App& app, non_rt_threads_appconfig& config)
{
  add_option(app,
             "--nof_non_rt_threads",
             config.nof_non_rt_threads,
             "Number of non real time threads for processing of CP and UP data in upper layers.")
      ->capture_default_str()
      ->check(CLI::Number);
}

static void configure_cli11_expert_execution_args(CLI::App& app, expert_execution_appconfig& config)
{
  // Affinity section.
  CLI::App* affinities_subcmd = add_subcommand(app, "affinities", "gNB CPU affinities configuration")->configurable();
  configure_cli11_cpu_affinities_args(*affinities_subcmd, config.affinities);

  // Threads section.
  CLI::App* threads_subcmd = add_subcommand(app, "threads", "Threads configuration")->configurable();

  // Non real time threads.
  CLI::App* non_rt_threads_subcmd =
      add_subcommand(*threads_subcmd, "non_rt", "Non real time thread configuration")->configurable();
  configure_cli11_non_rt_threads_args(*non_rt_threads_subcmd, config.threads.non_rt_threads);
}

static void configure_cli11_f1ap_args(CLI::App& app, srs_du::f1ap_appconfig& f1c_params)
{
  app.add_option("--cu_cp_addr", f1c_params.cu_cp_address, "CU-CP F1-C address to connect to")->capture_default_str();
  app.add_option(
         "--bind_addr", f1c_params.bind_address, "DU F1-C bind address. If left empty, implicit bind is performed")
      ->capture_default_str();
}

static void configure_cli11_f1u_args(CLI::App& app, srs_du::nru_appconfig& f1u_params)
{
  app.add_option("--queue_size", f1u_params.pdu_queue_size, "F1-U PDU queue size")->capture_default_str();
  app.add_option(
         "--bind_addr", f1u_params.bind_address, "DU F1-U bind address. If left empty, implicit bind is performed")
      ->capture_default_str();
}

static void configure_cli11_hal_args(CLI::App& app, std::optional<hal_appconfig>& config)
{
  config.emplace();

  add_option(app, "--eal_args", config->eal_args, "EAL configuration parameters used to initialize DPDK");
}

void srsran::configure_cli11_with_du_appconfig_schema(CLI::App& app, du_appconfig& du_cfg)
{
  // Loggers section.
  configure_cli11_with_logger_appconfig_schema(app, du_cfg.log_cfg);

  // Buffer pool section.
  configure_cli11_with_buffer_pool_appconfig_schema(app, du_cfg.buffer_pool_config);

  // F1-C section.
  CLI::App* f1ap_subcmd = app.add_subcommand("f1ap", "F1AP interface configuration")->configurable();
  configure_cli11_f1ap_args(*f1ap_subcmd, du_cfg.f1ap_cfg);

  // F1-U section.
  CLI::App* nru_subcmd = app.add_subcommand("nru", "NR-U interface configuration")->configurable();
  configure_cli11_f1u_args(*nru_subcmd, du_cfg.nru_cfg);

  // Metrics section.
  CLI::App* metrics_subcmd = app.add_subcommand("metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*metrics_subcmd, du_cfg.metrics_cfg);

  // E2 section.
  CLI::App* e2_subcmd = add_subcommand(app, "e2", "E2 parameters")->configurable();
  configure_cli11_e2_args(*e2_subcmd, du_cfg.e2_cfg);

  // Expert section.
  CLI::App* expert_subcmd = app.add_subcommand("expert_execution", "Expert execution configuration")->configurable();
  configure_cli11_expert_execution_args(*expert_subcmd, du_cfg.expert_execution_cfg);

  // HAL section.
  CLI::App* hal_subcmd = add_subcommand(app, "hal", "HAL configuration")->configurable();
  configure_cli11_hal_args(*hal_subcmd, du_cfg.hal_config);
}

void srsran::autoderive_du_parameters_after_parsing(CLI::App& app, du_appconfig& parsed_cfg) {}
