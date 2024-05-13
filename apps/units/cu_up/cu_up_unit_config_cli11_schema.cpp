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

#include "cu_up_unit_config_cli11_schema.h"
#include "apps/units/cu_up/cu_up_unit_config.h"
#include "srsran/support/cli11_utils.h"
#include "srsran/support/config_parsers.h"

using namespace srsran;

static void configure_cli11_cu_up_args(CLI::App& app, cu_up_unit_config& cu_up_params)
{
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
  auto level_check = [](const std::string& value) -> std::string {
    if (value == "info" || value == "debug" || value == "warning" || value == "error") {
      return {};
    }
    return "Log level value not supported. Accepted values [info,debug,warning,error]";
  };

  add_option(app, "--pdcp_level", log_params.pdcp_level, "PDCP log level")->capture_default_str()->check(level_check);
  add_option(app, "--sdap_level", log_params.sdap_level, "SDAP log level")->capture_default_str()->check(level_check);
  add_option(app, "--gtpu_level", log_params.gtpu_level, "GTPU log level")->capture_default_str()->check(level_check);
  add_option(app, "--f1u_level", log_params.f1u_level, "F1-U log level")->capture_default_str()->check(level_check);
  add_option(app, "--cu_level", log_params.cu_level, "Log level for the CU")->capture_default_str()->check(level_check);

  add_option(
      app, "--hex_max_size", log_params.hex_max_size, "Maximum number of bytes to print in hex (zero for no hex dumps)")
      ->capture_default_str()
      ->check(CLI::Range(0, 1024));
}

static void configure_cli11_metrics_args(CLI::App& app, cu_up_unit_metrics_config& metrics_params)
{
  add_option(app,
             "--cu_up_statistics_report_period",
             metrics_params.cu_up_statistics_report_period,
             "CU-UP statistics report period in seconds. Set this value to 0 to disable this feature")
      ->capture_default_str();

  add_option(app, "--pdcp_report_period", metrics_params.pdcp.report_period, "PDCP metrics report period")
      ->capture_default_str();
}

static void configure_cli11_amf_args(CLI::App& app, cu_up_unit_amf_config& amf_params)
{
  add_option(app, "--addr", amf_params.ip_addr, "AMF IP address");
  add_option(app, "--port", amf_params.port, "AMF port")->capture_default_str()->check(CLI::Range(20000, 40000));
  add_option(app,
             "--bind_addr",
             amf_params.bind_addr,
             "Default local IP address interfaces bind to, unless a specific bind address is specified")
      ->check(CLI::ValidIPV4);
  add_option(app, "--n2_bind_addr", amf_params.n2_bind_addr, "Local IP address to bind for N2 interface")
      ->check(CLI::ValidIPV4);
  add_option(app, "--n3_bind_addr", amf_params.n3_bind_addr, "Local IP address to bind for N3 interface")
      ->check(CLI::ValidIPV4);
  add_option(app, "--n3_bind_interface", amf_params.n3_bind_interface, "Network device to bind for N3 interface")
      ->capture_default_str();
  add_option(app, "--n2_bind_interface", amf_params.n2_bind_interface, "Network device to bind for N2 interface")
      ->capture_default_str();
  add_option(app,
             "--n3_ext_addr",
             amf_params.n3_ext_addr,
             "External IP address that is advertised to receive GTP-U packets from UPF via N3 interface")
      ->check(CLI::ValidIPV4);
  add_option(app, "--sctp_rto_initial", amf_params.sctp_rto_initial, "SCTP initial RTO value");
  add_option(app, "--sctp_rto_min", amf_params.sctp_rto_min, "SCTP RTO min");
  add_option(app, "--sctp_rto_max", amf_params.sctp_rto_max, "SCTP RTO max");
  add_option(app, "--sctp_init_max_attempts", amf_params.sctp_init_max_attempts, "SCTP init max attempts");
  add_option(app, "--sctp_max_init_timeo", amf_params.sctp_max_init_timeo, "SCTP max init timeout");
  add_option(app,
             "--sctp_nodelay",
             amf_params.sctp_nodelay,
             "Send SCTP messages as soon as possible without any Nagle-like algorithm");
  add_option(app, "--udp_max_rx_msgs", amf_params.udp_rx_max_msgs, "Maximum amount of messages RX in a single syscall");
  add_option(app, "--no_core", amf_params.no_core, "Allow gNB to run without a core");
}

static void configure_cli11_qos_args(CLI::App& app, cu_up_unit_qos_config& qos_params)
{
  add_option(app, "--five_qi", qos_params.five_qi, "5QI")->capture_default_str()->check(CLI::Range(0, 255));
}

void srsran::configure_cli11_with_cu_up_unit_config_schema(CLI::App& app, cu_up_unit_config& unit_cfg)
{
  // CU-UP section.
  CLI::App* cu_up_subcmd = add_subcommand(app, "cu_up", "CU-CP parameters")->configurable();
  configure_cli11_cu_up_args(*cu_up_subcmd, unit_cfg);

  // Loggers section.
  CLI::App* log_subcmd = add_subcommand(app, "log", "Logging configuration")->configurable();
  configure_cli11_log_args(*log_subcmd, unit_cfg.loggers);

  // Metrics section.
  CLI::App* metrics_subcmd = add_subcommand(app, "metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*metrics_subcmd, unit_cfg.metrics);

  // AMF section.
  CLI::App* amf_subcmd = add_subcommand(app, "amf", "AMF parameters")->configurable();
  configure_cli11_amf_args(*amf_subcmd, unit_cfg.amf_cfg);

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
