/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_unit_config_cli11_schema.h"
#include "apps/services/logger/logger_appconfig_cli11_utils.h"
#include "apps/units/cu_up/cu_up_unit_config.h"
#include "apps/units/cu_up/cu_up_unit_pcap_config.h"
#include "srsran/support/cli11_utils.h"
#include "srsran/support/config_parsers.h"

using namespace srsran;

static void configure_cli11_upf_args(CLI::App& app, cu_up_unit_upf_config& upf_params)
{
  add_option(app, "--bind_addr", upf_params.bind_addr, "Local IP address to bind for N3 interface")
      ->check(CLI::ValidIPV4 | CLI::IsMember({"auto"}));
  add_option(app, "--bind_interface", upf_params.bind_interface, "Network device to bind for N3 interface")
      ->capture_default_str();
  add_option(app,
             "--ext_addr",
             upf_params.ext_addr,
             "External IP address that is advertised to receive GTP-U packets from UPF via N3 interface")
      ->check(CLI::ValidIPV4 | CLI::IsMember({"auto"}));
  add_option(app, "--udp_max_rx_msgs", upf_params.udp_rx_max_msgs, "Maximum amount of messages RX in a single syscall");
  add_option(
      app, "--pool_threshold", upf_params.pool_threshold, "Pool accupancy threshold after which packets are dropped")
      ->check(CLI::Range(0.0, 1.0));
  add_option(app, "--no_core", upf_params.no_core, "Allow gNB to run without a core");
}

static void configure_cli11_cu_up_args(CLI::App& app, cu_up_unit_config& cu_up_params)
{
  // UPF section.
  CLI::App* upf_subcmd = add_subcommand(app, "upf", "UPF parameters")->configurable();
  configure_cli11_upf_args(*upf_subcmd, cu_up_params.upf_cfg);

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
  app_services::add_log_option(app, log_params.pdcp_level, "--pdcp_level", "PDCP log level");
  app_services::add_log_option(app, log_params.sdap_level, "--sdap_level", "SDAP log level");
  app_services::add_log_option(app, log_params.gtpu_level, "--gtpu_level", "GTPU log level");
  app_services::add_log_option(app, log_params.f1u_level, "--f1u_level", "F1-U log level");
  app_services::add_log_option(app, log_params.cu_level, "--cu_level", "Log level for the CU");

  add_option(
      app, "--hex_max_size", log_params.hex_max_size, "Maximum number of bytes to print in hex (zero for no hex dumps)")
      ->capture_default_str()
      ->check(CLI::Range(0, 1024));
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

static void configure_cli11_e2_args(CLI::App& app, e2_config& e2_params)
{
  add_option(app, "--enable_cu_up_e2", e2_params.enable_unit_e2, "Enable CU E2 agent")->capture_default_str();
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

void srsran::configure_cli11_with_cu_up_unit_config_schema(CLI::App& app, cu_up_unit_config& unit_cfg)
{
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

  // E2 section.
  CLI::App* e2_subcmd = add_subcommand(app, "e2", "E2 parameters")->configurable();
  configure_cli11_e2_args(*e2_subcmd, unit_cfg.e2_cfg);

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

  // Test mode section.
  CLI::App* test_mode_subcmd = add_subcommand(app, "test_mode", "CU-UP test mode parameters")->configurable();
  configure_cli11_test_mode_args(*test_mode_subcmd, unit_cfg.test_mode_cfg);
}
