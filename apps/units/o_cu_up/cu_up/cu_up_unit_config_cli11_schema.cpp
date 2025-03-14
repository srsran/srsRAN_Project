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

#include "cu_up_unit_config_cli11_schema.h"
#include "apps/helpers/logger/logger_appconfig_cli11_utils.h"
#include "apps/helpers/metrics/metrics_config_cli11_schema.h"
#include "apps/helpers/network/udp_cli11_schema.h"
#include "apps/units/o_cu_up/cu_up/cu_up_unit_config.h"
#include "apps/units/o_cu_up/cu_up/cu_up_unit_pcap_config.h"
#include "srsran/support/cli11_utils.h"
#include "srsran/support/config_parsers.h"

using namespace srsran;

static void configure_cli11_ngu_socket_args(CLI::App& app, cu_up_unit_ngu_socket_config& ngu_sock_params)
{
  add_option(app, "--bind_addr", ngu_sock_params.bind_addr, "Local IP address to bind for N3 interface")
      ->check(CLI::ValidIPV4 | CLI::IsMember({"auto"}));
  add_option(app, "--bind_interface", ngu_sock_params.bind_interface, "Network device to bind for N3 interface")
      ->capture_default_str();
  add_option(app,
             "--ext_addr",
             ngu_sock_params.ext_addr,
             "External IP address that is advertised to receive GTP-U packets from UPF via N3 interface")
      ->check(CLI::ValidIPV4 | CLI::IsMember({"auto"}));

  configure_cli11_with_udp_config_schema(app, ngu_sock_params.udp_config);
}

static void configure_cli11_ngu_args(CLI::App& app, cu_up_unit_ngu_config& ngu_params)
{
  add_option(app, "--no_core", ngu_params.no_core, "Allow gNB to run without a core");

  // Add option for multiple sockets, for usage with different slices, 5QIs or parallization.
  auto sock_lambda = [&ngu_params](const std::vector<std::string>& values) {
    // Prepare the radio bearers
    ngu_params.ngu_socket_cfg.resize(values.size());

    // Format every QoS setting.
    for (unsigned i = 0, e = values.size(); i != e; ++i) {
      CLI::App subapp("NG-U socket parameters", "NG-U socket config, item #" + std::to_string(i));
      subapp.config_formatter(create_yaml_config_parser());
      subapp.allow_config_extras(CLI::config_extras_mode::capture);
      configure_cli11_ngu_socket_args(subapp, ngu_params.ngu_socket_cfg[i]);
      std::istringstream ss(values[i]);
      subapp.parse_from_stream(ss);
    }
  };
  add_option_cell(app, "--socket", sock_lambda, "Configures UDP/IP socket parameters of the N3 interface");
}

static void configure_cli11_test_mode_args(CLI::App& app, cu_up_unit_test_mode_config& test_mode_params)
{
  add_option(app, "--enable", test_mode_params.enabled, "Enable or disable CU-UP test mode");
  add_option(app, "--integrity_enable", test_mode_params.integrity_enabled, "Enable or disable PDCP integrity testing");
  add_option(app, "--ciphering_enable", test_mode_params.ciphering_enabled, "Enable or disable PDCP ciphering testing");
  add_option(app, "--nea_algo", test_mode_params.nea_algo, "NEA algo to use for testing. Valid values {0, 1, 2, 3}.")
      ->capture_default_str()
      ->check(CLI::Range(0, 3));
  add_option(app, "--nia_algo", test_mode_params.nea_algo, "NIA algo to use for testing. Valid values {1, 2, 3}.")
      ->capture_default_str()
      ->check(CLI::Range(1, 3));
}

static void configure_cli11_cu_up_args(CLI::App& app, cu_up_unit_config& cu_up_params)
{
  // UPF section.
  CLI::App* ngu_subcmd = add_subcommand(app, "ngu", "NG-U parameters")->configurable();
  configure_cli11_ngu_args(*ngu_subcmd, cu_up_params.ngu_cfg);

  // Test mode section.
  CLI::App* test_mode_subcmd = add_subcommand(app, "test_mode", "CU-UP test mode parameters")->configurable();
  configure_cli11_test_mode_args(*test_mode_subcmd, cu_up_params.test_mode_cfg);

  add_option(app, "--gtpu_queue_size", cu_up_params.gtpu_queue_size, "GTP-U queue size, in PDUs")
      ->capture_default_str();
  add_option(app,
             "--gtpu_reordering_timer",
             cu_up_params.gtpu_reordering_timer_ms,
             "GTP-U RX reordering timer (in milliseconds)")
      ->capture_default_str();
  add_option(app,
             "--warn_on_drop",
             cu_up_params.warn_on_drop,
             "Log a warning for dropped packets in GTP-U, SDAP, PDCP and F1-U due to full queues")
      ->capture_default_str();
}

static void configure_cli11_log_args(CLI::App& app, cu_up_unit_logger_config& log_params)
{
  app_helpers::add_log_option(app, log_params.pdcp_level, "--pdcp_level", "PDCP log level");
  app_helpers::add_log_option(app, log_params.sdap_level, "--sdap_level", "SDAP log level");
  app_helpers::add_log_option(app, log_params.gtpu_level, "--gtpu_level", "GTPU log level");
  app_helpers::add_log_option(app, log_params.e1ap_level, "--e1ap_level", "E1AP log level");
  app_helpers::add_log_option(app, log_params.f1u_level, "--f1u_level", "F1-U log level");
  app_helpers::add_log_option(app, log_params.cu_level, "--cu_level", "Log level for the CU");

  add_option(
      app, "--hex_max_size", log_params.hex_max_size, "Maximum number of bytes to print in hex (zero for no hex dumps)")
      ->capture_default_str()
      ->check(CLI::Range(0, 1024));
  add_option(app, "--e1ap_json_enabled", log_params.e1ap_json_enabled, "Enable JSON logging of E1AP PDUs")
      ->always_capture_default();
}

static void configure_cli11_pcap_args(CLI::App& app, cu_up_unit_pcap_config& pcap_params)
{
  add_option(app, "--n3_filename", pcap_params.n3.filename, "N3 GTP-U PCAP file output path")->capture_default_str();
  add_option(app, "--n3_enable", pcap_params.n3.enabled, "Enable N3 GTP-U packet capture")->always_capture_default();
  add_option(app, "--f1u_filename", pcap_params.f1u.filename, "F1-U GTP-U PCAP file output path")
      ->capture_default_str();
  add_option(app, "--f1u_enable", pcap_params.f1u.enabled, "F1-U GTP-U PCAP")->always_capture_default();
  add_option(app, "--e1ap_filename", pcap_params.e1ap.filename, "E1AP PCAP file output path")->capture_default_str();
  add_option(app, "--e1ap_enable", pcap_params.e1ap.enabled, "E1AP PCAP")->always_capture_default();
}

static void configure_cli11_metrics_args(CLI::App& app, cu_up_unit_metrics_config& metrics_params)
{
  add_option(app,
             "--cu_up_statistics_report_period",
             metrics_params.cu_up_statistics_report_period,
             "CU-UP statistics report period in seconds. Set this value to 0 to disable this feature")
      ->capture_default_str();

  add_option(
      app, "--pdcp_report_period", metrics_params.pdcp.report_period, "PDCP metrics report period (in milliseconds)")
      ->capture_default_str();
}

static void configure_cli11_f1u_cu_up_args(CLI::App& app, cu_cp_unit_f1u_config& f1u_cu_up_params)
{
  app.add_option("--backoff_timer", f1u_cu_up_params.t_notify, "F1-U backoff timer (ms)")->capture_default_str();
}

static void configure_cli11_qos_args(CLI::App& app, cu_up_unit_qos_config& qos_params)
{
  add_option(app, "--five_qi", qos_params.five_qi, "5QI")->capture_default_str()->check(CLI::Range(0, 255));

  CLI::App* f1u_cu_up_subcmd = app.add_subcommand("f1u_cu_up", "F1-U parameters at CU_UP side");
  configure_cli11_f1u_cu_up_args(*f1u_cu_up_subcmd, qos_params.f1u_cu_up);
}

void srsran::configure_cli11_with_cu_up_unit_config_schema(CLI::App& app, cu_up_unit_config& unit_cfg)
{
  add_option(app, "--gnb_id", unit_cfg.gnb_id.id, "gNodeB identifier")->capture_default_str();
  // Adding a default function to display correctly the uint8_t type.
  add_option(app, "--gnb_id_bit_length", unit_cfg.gnb_id.bit_length, "gNodeB identifier length in bits")
      ->default_function([&value = unit_cfg.gnb_id.bit_length]() { return std::to_string(value); })
      ->capture_default_str()
      ->check(CLI::Range(22, 32));
  add_option(app, "--gnb_cu_up_id", unit_cfg.gnb_cu_up_id, "gNB-CU-UP Id")
      ->capture_default_str()
      ->check(CLI::Range(static_cast<uint64_t>(0U), static_cast<uint64_t>(pow(2, 36) - 1)));

  // CU-UP section.
  CLI::App* cu_up_subcmd = add_subcommand(app, "cu_up", "CU-UP parameters")->configurable();
  configure_cli11_cu_up_args(*cu_up_subcmd, unit_cfg);

  // Loggers section.
  CLI::App* log_subcmd = add_subcommand(app, "log", "Logging configuration")->configurable();
  configure_cli11_log_args(*log_subcmd, unit_cfg.loggers);

  // PCAP section.
  CLI::App* pcap_subcmd = add_subcommand(app, "pcap", "Logging configuration")->configurable();
  configure_cli11_pcap_args(*pcap_subcmd, unit_cfg.pcap_cfg);

  // Metrics section.
  CLI::App* metrics_subcmd = add_subcommand(app, "metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*metrics_subcmd, unit_cfg.metrics);
  app_helpers::configure_cli11_with_metrics_appconfig_schema(app, unit_cfg.metrics.common_metrics_cfg);

  // QoS section.
  auto qos_lambda = [&unit_cfg](const std::vector<std::string>& values) {
    // Prepare the radio bearers
    unit_cfg.qos_cfg.resize(values.size());

    // Format every QoS setting.
    for (unsigned i = 0, e = values.size(); i != e; ++i) {
      CLI::App subapp("QoS parameters", "QoS config, item #" + std::to_string(i));
      subapp.config_formatter(create_yaml_config_parser());
      subapp.allow_config_extras(CLI::config_extras_mode::capture);
      configure_cli11_qos_args(subapp, unit_cfg.qos_cfg[i]);
      std::istringstream ss(values[i]);
      subapp.parse_from_stream(ss);
    }
  };
  add_option_cell(app, "--qos", qos_lambda, "Configures RLC and PDCP radio bearers on a per 5QI basis.");
}

void srsran::autoderive_cu_up_parameters_after_parsing(CLI::App&          app,
                                                       cu_up_unit_config& unit_cfg,
                                                       bool               pdpc_metrics_requested)
{
  // Do nothing when PDCP period if present in the config file.
  if (auto* metrics_subcmd = app.get_subcommand("metrics");
      metrics_subcmd && metrics_subcmd->count("--pdcp_report_period")) {
    // Disable metrics if report period is configured to 0.
    unit_cfg.metrics.common_metrics_cfg.enable_log_metrics              = unit_cfg.metrics.pdcp.report_period != 0;
    unit_cfg.metrics.common_metrics_cfg.json_config.enable_json_metrics = unit_cfg.metrics.pdcp.report_period != 0;

    return;
  }

  // Disable PDCP report period when the metrics and E2 are not enabled.
  if (!pdpc_metrics_requested && !unit_cfg.metrics.common_metrics_cfg.enabled()) {
    unit_cfg.metrics.pdcp.report_period = 0;
  }
}
