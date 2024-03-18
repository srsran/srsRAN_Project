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

#include "gnb_appconfig_cli11_schema.h"
#include "gnb_appconfig.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/pdsch/pdsch_mcs.h"
#include "srsran/support/cli11_utils.h"
#include "srsran/support/config_parsers.h"
#include "srsran/support/error_handling.h"
#include "CLI/CLI11.hpp"

using namespace srsran;

template <typename Integer>
static expected<Integer, std::string> parse_int(const std::string& value)
{
  try {
    return std::stoi(value);
  } catch (const std::invalid_argument& e) {
    return {e.what()};
  } catch (const std::out_of_range& e) {
    return {e.what()};
  }
}

static void configure_cli11_log_args(CLI::App& app, log_appconfig& log_params)
{
  auto level_check = [](const std::string& value) -> std::string {
    if (value == "info" || value == "debug" || value == "warning" || value == "error") {
      return {};
    }
    return "Log level value not supported. Accepted values [info,debug,warning,error]";
  };

  app.add_option("--filename", log_params.filename, "Log file output path")->capture_default_str();
  app.add_option(
         "--all_level", log_params.all_level, "Default log level for PHY, MAC, RLC, PDCP, RRC, SDAP, NGAP and GTPU")
      ->capture_default_str()
      ->check(level_check);
  app.add_option("--phy_level", log_params.phy_level, "PHY log level")->capture_default_str()->check(level_check);
  app.add_option("--mac_level", log_params.mac_level, "MAC log level")->capture_default_str()->check(level_check);
  app.add_option("--rlc_level", log_params.rlc_level, "RLC log level")->capture_default_str()->check(level_check);
  app.add_option("--pdcp_level", log_params.pdcp_level, "PDCP log level")->capture_default_str()->check(level_check);
  app.add_option("--rrc_level", log_params.rrc_level, "RRC log level")->capture_default_str()->check(level_check);
  app.add_option("--sdap_level", log_params.sdap_level, "SDAP log level")->capture_default_str()->check(level_check);
  app.add_option("--ngap_level", log_params.ngap_level, "NGAP log level")->capture_default_str()->check(level_check);
  app.add_option("--gtpu_level", log_params.gtpu_level, "GTPU log level")->capture_default_str()->check(level_check);
  app.add_option("--radio_level", log_params.radio_level, "Radio log level")->capture_default_str()->check(level_check);
  app.add_option("--fapi_level", log_params.fapi_level, "FAPI log level")->capture_default_str()->check(level_check);
  app.add_option("--ofh_level", log_params.ofh_level, "Open Fronthaul log level")
      ->capture_default_str()
      ->check(level_check);
  app.add_option("--f1ap_level", log_params.f1ap_level, "F1AP log level")->capture_default_str()->check(level_check);
  app.add_option("--f1u_level", log_params.f1u_level, "F1-U log level")->capture_default_str()->check(level_check);
  app.add_option("--du_level", log_params.du_level, "Log level for the DU")->capture_default_str()->check(level_check);
  app.add_option("--cu_level", log_params.cu_level, "Log level for the CU")->capture_default_str()->check(level_check);
  app.add_option("--sec_level", log_params.sec_level, "Security functions log level")
      ->capture_default_str()
      ->check(level_check);
  app.add_option("--lib_level", log_params.lib_level, "Generic log level")->capture_default_str()->check(level_check);
  app.add_option(
         "--hex_max_size", log_params.hex_max_size, "Maximum number of bytes to print in hex (zero for no hex dumps)")
      ->capture_default_str()
      ->check(CLI::Range(0, 1024));
  app.add_option("--broadcast_enabled",
                 log_params.broadcast_enabled,
                 "Enable logging in the physical and MAC layer of broadcast messages and all PRACH opportunities")
      ->always_capture_default();
  app.add_option("--phy_rx_symbols_filename",
                 log_params.phy_rx_symbols_filename,
                 "Set to a valid file path to print the received symbols.")
      ->always_capture_default();
  app.add_option_function<std::string>(
         "--phy_rx_symbols_port",
         [&log_params](const std::string& value) {
           if (value == "all") {
             log_params.phy_rx_symbols_port = nullopt;
           } else {
             log_params.phy_rx_symbols_port = parse_int<unsigned>(value).value();
           }
         },
         "Set to a valid receive port number to dump the IQ symbols from that port only, or set to \"all\" to dump the "
         "IQ symbols from all UL receive ports. Only works if \"phy_rx_symbols_filename\" is set.")
      ->default_str("0")
      ->check(CLI::NonNegativeNumber | CLI::IsMember({"all"}));
  app.add_option("--phy_rx_symbols_prach",
                 log_params.phy_rx_symbols_prach,
                 "Set to true to dump the IQ symbols from all the PRACH ports. Only works if "
                 "\"phy_rx_symbols_filename\" is set.")
      ->capture_default_str();
  app.add_option("--tracing_filename", log_params.tracing_filename, "Set to a valid file path to enable tracing")
      ->always_capture_default();

  // Post-parsing callback. This allows us to set the log level to "all" level, if no level is provided.
  app.callback([&]() {
    if (app.count("--phy_level") == 0) {
      log_params.phy_level = log_params.all_level;
    }
    if (app.count("--mac_level") == 0) {
      log_params.mac_level = log_params.all_level;
    }
    if (app.count("--rlc_level") == 0) {
      log_params.rlc_level = log_params.all_level;
    }
    if (app.count("--f1ap_level") == 0) {
      log_params.f1ap_level = log_params.all_level;
    }
    if (app.count("--pdcp_level") == 0) {
      log_params.pdcp_level = log_params.all_level;
    }
    if (app.count("--rrc_level") == 0) {
      log_params.rrc_level = log_params.all_level;
    }
    if (app.count("--sdap_level") == 0) {
      log_params.sdap_level = log_params.all_level;
    }
    if (app.count("--ngap_level") == 0) {
      log_params.ngap_level = log_params.all_level;
    }
    if (app.count("--gtpu_level") == 0) {
      log_params.gtpu_level = log_params.all_level;
    }
    // Update the radio log level to all levels when radio level was not found and all level is not warning.
    if (app.count("--radio_level") == 0 && log_params.all_level != "warning") {
      log_params.radio_level = log_params.all_level;
    }
    if (app.count("--fapi_level") == 0) {
      log_params.fapi_level = log_params.all_level;
    }
    if (app.count("--ofh_level") == 0) {
      log_params.ofh_level = log_params.all_level;
    }
    if (app.count("--f1ap_level") == 0) {
      log_params.f1ap_level = log_params.all_level;
    }
    if (app.count("--f1u_level") == 0) {
      log_params.f1u_level = log_params.all_level;
    }
    if (app.count("--du_level") == 0) {
      log_params.du_level = log_params.all_level;
    }
    if (app.count("--cu_level") == 0) {
      log_params.cu_level = log_params.all_level;
    }
    if (app.count("--sec_level") == 0) {
      log_params.sec_level = log_params.all_level;
    }
    if (app.count("--lib_level") == 0) {
      log_params.lib_level = log_params.all_level;
    }
  });
}

static void configure_cli11_pcap_args(CLI::App& app, pcap_appconfig& pcap_params)
{
  app.add_option("--ngap_filename", pcap_params.ngap.filename, "NGAP PCAP file output path")->capture_default_str();
  app.add_option("--ngap_enable", pcap_params.ngap.enabled, "Enable NGAP packet capture")->always_capture_default();
  app.add_option("--e1ap_filename", pcap_params.e1ap.filename, "E1AP PCAP file output path")->capture_default_str();
  app.add_option("--e1ap_enable", pcap_params.e1ap.enabled, "Enable E1AP packet capture")->always_capture_default();
  app.add_option("--f1ap_filename", pcap_params.f1ap.filename, "F1AP PCAP file output path")->capture_default_str();
  app.add_option("--f1ap_enable", pcap_params.f1ap.enabled, "Enable F1AP packet capture")->always_capture_default();
  app.add_option("--rlc_filename", pcap_params.rlc.filename, "RLC PCAP file output path")->capture_default_str();
  app.add_option("--rlc_rb_type", pcap_params.rlc.rb_type, "RLC PCAP RB type (all, srb, drb)")->capture_default_str();
  app.add_option("--rlc_enable", pcap_params.rlc.enabled, "Enable RLC packet capture")->always_capture_default();
  app.add_option("--mac_filename", pcap_params.mac.filename, "MAC PCAP file output path")->capture_default_str();
  app.add_option("--mac_type", pcap_params.mac.type, "MAC PCAP pcap type (dlt or udp)")->capture_default_str();
  app.add_option("--mac_enable", pcap_params.mac.enabled, "Enable MAC packet capture")->always_capture_default();
  app.add_option("--e2ap_filename", pcap_params.e2ap.filename, "E2AP PCAP file output path")->capture_default_str();
  app.add_option("--e2ap_enable", pcap_params.e2ap.enabled, "Enable E2AP packet capture")->always_capture_default();
  app.add_option("--gtpu_filename", pcap_params.gtpu.filename, "GTP-U PCAP file output path")->capture_default_str();
  app.add_option("--gtpu_enable", pcap_params.gtpu.enabled, "Enable GTP-U packet capture")->always_capture_default();
}

static void configure_cli11_metrics_args(CLI::App& app, metrics_appconfig& metrics_params)
{
  app.add_option("--rlc_report_period", metrics_params.rlc.report_period, "RLC metrics report period")
      ->capture_default_str();
  app.add_option("--rlc_json_enable", metrics_params.rlc.json_enabled, "Enable RLC JSON metrics reporting")
      ->always_capture_default();

  app.add_option("--pdcp_report_period", metrics_params.pdcp.report_period, "PDCP metrics report period")
      ->capture_default_str();

  app.add_option("--cu_cp_statistics_report_period",
                 metrics_params.cu_cp_statistics_report_period,
                 "CU-CP statistics report period in seconds. Set this value to 0 to disable this feature")
      ->capture_default_str();

  app.add_option("--cu_up_statistics_report_period",
                 metrics_params.cu_up_statistics_report_period,
                 "CU-UP statistics report period in seconds. Set this value to 0 to disable this feature")
      ->capture_default_str();

  app.add_option("--enable_json_metrics", metrics_params.enable_json_metrics, "Enable JSON metrics reporting")
      ->always_capture_default();

  app.add_option("--addr", metrics_params.addr, "Metrics address.")->capture_default_str()->check(CLI::ValidIPV4);
  app.add_option("--port", metrics_params.port, "Metrics UDP port.")
      ->capture_default_str()
      ->check(CLI::Range(0, 65535));

  app.add_option(
         "--autostart_stdout_metrics", metrics_params.autostart_stdout_metrics, "Autostart stdout metrics reporting")
      ->capture_default_str();

  app.add_option("--stdout_metrics_period",
                 metrics_params.stdout_metrics_period,
                 "DU statistics report period in milliseconds. This metrics sets the console output period.")
      ->capture_default_str();
}

static void configure_cli11_slicing_args(CLI::App& app, s_nssai_t& slice_params)
{
  app.add_option("--sst", slice_params.sst, "Slice Service Type")->capture_default_str()->check(CLI::Range(0, 255));
  app.add_option("--sd", slice_params.sd, "Service Differentiator")
      ->capture_default_str()
      ->check(CLI::Range(0, 0xffffff));
}

static void configure_cli11_amf_args(CLI::App& app, amf_appconfig& amf_params)
{
  app.add_option("--addr", amf_params.ip_addr, "AMF IP address");
  app.add_option("--port", amf_params.port, "AMF port")->capture_default_str()->check(CLI::Range(20000, 40000));
  app.add_option("--bind_addr",
                 amf_params.bind_addr,
                 "Default local IP address interfaces bind to, unless a specific bind address is specified")
      ->check(CLI::ValidIPV4);
  app.add_option("--n2_bind_addr", amf_params.n2_bind_addr, "Local IP address to bind for N2 interface")
      ->check(CLI::ValidIPV4);
  app.add_option("--n3_bind_addr", amf_params.n3_bind_addr, "Local IP address to bind for N3 interface")
      ->check(CLI::ValidIPV4);
  app.add_option("--n3_bind_interface", amf_params.n3_bind_interface, "Network device to bind for N3 interface")
      ->capture_default_str();
  app.add_option("--n2_bind_interface", amf_params.n2_bind_interface, "Network device to bind for N2 interface")
      ->capture_default_str();
  app.add_option("--sctp_rto_initial", amf_params.sctp_rto_initial, "SCTP initial RTO value");
  app.add_option("--sctp_rto_min", amf_params.sctp_rto_min, "SCTP RTO min");
  app.add_option("--sctp_rto_max", amf_params.sctp_rto_max, "SCTP RTO max");
  app.add_option("--sctp_init_max_attempts", amf_params.sctp_init_max_attempts, "SCTP init max attempts");
  app.add_option("--sctp_max_init_timeo", amf_params.sctp_max_init_timeo, "SCTP max init timeout");
  app.add_option("--sctp_nodelay",
                 amf_params.sctp_nodelay,
                 "Send SCTP messages as soon as possible without any Nagle-like algorithm");
  app.add_option("--udp_max_rx_msgs", amf_params.udp_rx_max_msgs, "Maximum amount of messages RX in a single syscall");
  app.add_option("--no_core", amf_params.no_core, "Allow gNB to run without a core");
}

static void configure_cli11_e2_args(CLI::App& app, e2_appconfig& e2_params)
{
  app.add_option("--enable_du_e2", e2_params.enable_du_e2, "Enable DU E2 agent");
  app.add_option("--addr", e2_params.ip_addr, "RIC IP address");
  app.add_option("--port", e2_params.port, "RIC port")->capture_default_str()->check(CLI::Range(20000, 40000));
  app.add_option("--bind_addr", e2_params.bind_addr, "Local IP address to bind for RIC connection")
      ->check(CLI::ValidIPV4);
  app.add_option("--sctp_rto_initial", e2_params.sctp_rto_initial, "SCTP initial RTO value");
  app.add_option("--sctp_rto_min", e2_params.sctp_rto_min, "SCTP RTO min");
  app.add_option("--sctp_rto_max", e2_params.sctp_rto_max, "SCTP RTO max");
  app.add_option("--sctp_init_max_attempts", e2_params.sctp_init_max_attempts, "SCTP init max attempts");
  app.add_option("--sctp_max_init_timeo", e2_params.sctp_max_init_timeo, "SCTP max init timeout");
  app.add_option("--e2sm_kpm_enabled", e2_params.e2sm_kpm_enabled, "Enable KPM service module");
  app.add_option("--e2sm_rc_enabled", e2_params.e2sm_rc_enabled, "Enable RC service module");
}

static void configure_cli11_ncell_args(CLI::App& app, cu_cp_neighbor_cell_appconfig_item& config)
{
  app.add_option("--nr_cell_id", config.nr_cell_id, "Neighbor cell id");
  app.add_option(
      "--report_configs", config.report_cfg_ids, "Report configurations to configure for this neighbor cell");
}

static void configure_cli11_cells_args(CLI::App& app, cu_cp_cell_appconfig_item& config)
{
  app.add_option("--nr_cell_id", config.nr_cell_id, "Cell id to be configured");
  app.add_option("--rat", config.rat, "RAT of this neighbor cell")->capture_default_str();
  app.add_option("--periodic_report_cfg_id",
                 config.periodic_report_cfg_id,
                 "Periodical report configuration for the serving cell")
      ->check(CLI::Range(1, 64));
  ;
  add_auto_enum_option(app, "--band", config.band, "NR frequency band");

  app.add_option("--gnb_id", config.gnb_id, "gNodeB identifier");
  app.add_option("--ssb_arfcn", config.ssb_arfcn, "SSB ARFCN");
  app.add_option("--ssb_scs", config.ssb_scs, "SSB subcarrier spacing")->check(CLI::IsMember({15, 30, 60, 120, 240}));
  app.add_option("--ssb_period", config.ssb_period, "SSB period in ms");
  app.add_option("--ssb_offset", config.ssb_offset, "SSB offset");
  app.add_option("--ssb_duration", config.ssb_duration, "SSB duration");

  // report configuration parameters.
  app.add_option_function<std::vector<std::string>>(
      "--ncells",
      [&config](const std::vector<std::string>& values) {
        config.ncells.resize(values.size());

        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("CU-CP neighbor cell list");
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras(CLI::config_extras_mode::error);
          configure_cli11_ncell_args(subapp, config.ncells[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Sets the list of neighbor cells known to the CU-CP");
}

static void configure_cli11_report_args(CLI::App& app, cu_cp_report_appconfig& report_params)
{
  app.add_option("--report_cfg_id", report_params.report_cfg_id, "Report configuration id to be configured")
      ->check(CLI::Range(1, 64));
  app.add_option("--report_type", report_params.report_type, "Type of the report configuration")
      ->check(CLI::IsMember({"periodical", "event_triggered"}));
  app.add_option("--report_interval_ms", report_params.report_interval_ms, "Report interval in ms");
  app.add_option("--a3_report_type", report_params.a3_report_type, "A3 report type");
  app.add_option("--a3_offset_db", report_params.a3_offset_db, "A3 offset in dB used for measurement report trigger");
  app.add_option(
      "--a3_hysteresis_db", report_params.a3_hysteresis_db, "A3 hysteresis in dB used for measurement report trigger");
  app.add_option("--a3_time_to_trigger_ms",
                 report_params.a3_time_to_trigger_ms,
                 "Time in ms during which A3 condition must be met before measurement report trigger");
}

static void configure_cli11_mobility_args(CLI::App& app, mobility_appconfig& config)
{
  app.add_option("--trigger_handover_from_measurements",
                 config.trigger_handover_from_measurements,
                 "Whether to start HO if neighbor cells become stronger")
      ->capture_default_str();

  // Cell map parameters.
  app.add_option_function<std::vector<std::string>>(
      "--cells",
      [&config](const std::vector<std::string>& values) {
        config.cells.resize(values.size());

        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("CU-CP cell list");
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras(CLI::config_extras_mode::error);
          configure_cli11_cells_args(subapp, config.cells[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Sets the list of cells known to the CU-CP");

  // report configuration parameters.
  app.add_option_function<std::vector<std::string>>(
      "--report_configs",
      [&config](const std::vector<std::string>& values) {
        config.report_configs.resize(values.size());

        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("CU-CP measurement report config list");
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras(CLI::config_extras_mode::error);
          configure_cli11_report_args(subapp, config.report_configs[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Sets report configurations");
}

static void configure_cli11_rrc_args(CLI::App& app, rrc_appconfig& config)
{
  app.add_option("--force_reestablishment_fallback",
                 config.force_reestablishment_fallback,
                 "Force RRC re-establishment fallback to RRC setup")
      ->capture_default_str();

  app.add_option(
         "--rrc_procedure_timeout_ms",
         config.rrc_procedure_timeout_ms,
         "Timeout in ms used for RRC message exchange with UE. It needs to suit the expected communication delay and "
         "account for potential retransmissions UE processing delays, SR delays, etc.")
      ->capture_default_str();
}

static void configure_cli11_security_args(CLI::App& app, security_appconfig& config)
{
  auto sec_check = [](const std::string& value) -> std::string {
    if (value == "required" || value == "preferred" || value == "not_needed") {
      return {};
    }
    return "Security indication value not supported. Accepted values [required,preferred,not_needed]";
  };

  app.add_option("--integrity", config.integrity_protection, "Default integrity protection indication for DRBs")
      ->capture_default_str()
      ->check(sec_check);

  app.add_option("--confidentiality",
                 config.confidentiality_protection,
                 "Default confidentiality protection indication for DRBs")
      ->capture_default_str()
      ->check(sec_check);

  app.add_option("--nea_pref_list",
                 config.nea_preference_list,
                 "Ordered preference list for the selection of encryption algorithm (NEA) (default: NEA0, NEA2, NEA1)");

  app.add_option("--nia_pref_list",
                 config.nia_preference_list,
                 "Ordered preference list for the selection of encryption algorithm (NIA) (default: NIA2, NIA1)")
      ->capture_default_str();
}

static void configure_cli11_cu_cp_args(CLI::App& app, cu_cp_appconfig& cu_cp_params)
{
  app.add_option(
      "--max_nof_dus", cu_cp_params.max_nof_dus, "Maximum number of DU connections that the CU-CP may accept");

  app.add_option(
      "--max_nof_cu_ups", cu_cp_params.max_nof_cu_ups, "Maximum number of CU-UP connections that the CU-CP may accept");

  app.add_option("--inactivity_timer", cu_cp_params.inactivity_timer, "UE/PDU Session/DRB inactivity timer in seconds")
      ->capture_default_str()
      ->check(CLI::Range(1, 7200));

  app.add_option("--pdu_session_setup_timeout",
                 cu_cp_params.pdu_session_setup_timeout,
                 "Timeout for the setup of a PDU session after an InitialUeMessage was sent to the core, in "
                 "seconds. The timeout must be larger than T310. If the value is reached, the UE will be released")
      ->capture_default_str();

  CLI::App* mobility_subcmd = app.add_subcommand("mobility", "Mobility configuration");
  configure_cli11_mobility_args(*mobility_subcmd, cu_cp_params.mobility_config);

  CLI::App* rrc_subcmd = app.add_subcommand("rrc", "RRC specific configuration");
  configure_cli11_rrc_args(*rrc_subcmd, cu_cp_params.rrc_config);

  CLI::App* security_subcmd = app.add_subcommand("security", "Security configuration");
  configure_cli11_security_args(*security_subcmd, cu_cp_params.security_config);
}

static void configure_cli11_cu_up_args(CLI::App& app, cu_up_appconfig& cu_up_params)
{
  app.add_option("--gtpu_queue_size", cu_up_params.gtpu_queue_size, "GTP-U queue size, in PDUs")->capture_default_str();
  app.add_option("--gtpu_reordering_timer",
                 cu_up_params.gtpu_reordering_timer_ms,
                 "GTP-U RX reordering timer (in milliseconds)")
      ->capture_default_str();
  app.add_option("--warn_on_drop",
                 cu_up_params.warn_on_drop,
                 "Log a warning for dropped packets in GTP-U and PDCP due to full queues")
      ->capture_default_str();
}

static void configure_cli11_expert_phy_args(CLI::App& app, expert_upper_phy_appconfig& expert_phy_params)
{
  auto pusch_sinr_method_check = [](const std::string& value) -> std::string {
    if ((value == "channel_estimator") || (value == "post_equalization") || (value == "evm")) {
      return {};
    }
    return "Invalid PUSCH SINR calculation method. Accepted values [channel_estimator,post_equalization,evm]";
  };

  app.add_option("--max_proc_delay",
                 expert_phy_params.max_processing_delay_slots,
                 "Maximum allowed DL processing delay in slots.")
      ->capture_default_str()
      ->check(CLI::Range(1, 30));
  app.add_option("--pusch_dec_max_iterations",
                 expert_phy_params.pusch_decoder_max_iterations,
                 "Maximum number of PUSCH LDPC decoder iterations")
      ->capture_default_str()
      ->check(CLI::Number);
  app.add_option("--pusch_dec_enable_early_stop",
                 expert_phy_params.pusch_decoder_early_stop,
                 "Enables PUSCH LDPC decoder early stop")
      ->capture_default_str();
  app.add_option("--pusch_sinr_calc_method",
                 expert_phy_params.pusch_sinr_calc_method,
                 "PUSCH SINR calculation method: channel_estimator, post_equalization and evm.")
      ->capture_default_str()
      ->check(pusch_sinr_method_check);
  app.add_option("--max_request_headroom_slots",
                 expert_phy_params.nof_slots_request_headroom,
                 "Maximum request headroom size in slots.")
      ->capture_default_str()
      ->check(CLI::Range(0, 3));
}

static void configure_cli11_pdcch_common_args(CLI::App& app, pdcch_common_appconfig& common_params)
{
  app.add_option("--coreset0_index", common_params.coreset0_index, "CORESET#0 index")
      ->capture_default_str()
      ->check(CLI::Range(0, 15));

  app.add_option("--ss1_n_candidates",
                 common_params.ss1_n_candidates,
                 "Number of PDCCH candidates per aggregation level for SearchSpace#1. Default: {0, 0, 1, 0, 0}")
      ->capture_default_str()
      ->check(CLI::IsMember({0, 1, 2, 3, 4, 5, 6, 8}));

  app.add_option("--ss0_index", common_params.ss0_index, "SearchSpace#0 index")
      ->capture_default_str()
      ->check(CLI::Range(0, 15));

  // NOTE: The CORESET duration of 3 symbols is only permitted if the dmrs-typeA-Position information element has
  // been set to 3. And, we use only pos2 or pos1.
  app.add_option("--max_coreset0_duration",
                 common_params.max_coreset0_duration,
                 "Maximum CORESET#0 duration in OFDM symbols to consider when deriving CORESET#0 index")
      ->capture_default_str()
      ->check(CLI::Range(1, 2));
}

static void configure_cli11_pdcch_dedicated_args(CLI::App& app, pdcch_dedicated_appconfig& ded_params)
{
  app.add_option("--coreset1_rb_start",
                 ded_params.coreset1_rb_start,
                 "Starting Common Resource Block (CRB) number for CORESET 1 relative to CRB 0. Default: CRB0")
      ->capture_default_str()
      ->check(CLI::Range(0, 275));

  app.add_option("--coreset1_l_crb",
                 ded_params.coreset1_l_crb,
                 "Length of CORESET 1 in number of CRBs. Default: Across entire BW of cell")
      ->capture_default_str()
      ->check(CLI::Range(0, 275));

  // NOTE: The CORESET duration of 3 symbols is only permitted if the dmrs-typeA-Position information element has been
  // set to 3. And, we use only pos2 or pos1.
  app.add_option("--coreset1_duration",
                 ded_params.coreset1_duration,
                 "Duration of CORESET 1 in number of OFDM symbols. Default: Max(2, Nof. CORESET#0 symbols)")
      ->capture_default_str()
      ->check(CLI::Range(1, 2));

  app.add_option("--ss2_n_candidates",
                 ded_params.ss2_n_candidates,
                 "Number of PDCCH candidates per aggregation level for SearchSpace#2. Default: {0, 0, 0, 0, 0} i.e. "
                 "auto-compute nof. candidates")
      ->capture_default_str()
      ->check(CLI::IsMember({0, 1, 2, 3, 4, 5, 6, 8}));

  app.add_option("--dci_format_0_1_and_1_1",
                 ded_params.dci_format_0_1_and_1_1,
                 "DCI format to use in UE dedicated SearchSpace#2")
      ->capture_default_str();
  app.add_option_function<std::string>(
         "--ss2_type",
         [&ded_params](const std::string& value) {
           ded_params.ss2_type = (value == "common") ? search_space_configuration::type_t::common
                                                     : search_space_configuration::type_t::ue_dedicated;
         },
         "SearchSpace type for UE dedicated SearchSpace#2")
      ->default_str("ue_dedicated")
      ->check(CLI::IsMember({"common", "ue_dedicated"}, CLI::ignore_case));
}

static void configure_cli11_pdcch_args(CLI::App& app, pdcch_appconfig& pdcch_params)
{
  // PDCCH Common configuration.
  CLI::App* pdcch_common_subcmd = app.add_subcommand("common", "PDCCH Common configuration parameters");
  configure_cli11_pdcch_common_args(*pdcch_common_subcmd, pdcch_params.common);

  // PDCCH Dedicated configuration.
  CLI::App* pdcch_dedicated_subcmd = app.add_subcommand("dedicated", "PDCCH Dedicated configuration parameters");
  configure_cli11_pdcch_dedicated_args(*pdcch_dedicated_subcmd, pdcch_params.dedicated);
}

static void configure_cli11_pdsch_args(CLI::App& app, pdsch_appconfig& pdsch_params)
{
  app.add_option("--min_ue_mcs", pdsch_params.min_ue_mcs, "Minimum UE MCS")
      ->capture_default_str()
      ->check(CLI::Range(0, 28));
  app.add_option("--max_ue_mcs", pdsch_params.max_ue_mcs, "Maximum UE MCS")
      ->capture_default_str()
      ->check(CLI::Range(0, 28));
  app.add_option("--fixed_rar_mcs", pdsch_params.fixed_rar_mcs, "Fixed RAR MCS")
      ->capture_default_str()
      ->check(CLI::Range(0, 28));
  app.add_option("--fixed_sib1_mcs", pdsch_params.fixed_sib1_mcs, "Fixed SIB1 MCS")
      ->capture_default_str()
      ->check(CLI::Range(0, 28));
  app.add_option("--nof_harqs", pdsch_params.nof_harqs, "Number of DL HARQ processes")
      ->capture_default_str()
      ->check(CLI::IsMember({2, 4, 6, 8, 10, 12, 16}));
  app.add_option("--max_nof_harq_retxs",
                 pdsch_params.max_nof_harq_retxs,
                 "Maximum number of times a DL HARQ can be retransmitted, before it gets discarded")
      ->capture_default_str()
      ->check(CLI::Range(0, 4));
  app.add_option("--max_consecutive_kos",
                 pdsch_params.max_consecutive_kos,
                 "Maximum number of HARQ-ACK consecutive KOs before an Radio Link Failure is reported")
      ->capture_default_str();
  app.add_option("--rv_sequence", pdsch_params.rv_sequence, "RV sequence for PUSCH. (e.g. [0 2 3 1]")
      ->capture_default_str()
      ->check(CLI::IsMember({0, 1, 2, 3}));
  app.add_option_function<std::string>(
         "--mcs_table",
         [&pdsch_params](const std::string& value) {
           if (value == "qam256") {
             pdsch_params.mcs_table = pdsch_mcs_table::qam256;
           }
         },
         "MCS table to use PDSCH")
      ->default_str("qam64")
      ->check(CLI::IsMember({"qam64", "qam256"}, CLI::ignore_case));
  app.add_option("--min_rb_size", pdsch_params.min_rb_size, "Minimum RB size for UE PDSCH resource allocation")
      ->capture_default_str()
      ->check(CLI::Range(1U, (unsigned)MAX_NOF_PRBS));
  app.add_option("--max_rb_size", pdsch_params.max_rb_size, "Maximum RB size for UE PDSCH resource allocation")
      ->capture_default_str()
      ->check(CLI::Range(1U, (unsigned)MAX_NOF_PRBS));
  app.add_option("--max_pdschs_per_slot",
                 pdsch_params.max_pdschs_per_slot,
                 "Maximum number of PDSCH grants per slot, including SIB, RAR, Paging and UE data grants.")
      ->capture_default_str()
      ->check(CLI::Range(1U, (unsigned)MAX_PDSCH_PDUS_PER_SLOT));
  app.add_option("--max_alloc_attempts",
                 pdsch_params.max_pdcch_alloc_attempts_per_slot,
                 "Maximum number of DL or UL PDCCH grant allocation attempts per slot before scheduler skips the slot")
      ->capture_default_str()
      ->check(CLI::Range(1U, (unsigned)std::max(MAX_DL_PDCCH_PDUS_PER_SLOT, MAX_UL_PDCCH_PDUS_PER_SLOT)));
  app.add_option("--olla_cqi_inc_step",
                 pdsch_params.olla_cqi_inc,
                 "Outer-loop link adaptation (OLLA) increment value. The value 0 means that OLLA is disabled")
      ->capture_default_str()
      ->check(CLI::Range(0.0, 1.0));
  app.add_option("--olla_target_bler",
                 pdsch_params.olla_target_bler,
                 "Target DL BLER set in Outer-loop link adaptation (OLLA) algorithm")
      ->capture_default_str()
      ->check(CLI::Range(0.0, 0.5));
  app.add_option("--olla_max_cqi_offset",
                 pdsch_params.olla_max_cqi_offset,
                 "Maximum offset that the Outer-loop link adaptation (OLLA) can apply to CQI")
      ->capture_default_str()
      ->check(CLI::PositiveNumber);
  app.add_option_function<std::string>(
         "--dc_offset",
         [&pdsch_params](const std::string& value) {
           if (value == "undetermined") {
             pdsch_params.dc_offset = dc_offset_t::undetermined;
           } else if (value == "outside") {
             pdsch_params.dc_offset = dc_offset_t::outside;
           } else if (value == "center") {
             pdsch_params.dc_offset = dc_offset_t::center;
           } else {
             pdsch_params.dc_offset = static_cast<dc_offset_t>(parse_int<int>(value).value());
           }
         },
         "Direct Current (DC) Offset in number of subcarriers, using the common SCS as reference for carrier spacing, "
         "and the center of the gNB DL carrier as DC offset value 0. The user can additionally set \"outside\" to "
         "define that the DC offset falls outside the DL carrier or \"undetermined\" in the case the DC offset is "
         "unknown.")
      ->capture_default_str()
      ->check(CLI::Range(static_cast<int>(dc_offset_t::min), static_cast<int>(dc_offset_t::max)) |
              CLI::IsMember({"outside", "undetermined", "center"}));
  app.add_option("--harq_la_cqi_drop_threshold",
                 pdsch_params.harq_la_cqi_drop_threshold,
                 "Link Adaptation (LA) threshold for drop in CQI of the first HARQ transmission above which HARQ "
                 "retransmissions are cancelled. Set this value to 0 to disable this feature")
      ->capture_default_str()
      ->check(CLI::Range(0, 15));
  app.add_option("--harq_la_ri_drop_threshold",
                 pdsch_params.harq_la_ri_drop_threshold,
                 "Link Adaptation (LA) threshold for drop in nof. layers of the first HARQ transmission above which "
                 "HARQ retransmission is cancelled. Set this value to 0 to disable this feature")
      ->capture_default_str()
      ->check(CLI::Range(0, 4));
  app.add_option("--dmrs_additional_position", pdsch_params.dmrs_add_pos, "PDSCH DMRS additional position")
      ->capture_default_str()
      ->check(CLI::Range(0, 3));
}

static void configure_cli11_pusch_args(CLI::App& app, pusch_appconfig& pusch_params)
{
  app.add_option("--min_ue_mcs", pusch_params.min_ue_mcs, "Minimum UE MCS")
      ->capture_default_str()
      ->check(CLI::Range(0, 28));
  app.add_option("--max_ue_mcs", pusch_params.max_ue_mcs, "Maximum UE MCS")
      ->capture_default_str()
      ->check(CLI::Range(0, 28));
  app.add_option("--max_consecutive_kos",
                 pusch_params.max_consecutive_kos,
                 "Maximum number of CRC consecutive KOs before an Radio Link Failure is reported")
      ->capture_default_str();
  app.add_option("--rv_sequence", pusch_params.rv_sequence, "RV sequence for PUSCH. (e.g. [0 2 3 1]")
      ->capture_default_str()
      ->check(CLI::IsMember({0, 1, 2, 3}));
  app.add_option_function<std::string>(
         "--mcs_table",
         [&pusch_params](const std::string& value) {
           if (value == "qam256") {
             pusch_params.mcs_table = pusch_mcs_table::qam256;
           }
         },
         "MCS table to use PUSCH")
      ->default_str("qam64")
      ->check(CLI::IsMember({"qam64", "qam256"}, CLI::ignore_case));
  app.add_option("--msg3_delta_preamble",
                 pusch_params.msg3_delta_preamble,
                 "msg3-DeltaPreamble, Power offset between msg3 and RACH preamble transmission")
      ->capture_default_str()
      ->check(CLI::Range(-1, 6));
  app.add_option("--p0_nominal_with_grant",
                 pusch_params.p0_nominal_with_grant,
                 "P0 value for PUSCH with grant (except msg3). Value in dBm. Valid values must be multiple of 2 and "
                 "within the [-202, 24] interval.  Default: -76")
      ->capture_default_str()
      ->check([](const std::string& value) -> std::string {
        std::stringstream ss(value);
        int               pw;
        ss >> pw;
        const std::string& error_message = "Must be a multiple of 2 and within the [-202, 24] interval";
        if (pw < -202 or pw > 24 or pw % 2 != 0) {
          return error_message;
        }

        return "";
      });
  app.add_option("--msg3_delta_power",
                 pusch_params.msg3_delta_power,
                 "Target power level at the network receiver side, in dBm. Valid values must be multiple of 2 and "
                 "within the [-6, 8] interval. Default: 8")
      ->capture_default_str()
      ->check([](const std::string& value) -> std::string {
        std::stringstream ss(value);
        int               pw;
        ss >> pw;
        const std::string& error_message = "Must be a multiple of 2 and within the [-6, 8] interval";
        if (pw < -6 or pw > 8 or pw % 2 != 0) {
          return error_message;
        }

        return "";
      });
  app.add_option("--max_puschs_per_slot", pusch_params.max_puschs_per_slot, "Maximum number of PUSCH grants per slot")
      ->capture_default_str()
      ->check(CLI::Range(1U, (unsigned)MAX_PUSCH_PDUS_PER_SLOT));
  app.add_option("--b_offset_ack_idx_1", pusch_params.b_offset_ack_idx_1, "betaOffsetACK-Index1 part of UCI-OnPUSCH")
      ->capture_default_str()
      ->check(CLI::Range(0, 31));
  app.add_option("--b_offset_ack_idx_2", pusch_params.b_offset_ack_idx_2, "betaOffsetACK-Index2 part of UCI-OnPUSCH")
      ->capture_default_str()
      ->check(CLI::Range(0, 31));
  app.add_option("--b_offset_ack_idx_3", pusch_params.b_offset_ack_idx_3, "betaOffsetACK-Index3 part of UCI-OnPUSCH")
      ->capture_default_str()
      ->check(CLI::Range(0, 31));
  app.add_option(
         "--beta_offset_csi_p1_idx_1", pusch_params.b_offset_csi_p1_idx_1, "b_offset_csi_p1_idx_1 part of UCI-OnPUSCH")
      ->capture_default_str()
      ->check(CLI::Range(0, 31));
  app.add_option(
         "--beta_offset_csi_p1_idx_2", pusch_params.b_offset_csi_p1_idx_2, "b_offset_csi_p1_idx_2 part of UCI-OnPUSCH")
      ->capture_default_str()
      ->check(CLI::Range(0, 31));
  app.add_option(
         "--beta_offset_csi_p2_idx_1", pusch_params.b_offset_csi_p2_idx_1, "b_offset_csi_p2_idx_1 part of UCI-OnPUSCH")
      ->capture_default_str()
      ->check(CLI::Range(0, 31));
  app.add_option(
         "--beta_offset_csi_p2_idx_2", pusch_params.b_offset_csi_p2_idx_2, "b_offset_csi_p2_idx_2 part of UCI-OnPUSCH")
      ->capture_default_str()
      ->check(CLI::Range(0, 31));
  app.add_option("--min_k2", pusch_params.min_k2, "Minimum value of K2 (difference in slots between PDCCH and PUSCH).")
      ->capture_default_str()
      ->check(CLI::Range(1, 4));
  app.add_option_function<std::string>(
         "--dc_offset",
         [&pusch_params](const std::string& value) {
           if (value == "undetermined") {
             pusch_params.dc_offset = dc_offset_t::undetermined;
           } else if (value == "outside") {
             pusch_params.dc_offset = dc_offset_t::outside;
           } else {
             pusch_params.dc_offset = static_cast<dc_offset_t>(parse_int<int>(value).value());
           }
         },
         "Direct Current (DC) Offset in number of subcarriers, using the common SCS as reference for carrier spacing, "
         "and the center of the gNB UL carrier as DC offset value 0. The user can additionally set \"outside\" to "
         "define that the DC offset falls outside the UL carrier or \"undetermined\" in the case the DC offset is "
         "unknown.")
      ->capture_default_str()
      ->check(CLI::Range(static_cast<int>(dc_offset_t::min), static_cast<int>(dc_offset_t::max)) |
              CLI::IsMember({"outside", "undetermined"}));
  app.add_option("--olla_snr_inc_step",
                 pusch_params.olla_snr_inc,
                 "Outer-loop link adaptation (OLLA) increment value. The value 0 means that OLLA is disabled")
      ->capture_default_str()
      ->check(CLI::Range(0.0, 1.0));
  app.add_option("--olla_target_bler",
                 pusch_params.olla_target_bler,
                 "Target UL BLER set in Outer-loop link adaptation (OLLA) algorithm")
      ->capture_default_str()
      ->check(CLI::Range(0.0, 0.5));
  app.add_option("--olla_max_snr_offset",
                 pusch_params.olla_max_snr_offset,
                 "Maximum offset that the Outer-loop link adaptation (OLLA) can apply to the estimated UL SINR")
      ->capture_default_str()
      ->check(CLI::PositiveNumber);
  app.add_option("--dmrs_additional_position", pusch_params.dmrs_add_pos, "PUSCH DMRS additional position")
      ->capture_default_str()
      ->check(CLI::Range(0, 3));
}

static void configure_cli11_pucch_args(CLI::App& app, pucch_appconfig& pucch_params)
{
  app.add_option(
         "--p0_nominal",
         pucch_params.p0_nominal,
         "Power control parameter P0 for PUCCH transmissions. Value in dBm. Valid values must be multiple of 2 and "
         "within the [-202, 24] interval. Default: -90")
      ->capture_default_str()
      ->check([](const std::string& value) -> std::string {
        std::stringstream ss(value);
        int               pw;
        ss >> pw;
        const std::string& error_message = "Must be a multiple of 2 and within the [-202, 24] interval";
        if (pw < -202 or pw > 24 or pw % 2 != 0) {
          return error_message;
        }

        return "";
      });
  app.add_option("--sr_period_ms", pucch_params.sr_period_msec, "SR period in msec")
      ->capture_default_str()
      ->check(CLI::IsMember({1, 2, 4, 8, 10, 16, 20, 40, 80, 160, 320}));
  app.add_option("--f1_nof_ue_res_harq",
                 pucch_params.nof_ue_pucch_f1_res_harq,
                 "Number of PUCCH F1 resources available per UE for HARQ")
      ->capture_default_str()
      ->check(CLI::Range(1, 8));
  app.add_option("--f1_nof_cell_res_sr",
                 pucch_params.nof_cell_sr_resources,
                 "Number of PUCCH F1 resources available per cell for SR")
      ->capture_default_str()
      ->check(CLI::Range(1, 50));
  app.add_option("--f1_nof_symbols", pucch_params.f1_nof_symbols, "Number of symbols for PUCCH F1 resources")
      ->capture_default_str()
      ->check(CLI::Range(4, 14));
  app.add_option("--f1_enable_occ", pucch_params.f1_enable_occ, "Enable OCC for PUCCH F1")->capture_default_str();
  app.add_option("--f1_nof_cyclic_shifts",
                 pucch_params.nof_cyclic_shift,
                 "Number of possible cyclic shifts available for PUCCH F1 resources")
      ->capture_default_str()
      ->check(CLI::IsMember({1, 2, 3, 4, 6, 12}));
  app.add_option("--f1_intraslot_freq_hop",
                 pucch_params.f1_intraslot_freq_hopping,
                 "Enable intra-slot frequency hopping for PUCCH F1")
      ->capture_default_str();
  app.add_option("--nof_cell_harq_pucch_res_sets",
                 pucch_params.nof_cell_harq_pucch_sets,
                 "Number of separate PUCCH resource sets for HARQ-ACK that are available in the cell. NOTE: the "
                 "higher the number of sets, the lower the chances UEs have to share the same PUCCH resources.")
      ->capture_default_str()
      ->check(CLI::Range(1, 10));
  app.add_option("--f2_nof_ue_res_harq",
                 pucch_params.nof_ue_pucch_f2_res_harq,
                 "Number of PUCCH F2 resources available per UE for HARQ")
      ->capture_default_str()
      ->check(CLI::Range(1, 8));
  app.add_option("--f2_nof_cell_res_csi",
                 pucch_params.nof_cell_csi_resources,
                 "Number of PUCCH F2 resources available per cell for CSI")
      ->capture_default_str()
      ->check(CLI::Range(0, 50));
  app.add_option("--f2_nof_symbols", pucch_params.f2_nof_symbols, "Number of symbols for PUCCH F2 resources")
      ->capture_default_str()
      ->check(CLI::Range(1, 2));
  app.add_option("--f2_max_nof_rbs", pucch_params.f2_max_nof_rbs, "Max number of RBs for PUCCH F2 resources")
      ->capture_default_str()
      ->check(CLI::Range(1, 16));
  app.add_option("--f2_max_payload", pucch_params.max_payload_bits, "Max number payload bits for PUCCH F2 resources")
      ->check(CLI::Range(1, 11));
  app.add_option_function<std::string>(
         "--f2_max_code_rate",
         [&pucch_params](const std::string& value) {
           if (value == "dot08") {
             pucch_params.max_code_rate = max_pucch_code_rate::dot_08;
           } else if (value == "dot15") {
             pucch_params.max_code_rate = max_pucch_code_rate::dot_15;
           } else if (value == "dot25") {
             pucch_params.max_code_rate = max_pucch_code_rate::dot_25;
           } else if (value == "dot35") {
             pucch_params.max_code_rate = max_pucch_code_rate::dot_35;
           } else if (value == "dot45") {
             pucch_params.max_code_rate = max_pucch_code_rate::dot_45;
           } else if (value == "dot60") {
             pucch_params.max_code_rate = max_pucch_code_rate::dot_60;
           } else if (value == "dot80") {
             pucch_params.max_code_rate = max_pucch_code_rate::dot_80;
           }
         },
         "PUCCH F2 max code rate {dot08, dot15, dot25, dot35, dot45, dot60, dot80}. Default: dot35")
      ->check(CLI::IsMember({"dot08", "dot15", "dot25", "dot35", "dot45", "dot60", "dot80"}, CLI::ignore_case));
  app.add_option("--f2_intraslot_freq_hop",
                 pucch_params.f2_intraslot_freq_hopping,
                 "Enable intra-slot frequency hopping for PUCCH F2")
      ->capture_default_str();
  app.add_option("--min_k1",
                 pucch_params.min_k1,
                 "Minimum value of K1 (difference in slots between PDSCH and HARQ-ACK). Lower k1 values will reduce "
                 "latency, but place a stricter requirement on the UE decode latency.")
      ->capture_default_str()
      ->check(CLI::Range(1, 4));

  app.add_option("--max_consecutive_kos",
                 pucch_params.max_consecutive_kos,
                 "Maximum number of consecutive undecoded PUCCH F2 for CSI before an Radio Link Failure is reported")
      ->capture_default_str();
}

static void configure_cli11_ul_common_args(CLI::App& app, ul_common_appconfig& ul_common_params)
{
  app.add_option("--p_max", ul_common_params.p_max, "Maximum transmit power allowed in this serving cell")
      ->capture_default_str()
      ->check(CLI::Range(-30, 23));
  app.add_option("--max_pucchs_per_slot",
                 ul_common_params.max_pucchs_per_slot,
                 "Maximum number of PUCCH grants that can be allocated per slot")
      ->capture_default_str()
      ->check(CLI::Range(1U, static_cast<unsigned>(MAX_PUCCH_PDUS_PER_SLOT)));
  app.add_option("--max_ul_grants_per_slot",
                 ul_common_params.max_ul_grants_per_slot,
                 "Maximum number of UL grants that can be allocated per slot")
      ->capture_default_str()
      ->check(CLI::Range(1U, (unsigned)(MAX_PUSCH_PDUS_PER_SLOT + MAX_PUCCH_PDUS_PER_SLOT)));
}

static void configure_cli11_ssb_args(CLI::App& app, ssb_appconfig& ssb_params)
{
  app.add_option("--ssb_period", ssb_params.ssb_period_msec, "Period of SSB scheduling in milliseconds")
      ->capture_default_str()
      ->check(CLI::IsMember({5, 10, 20}));
  app.add_option("--ssb_block_power_dbm", ssb_params.ssb_block_power, "SS_PBCH_power_block in dBm")
      ->capture_default_str()
      ->check(CLI::Range(-60, 50));
  app.add_option_function<unsigned>(
         "--pss_to_sss_epre_db",
         [&ssb_params](unsigned value) {
           if (value == 0) {
             ssb_params.pss_to_sss_epre = srsran::ssb_pss_to_sss_epre::dB_0;
           } else if (value == 3) {
             ssb_params.pss_to_sss_epre = srsran::ssb_pss_to_sss_epre::dB_3;
           }
         },
         "SSB PSS to SSS EPRE ratio in dB {0, 3}")
      ->check(CLI::IsMember({0, 3}));
}

static void configure_cli11_si_sched_info(CLI::App& app, sib_appconfig::si_sched_info_config& si_sched_info)
{
  app.add_option("--si_period", si_sched_info.si_period_rf, "SI message scheduling period in radio frames")
      ->capture_default_str()
      ->check(CLI::IsMember({8, 16, 32, 64, 128, 256, 512}));
  app.add_option("--sib_mapping",
                 si_sched_info.sib_mapping_info,
                 "Mapping of SIB types to SI-messages. SIB numbers should not be repeated")
      ->capture_default_str()
      ->check(CLI::IsMember({2, 19}));
}

static void configure_cli11_epoch_time(CLI::App& app, epoch_time_t& epoch_time)
{
  app.add_option("--sfn", epoch_time.sfn, "SFN Part")->capture_default_str()->check(CLI::Range(0, 1023));
  app.add_option("--subframe_number", epoch_time.subframe_number, "Sub-frame number Part")
      ->capture_default_str()
      ->check(CLI::Range(0, 9));
}

static void configure_cli11_ephemeris_info_ecef(CLI::App& app, ecef_coordinates_t& ephemeris_info)
{
  app.add_option("--pos_x", ephemeris_info.position_x, "X Position of the satellite")
      ->capture_default_str()
      ->check(CLI::Range(-67108864, 67108863));
  app.add_option("--pos_y", ephemeris_info.position_y, "Y Position of the satellite")
      ->capture_default_str()
      ->check(CLI::Range(-67108864, 67108863));
  app.add_option("--pos_z", ephemeris_info.position_z, "Z Position of the satellite")
      ->capture_default_str()
      ->check(CLI::Range(-67108864, 67108863));
  app.add_option("--vel_x", ephemeris_info.velocity_vx, "X Velocity of the satellite")
      ->capture_default_str()
      ->check(CLI::Range(-32768, 32767));
  app.add_option("--vel_y", ephemeris_info.velocity_vy, "Y Velocity of the satellite")
      ->capture_default_str()
      ->check(CLI::Range(-32768, 32767));
  app.add_option("--vel_z", ephemeris_info.velocity_vz, "Z Velocity of the satellite")
      ->capture_default_str()
      ->check(CLI::Range(-32768, 32767));
}

static void configure_cli11_ephemeris_info_orbital(CLI::App& app, orbital_coordinates_t& ephemeris_info)
{
  app.add_option("--semi_major_axis", ephemeris_info.semi_major_axis, "Semi-major axis of the satellite")
      ->capture_default_str()
      ->check(CLI::Range(0, 1000000000));
  app.add_option("--eccentricity", ephemeris_info.eccentricity, "Eccentricity of the satellite")->capture_default_str();
  app.add_option("--periapsis", ephemeris_info.periapsis, "Periapsis of the satellite")->capture_default_str();
  app.add_option("--longitude", ephemeris_info.longitude, "Longitude of the satellites angle of ascending node")
      ->capture_default_str();
  app.add_option("--inclination", ephemeris_info.inclination, "Inclination of the satellite")->capture_default_str();
  app.add_option("--mean_anomaly", ephemeris_info.mean_anomaly, "Mean anomaly of the satellite")->capture_default_str();
}
static void configure_cli11_ntn_args(CLI::App&              app,
                                     optional<ntn_config>&  ntn,
                                     epoch_time_t&          epoch_time,
                                     orbital_coordinates_t& orbital_coordinates,
                                     ecef_coordinates_t&    ecef_coordinates)
{
  ntn.emplace();
  app.add_option("--cell_specific_koffset", ntn->cell_specific_koffset, "Cell-specific k-offset to be used for NTN.")
      ->capture_default_str()
      ->check(CLI::Range(0, 1023));

  ntn.value().ta_info.emplace();
  app.add_option("--ta_common", ntn->ta_info->ta_common, "TA common offset");

  // epoch time.
  CLI::App* epoch_time_subcmd = app.add_subcommand("epoch_time", "Epoch time for the NTN assistance information");
  configure_cli11_epoch_time(*epoch_time_subcmd, epoch_time);

  // ephemeris configuration.
  CLI::App* ephem_subcmd_ecef =
      app.add_subcommand("ephemeris_info_ecef", "ephermeris information of the satellite in ecef coordinates");
  configure_cli11_ephemeris_info_ecef(*ephem_subcmd_ecef, ecef_coordinates);

  CLI::App* ephem_subcmd_orbital =
      app.add_subcommand("ephemeris_orbital", "ephermeris information of the satellite in orbital coordinates");
  configure_cli11_ephemeris_info_orbital(*ephem_subcmd_orbital, orbital_coordinates);
}

static void configure_cli11_sib_args(CLI::App& app, sib_appconfig& sib_params)
{
  app.add_option(
         "--si_window_length",
         sib_params.si_window_len_slots,
         "The length of the SI scheduling window, in slots. It must be always shorter or equal to the period of "
         "the SI message.")
      ->capture_default_str()
      ->check(CLI::IsMember({5, 10, 20, 40, 80, 160, 320, 640, 1280}));

  // SI message scheduling parameters.
  app.add_option_function<std::vector<std::string>>(
      "--si_sched_info",
      [&sib_params](const std::vector<std::string>& values) {
        sib_params.si_sched_info.resize(values.size());

        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("SI-message scheduling information");
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras(CLI::config_extras_mode::error);
          configure_cli11_si_sched_info(subapp, sib_params.si_sched_info[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Configures the scheduling for each of the SI-messages broadcast by the gNB");

  app.add_option("--t300",
                 sib_params.ue_timers_and_constants.t300,
                 "RRC Connection Establishment timer in ms. The timer starts upon transmission of RRCSetupRequest.")
      ->capture_default_str()
      ->check(CLI::IsMember({100, 200, 300, 400, 600, 1000, 1500, 2000}));
  app.add_option("--t301",
                 sib_params.ue_timers_and_constants.t301,
                 "RRC Connection Re-establishment timer in ms. The timer starts upon transmission of "
                 "RRCReestablishmentRequest.")
      ->capture_default_str()
      ->check(CLI::IsMember({100, 200, 300, 400, 600, 1000, 1500, 2000}));
  app.add_option("--t310",
                 sib_params.ue_timers_and_constants.t310,
                 "Out-of-sync timer in ms. The timer starts upon detecting physical layer problems for the SpCell i.e. "
                 "upon receiving N310 consecutive out-of-sync indications from lower layers.")
      ->capture_default_str()
      ->check(CLI::IsMember({0, 50, 100, 200, 500, 1000, 2000}));
  app.add_option("--n310",
                 sib_params.ue_timers_and_constants.n310,
                 "Out-of-sync counter. The counter is increased upon reception of \"out-of-sync\" from lower layer "
                 "while the timer T310 is stopped. Starts the timer T310, when configured value is reached.")
      ->capture_default_str()
      ->check(CLI::IsMember({1, 2, 3, 4, 6, 8, 10, 20}));
  app.add_option("--t311",
                 sib_params.ue_timers_and_constants.t311,
                 "RRC Connection Re-establishment procedure timer in ms. The timer starts upon initiating the RRC "
                 "connection re-establishment procedure.")
      ->capture_default_str()
      ->check(CLI::IsMember({1000, 3000, 5000, 10000, 15000, 20000, 30000}));
  app.add_option("--n311",
                 sib_params.ue_timers_and_constants.n311,
                 "In-sync counter. The counter is increased upon reception of the \"in-sync\" from lower layer while "
                 "the timer T310 is running. Stops the timer T310, when configured value is reached.")
      ->capture_default_str()
      ->check(CLI::IsMember({1, 2, 3, 4, 5, 6, 8, 10}));
  app.add_option("--t319",
                 sib_params.ue_timers_and_constants.t319,
                 "RRC Connection Resume timer in ms. The timer starts upon transmission of RRCResumeRequest "
                 "or RRCResumeRequest1.")
      ->capture_default_str()
      ->check(CLI::IsMember({100, 200, 300, 400, 600, 1000, 1500, 2000}));
}

static void configure_cli11_prach_args(CLI::App& app, prach_appconfig& prach_params)
{
  app.add_option("--prach_config_index",
                 prach_params.prach_config_index,
                 "PRACH configuration index. If not set, the value is derived, so that the PRACH fits in an UL slot")
      ->capture_default_str()
      ->check(CLI::Range(0, 255));
  app.add_option("--prach_root_sequence_index", prach_params.prach_root_sequence_index, "PRACH root sequence index")
      ->capture_default_str()
      ->check(CLI::Range(0, 837));
  app.add_option("--zero_correlation_zone", prach_params.zero_correlation_zone, "Zero correlation zone index")
      ->capture_default_str()
      ->check(CLI::Range(0, 15));
  app.add_option("--fixed_msg3_mcs", prach_params.fixed_msg3_mcs, "Fixed message 3 MCS")
      ->capture_default_str()
      ->check(CLI::Range(0, 28));
  app.add_option(
         "--max_msg3_harq_retx", prach_params.max_msg3_harq_retx, "Maximum number of message 3 HARQ retransmissions")
      ->capture_default_str()
      ->check(CLI::Range(0, 4));
  app.add_option("--total_nof_ra_preambles", prach_params.total_nof_ra_preambles, "Number of different PRACH preambles")
      ->check(CLI::Range(1, 64));
  app.add_option(
         "--prach_frequency_start", prach_params.prach_frequency_start, "PRACH message frequency offset in PRBs")
      ->capture_default_str()
      ->check(CLI::Range(0, 274));
  app.add_option("--preamble_rx_target_pw",
                 prach_params.preamble_rx_target_pw,
                 "Target power level at the network receiver side, in dBm")
      ->capture_default_str()
      ->check([](const std::string& value) -> std::string {
        std::stringstream ss(value);
        int               pw;
        ss >> pw;
        const std::string& error_message = "Must be a multiple of 2 and within the [-202, -60] interval";
        // Bandwidth cannot be less than 5MHz.
        if (pw < -202 or pw > -60 or pw % 2 != 0) {
          return error_message;
        }

        return "";
      });
  app.add_option("--preamble_trans_max",
                 prach_params.preamble_trans_max,
                 "Max number of RA preamble transmissions performed before declaring a failure")
      ->capture_default_str()
      ->check(CLI::IsMember({3, 4, 5, 6, 7, 8, 10, 20, 50, 100, 200}));
  app.add_option("--power_ramping_step_db", prach_params.power_ramping_step_db, "Power ramping steps for PRACH")
      ->capture_default_str()
      ->check(CLI::IsMember({0, 2, 4, 6}));
  app.add_option("--ports", prach_params.ports, "List of antenna ports")->capture_default_str();
  app.add_option("--nof_ssb_per_ro", prach_params.nof_ssb_per_ro, "Number of SSBs per RACH occasion")
      ->check(CLI::IsMember({1}));
  app.add_option("--nof_cb_preambles_per_ssb",
                 prach_params.nof_cb_preambles_per_ssb,
                 "Number of Contention Based preambles per SSB")
      ->check(CLI::Range(1, 64));
}

static void configure_cli11_amplitude_control_args(CLI::App& app, amplitude_control_appconfig& amplitude_params)
{
  app.add_option("--tx_gain_backoff",
                 amplitude_params.gain_backoff_dB,
                 "Gain back-off to accommodate the signal PAPR in decibels")
      ->capture_default_str();
  app.add_option("--enable_clipping", amplitude_params.enable_clipping, "Signal clipping")->capture_default_str();
  app.add_option("--ceiling", amplitude_params.power_ceiling_dBFS, "Clipping ceiling referenced to full scale")
      ->capture_default_str();
}

static void configure_cli11_tdd_ul_dl_pattern_args(CLI::App& app, tdd_ul_dl_pattern_appconfig& pattern_params)
{
  app.add_option("--dl_ul_tx_period",
                 pattern_params.dl_ul_period_slots,
                 "TDD pattern periodicity in slots. The combination of this value and the chosen numerology must lead"
                 " to a TDD periodicity of 0.5, 0.625, 1, 1.25, 2, 2.5, 3, 4, 5 or 10 milliseconds.")
      ->capture_default_str()
      ->check(CLI::Range(2, 80));
  app.add_option("--nof_dl_slots", pattern_params.nof_dl_slots, "TDD pattern nof. consecutive full DL slots")
      ->capture_default_str()
      ->check(CLI::Range(0, 80));
  app.add_option("--nof_dl_symbols",
                 pattern_params.nof_dl_symbols,
                 "TDD pattern nof. DL symbols at the beginning of the slot following full DL slots")
      ->capture_default_str()
      ->check(CLI::Range(0, 13));
  app.add_option("--nof_ul_slots", pattern_params.nof_ul_slots, "TDD pattern nof. consecutive full UL slots")
      ->capture_default_str()
      ->check(CLI::Range(0, 80));
  app.add_option("--nof_ul_symbols",
                 pattern_params.nof_ul_symbols,
                 "TDD pattern nof. UL symbols at the end of the slot preceding the first full UL slot")
      ->capture_default_str()
      ->check(CLI::Range(0, 13));
}

static void configure_cli11_tdd_ul_dl_args(CLI::App& app, tdd_ul_dl_appconfig& tdd_ul_dl_params)
{
  configure_cli11_tdd_ul_dl_pattern_args(app, tdd_ul_dl_params.pattern1);
  // TDD pattern 2.
  // NOTE: CLI11 needs that the life of the variable last longer than the call of callback function. Therefore, the
  // pattern2_cfg variable needs to be static.
  static tdd_ul_dl_pattern_appconfig pattern2_app_cfg;
  CLI::App*                          pattern2_sub_cmd =
      app.add_subcommand("pattern2", "TDD UL DL pattern2 configuration parameters")->configurable();
  configure_cli11_tdd_ul_dl_pattern_args(*pattern2_sub_cmd, pattern2_app_cfg);
  auto tdd_pattern2_verify_callback = [&]() {
    CLI::App* sub_cmd = app.get_subcommand("pattern2");
    if (sub_cmd->count() != 0) {
      tdd_ul_dl_params.pattern2.emplace(pattern2_app_cfg);
    }
  };
  pattern2_sub_cmd->parse_complete_callback(tdd_pattern2_verify_callback);
}

static void configure_cli11_paging_args(CLI::App& app, paging_appconfig& pg_params)
{
  app.add_option("--pg_search_space_id", pg_params.paging_search_space_id, "SearchSpace to use for Paging")
      ->capture_default_str()
      ->check(CLI::IsMember({0, 1}));
  app.add_option(
         "--default_pg_cycle_in_rf", pg_params.default_paging_cycle, "Default Paging cycle in nof. Radio Frames")
      ->capture_default_str()
      ->check(CLI::IsMember({32, 64, 128, 256}));
  app.add_option_function<std::string>(
         "--nof_pf_per_paging_cycle",
         [&pg_params](const std::string& value) {
           if (value == "oneT") {
             pg_params.nof_pf = pcch_config::nof_pf_per_drx_cycle::oneT;
           } else if (value == "halfT") {
             pg_params.nof_pf = pcch_config::nof_pf_per_drx_cycle::halfT;
           } else if (value == "quarterT") {
             pg_params.nof_pf = pcch_config::nof_pf_per_drx_cycle::quarterT;
           } else if (value == "oneEighthT") {
             pg_params.nof_pf = pcch_config::nof_pf_per_drx_cycle::oneEighthT;
           } else if (value == "oneSixteethT") {
             pg_params.nof_pf = pcch_config::nof_pf_per_drx_cycle::oneSixteethT;
           }
         },
         "Number of paging frames per DRX cycle {oneT, halfT, quarterT, oneEighthT, oneSixteethT}. Default: oneT")
      ->check(CLI::IsMember({"oneT", "halfT", "quarterT", "oneEighthT", "oneSixteethT"}, CLI::ignore_case));
  app.add_option("--pf_offset", pg_params.pf_offset, "Paging frame offset")->capture_default_str();
  app.add_option("--nof_po_per_pf", pg_params.nof_po_per_pf, "Number of paging occasions per paging frame")
      ->capture_default_str()
      ->check(CLI::IsMember({1, 2, 4}));
}

static void configure_cli11_csi_args(CLI::App& app, csi_appconfig& csi_params)
{
  app.add_option("--csi_rs_enabled", csi_params.csi_rs_enabled, "Enable CSI-RS resources and CSI reporting")
      ->capture_default_str();
  app.add_option("--csi_rs_period", csi_params.csi_rs_period_msec, "CSI-RS period in milliseconds")
      ->capture_default_str()
      ->check(CLI::IsMember({10, 20, 40, 80}));
  app.add_option("--meas_csi_rs_slot_offset",
                 csi_params.meas_csi_slot_offset,
                 "Slot offset of first CSI-RS resource used for measurement")
      ->capture_default_str();
  app.add_option("--tracking_csi_rs_slot_offset",
                 csi_params.tracking_csi_slot_offset,
                 "Slot offset of first CSI-RS resource used for tracking")
      ->capture_default_str();
  app.add_option("--zp_csi_rs_slot_offset", csi_params.zp_csi_slot_offset, "Slot offset of the ZP CSI-RS resources")
      ->capture_default_str();
  app.add_option("--pwr_ctrl_offset",
                 csi_params.pwr_ctrl_offset,
                 "powerControlOffset, Power offset of PDSCH RE to NZP CSI-RS RE in dB")
      ->capture_default_str()
      ->check(CLI::Range(-8, 15));
}

static void configure_cli11_mac_bsr_args(CLI::App& app, mac_bsr_appconfig& bsr_params)
{
  app.add_option("--periodic_bsr_timer",
                 bsr_params.periodic_bsr_timer,
                 "Periodic Buffer Status Report Timer value in nof. subframes. Value 0 equates to infinity")
      ->capture_default_str()
      ->check(CLI::IsMember({1, 5, 10, 16, 20, 32, 40, 64, 80, 128, 160, 320, 640, 1280, 2560, 0}));
  app.add_option("--retx_bsr_timer",
                 bsr_params.retx_bsr_timer,
                 "Retransmission Buffer Status Report Timer value in nof. subframes")
      ->capture_default_str()
      ->check(CLI::IsMember({10, 20, 40, 80, 160, 320, 640, 1280, 2560, 5120, 10240}));
  app.add_option(
         "--lc_sr_delay_timer", bsr_params.lc_sr_delay_timer, "Logical Channel SR delay timer in nof. subframes")
      ->capture_default_str()
      ->check(CLI::IsMember({20, 40, 64, 128, 512, 1024, 2560}));
}

static void configure_cli11_mac_phr_args(CLI::App& app, mac_phr_appconfig& phr_params)
{
  app.add_option("--phr_prohibit_timer", phr_params.phr_prohib_timer, "PHR prohibit timer in nof. subframes")
      ->capture_default_str()
      ->check(CLI::IsMember({0, 10, 20, 50, 100, 200, 500, 1000}));
}

static void configure_cli11_mac_sr_args(CLI::App& app, mac_sr_appconfig& sr_params)
{
  app.add_option("--sr_trans_max", sr_params.sr_trans_max, "Maximum number of SR transmissions")
      ->capture_default_str()
      ->check(CLI::IsMember({4, 8, 16, 32, 64}));
  app.add_option("--sr_prohibit_timer", sr_params.sr_prohibit_timer, "Timer for SR transmission on PUCCH in ms")
      ->check(CLI::IsMember({1, 2, 4, 8, 16, 32, 64, 128}));
}

static void configure_cli11_mac_cell_group_args(CLI::App& app, mac_cell_group_appconfig& mcg_params)
{
  // BSR configuration.
  CLI::App* bsr_subcmd = app.add_subcommand("bsr_cfg", "Buffer status report configuration parameters");
  configure_cli11_mac_bsr_args(*bsr_subcmd, mcg_params.bsr_cfg);
  CLI::App* phr_subcmd = app.add_subcommand("phr_cfg", "Power Headroom report configuration parameters");
  configure_cli11_mac_phr_args(*phr_subcmd, mcg_params.phr_cfg);
  CLI::App* sr_subcmd = app.add_subcommand("sr_cfg", "Scheduling Request configuration parameters");
  configure_cli11_mac_sr_args(*sr_subcmd, mcg_params.sr_cfg);
}

static void configure_cli11_common_cell_args(CLI::App& app, base_cell_appconfig& cell_params)
{
  app.add_option("--pci", cell_params.pci, "PCI")->capture_default_str()->check(CLI::Range(0, 1007));
  app.add_option("--dl_arfcn", cell_params.dl_arfcn, "Downlink ARFCN")->capture_default_str();
  add_auto_enum_option(app, "--band", cell_params.band, "NR band");
  app.add_option("--common_scs", cell_params.common_scs, "Cell common subcarrier spacing")
      ->transform([](const std::string& value) {
        subcarrier_spacing scs = to_subcarrier_spacing(value);
        if (scs == subcarrier_spacing::invalid) {
          return "Invalid common subcarrier spacing '" + value + "'";
        }
        return std::to_string(to_numerology_value(scs));
      })
      ->capture_default_str();
  app.add_option("--channel_bandwidth_MHz", cell_params.channel_bw_mhz, "Channel bandwidth in MHz")
      ->capture_default_str()
      ->check([](const std::string& value) -> std::string {
        std::stringstream ss(value);
        unsigned          bw;
        ss >> bw;
        const std::string& error_message = "Error in the channel bandwidth property. Valid values "
                                           "[5,10,15,20,25,30,40,50,60,70,80,90,100]";
        // Bandwidth cannot be less than 5MHz.
        if (bw < 5U) {
          return error_message;
        }

        // Check from [5-25] in steps of 5.
        if (bw < 26U) {
          return ((bw % 5) == 0) ? "" : error_message;
        }

        // Check from [30-100] in steps of 10.
        if (bw < 101U) {
          return ((bw % 10) == 0) ? "" : error_message;
        }

        return error_message;
      });
  app.add_option("--nof_antennas_ul", cell_params.nof_antennas_ul, "Number of antennas in uplink")
      ->capture_default_str();
  app.add_option("--nof_antennas_dl", cell_params.nof_antennas_dl, "Number of antennas in downlink")
      ->capture_default_str();
  app.add_option("--plmn", cell_params.plmn, "PLMN")->capture_default_str();
  app.add_option("--tac", cell_params.tac, "TAC")->capture_default_str()->check([](const std::string& value) {
    std::stringstream ss(value);
    unsigned          tac;
    ss >> tac;

    // Values 0 and 0xfffffe are reserved.
    if (tac == 0U || tac == 0xfffffeU) {
      return "TAC values 0 or 0xfffffe are reserved";
    }

    return (tac <= 0xffffffU) ? "" : "TAC value out of range";
  });
  app.add_option("--q_rx_lev_min",
                 cell_params.q_rx_lev_min,
                 "q-RxLevMin, required minimum received RSRP level for cell selection/re-selection, in dBm")
      ->capture_default_str()
      ->check(CLI::Range(-70, -22));
  app.add_option("--q_qual_min",
                 cell_params.q_qual_min,
                 "q-QualMin, required minimum received RSRQ level for cell selection/re-selection, in dB")
      ->capture_default_str()
      ->check(CLI::Range(-43, -12));
  app.add_option("--pcg_p_nr_fr1",
                 cell_params.pcg_cfg.p_nr_fr1,
                 "p-nr-fr1, maximum total TX power to be used by the UE in this NR cell group across in FR1")
      ->capture_default_str()
      ->check(CLI::Range(-30, 23));

  // MAC Cell group parameters.
  CLI::App* mcg_subcmd = app.add_subcommand("mac_cell_group", "MAC Cell Group parameters")->configurable();
  configure_cli11_mac_cell_group_args(*mcg_subcmd, cell_params.mcg_cfg);

  // SSB configuration.
  CLI::App* ssb_subcmd = app.add_subcommand("ssb", "SSB parameters");
  configure_cli11_ssb_args(*ssb_subcmd, cell_params.ssb_cfg);

  // SIB configuration.
  CLI::App* sib_subcmd = app.add_subcommand("sib", "SIB configuration parameters");
  configure_cli11_sib_args(*sib_subcmd, cell_params.sib_cfg);

  // UL common configuration.
  CLI::App* ul_common_subcmd = app.add_subcommand("ul_common", "UL common parameters");
  configure_cli11_ul_common_args(*ul_common_subcmd, cell_params.ul_common_cfg);

  // PDCCH configuration.
  CLI::App* pdcch_subcmd = app.add_subcommand("pdcch", "PDCCH parameters");
  configure_cli11_pdcch_args(*pdcch_subcmd, cell_params.pdcch_cfg);

  // PDSCH configuration.
  CLI::App* pdsch_subcmd = app.add_subcommand("pdsch", "PDSCH parameters");
  configure_cli11_pdsch_args(*pdsch_subcmd, cell_params.pdsch_cfg);

  // PUSCH configuration.
  CLI::App* pusch_subcmd = app.add_subcommand("pusch", "PUSCH parameters");
  configure_cli11_pusch_args(*pusch_subcmd, cell_params.pusch_cfg);

  // PUSCH configuration.
  CLI::App* pucch_subcmd = app.add_subcommand("pucch", "PUCCH parameters");
  configure_cli11_pucch_args(*pucch_subcmd, cell_params.pucch_cfg);

  // PRACH configuration.
  CLI::App* prach_subcmd = app.add_subcommand("prach", "PRACH parameters");
  configure_cli11_prach_args(*prach_subcmd, cell_params.prach_cfg);

  // TDD UL DL configuration.
  // NOTE: CLI11 needs that the life of the variable last longer than the call of callback function. Therefore, the
  // tdd_cfg variable needs to be static.
  static tdd_ul_dl_appconfig tdd_app_cfg;
  CLI::App*                  tdd_ul_dl_subcmd =
      app.add_subcommand("tdd_ul_dl_cfg", "TDD UL DL configuration parameters")->configurable();
  configure_cli11_tdd_ul_dl_args(*tdd_ul_dl_subcmd, tdd_app_cfg);
  auto tdd_ul_dl_verify_callback = [&]() {
    CLI::App* tdd_sub_cmd = app.get_subcommand("tdd_ul_dl_cfg");
    if (tdd_sub_cmd->count() != 0) {
      cell_params.tdd_ul_dl_cfg.emplace(tdd_app_cfg);
    }
  };
  tdd_ul_dl_subcmd->parse_complete_callback(tdd_ul_dl_verify_callback);

  // Paging configuration.
  CLI::App* paging_subcmd = app.add_subcommand("paging", "Paging parameters");
  configure_cli11_paging_args(*paging_subcmd, cell_params.paging_cfg);

  // CSI configuration.
  CLI::App* csi_subcmd = app.add_subcommand("csi", "CSI-Meas parameters");
  configure_cli11_csi_args(*csi_subcmd, cell_params.csi_cfg);
}

static void configure_cli11_cells_args(CLI::App& app, cell_appconfig& cell_params)
{
  configure_cli11_common_cell_args(app, cell_params.cell);
}

static void configure_cli11_rlc_um_args(CLI::App& app, rlc_um_appconfig& rlc_um_params)
{
  CLI::App* rlc_tx_um_subcmd = app.add_subcommand("tx", "UM TX parameters");
  rlc_tx_um_subcmd->add_option("--sn", rlc_um_params.tx.sn_field_length, "RLC UM TX SN")->capture_default_str();
  rlc_tx_um_subcmd->add_option("--queue-size", rlc_um_params.tx.queue_size, "RLC UM TX SDU queue size")
      ->capture_default_str();
  CLI::App* rlc_rx_um_subcmd = app.add_subcommand("rx", "UM TX parameters");
  rlc_rx_um_subcmd->add_option("--sn", rlc_um_params.rx.sn_field_length, "RLC UM RX SN")->capture_default_str();
  rlc_rx_um_subcmd->add_option("--t-reassembly", rlc_um_params.rx.t_reassembly, "RLC UM t-Reassembly")
      ->capture_default_str();
}

static void configure_cli11_rlc_am_args(CLI::App& app, rlc_am_appconfig& rlc_am_params)
{
  CLI::App* rlc_tx_am_subcmd = app.add_subcommand("tx", "AM TX parameters");
  rlc_tx_am_subcmd->add_option("--sn", rlc_am_params.tx.sn_field_length, "RLC AM TX SN size")->capture_default_str();
  rlc_tx_am_subcmd->add_option("--t-poll-retransmit", rlc_am_params.tx.t_poll_retx, "RLC AM TX t-PollRetransmit (ms)")
      ->capture_default_str();
  rlc_tx_am_subcmd->add_option("--max-retx-threshold", rlc_am_params.tx.max_retx_thresh, "RLC AM max retx threshold")
      ->capture_default_str();
  rlc_tx_am_subcmd->add_option("--poll-pdu", rlc_am_params.tx.poll_pdu, "RLC AM TX PollPdu")->capture_default_str();
  rlc_tx_am_subcmd->add_option("--poll-byte", rlc_am_params.tx.poll_byte, "RLC AM TX PollByte")->capture_default_str();
  rlc_tx_am_subcmd->add_option(
      "--max_window",
      rlc_am_params.tx.max_window,
      "Non-standard parameter that limits the tx window size. Can be used for limiting memory usage with "
      "large windows. 0 means no limits other than the SN size (i.e. 2^[sn_size-1]).");
  rlc_tx_am_subcmd->add_option("--queue-size", rlc_am_params.tx.queue_size, "RLC AM TX SDU queue size")
      ->capture_default_str();
  CLI::App* rlc_rx_am_subcmd = app.add_subcommand("rx", "AM RX parameters");
  rlc_rx_am_subcmd->add_option("--sn", rlc_am_params.rx.sn_field_length, "RLC AM RX SN")->capture_default_str();
  rlc_rx_am_subcmd->add_option("--t-reassembly", rlc_am_params.rx.t_reassembly, "RLC AM RX t-Reassembly")
      ->capture_default_str();
  rlc_rx_am_subcmd->add_option("--t-status-prohibit", rlc_am_params.rx.t_status_prohibit, "RLC AM RX t-StatusProhibit")
      ->capture_default_str();
  rlc_rx_am_subcmd->add_option("--max_sn_per_status", rlc_am_params.rx.max_sn_per_status, "RLC AM RX status SN limit")
      ->capture_default_str();
  ;
}

static void configure_cli11_rlc_args(CLI::App& app, rlc_appconfig& rlc_params)
{
  app.add_option("--mode", rlc_params.mode, "RLC mode")->capture_default_str();
  CLI::App* rlc_um_subcmd = app.add_subcommand("um-bidir", "UM parameters");
  configure_cli11_rlc_um_args(*rlc_um_subcmd, rlc_params.um);
  CLI::App* rlc_am_subcmd = app.add_subcommand("am", "AM parameters");
  configure_cli11_rlc_am_args(*rlc_am_subcmd, rlc_params.am);
}

static void configure_cli11_f1u_du_args(CLI::App& app, f1u_du_appconfig& f1u_du_params)
{
  app.add_option("--backoff_timer", f1u_du_params.t_notify, "F1-U backoff timer (ms)")->capture_default_str();
}

static void configure_cli11_f1u_cu_up_args(CLI::App& app, f1u_cu_up_appconfig& f1u_cu_up_params)
{
  app.add_option("--backoff_timer", f1u_cu_up_params.t_notify, "F1-U backoff timer (ms)")->capture_default_str();
}

static void configure_cli11_pdcp_tx_args(CLI::App& app, pdcp_tx_appconfig& pdcp_tx_params)
{
  app.add_option("--sn", pdcp_tx_params.sn_field_length, "PDCP TX SN size")->capture_default_str();
  app.add_option("--discard_timer", pdcp_tx_params.discard_timer, "PDCP TX discard timer (ms)")->capture_default_str();
  app.add_option("--status_report_required", pdcp_tx_params.status_report_required, "PDCP TX status report required")
      ->capture_default_str();
}

static void configure_cli11_pdcp_rx_args(CLI::App& app, pdcp_rx_appconfig& pdcp_rx_params)
{
  app.add_option("--sn", pdcp_rx_params.sn_field_length, "PDCP RX SN size")->capture_default_str();
  app.add_option("--t_reordering", pdcp_rx_params.t_reordering, "PDCP RX t-Reordering (ms)")->capture_default_str();
  app.add_option(
         "--out_of_order_delivery", pdcp_rx_params.out_of_order_delivery, "PDCP RX enable out-of-order delivery")
      ->capture_default_str();
}

static void configure_cli11_pdcp_args(CLI::App& app, pdcp_appconfig& pdcp_params)
{
  app.add_option("integrity_required", pdcp_params.integrity_protection_required, "DRB Integrity required")
      ->capture_default_str();
  CLI::App* pdcp_tx_subcmd = app.add_subcommand("tx", "PDCP TX parameters");
  configure_cli11_pdcp_tx_args(*pdcp_tx_subcmd, pdcp_params.tx);
  CLI::App* pdcp_rx_subcmd = app.add_subcommand("rx", "PDCP RX parameters");
  configure_cli11_pdcp_rx_args(*pdcp_rx_subcmd, pdcp_params.rx);
}

static void configure_cli11_mac_args(CLI::App& app, mac_lc_appconfig& mac_params)
{
  app.add_option("--lc_priority",
                 mac_params.priority,
                 "Logical Channel priority. An increasing priority value indicates a lower priority level")
      ->capture_default_str()
      ->check(CLI::Range(4, 16));
  app.add_option("--lc_group_id", mac_params.lc_group_id, "Logical Channel Group id")
      ->capture_default_str()
      ->check(CLI::Range(1, 7));
  app.add_option(
         "--bucket_size_duration_ms", mac_params.bucket_size_duration_ms, "Bucket size duration in milliseconds")
      ->capture_default_str()
      ->check(CLI::IsMember({5, 10, 20, 50, 100, 150, 300, 500, 1000}));
  app.add_option("--prioritized_bit_rate_kBps",
                 mac_params.prioritized_bit_rate_kBps,
                 "Prioritized bit rate in kBps. Value 65537 means infinity")
      ->capture_default_str()
      ->check(CLI::IsMember({0, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 2768, 65536, 65537}));
}

static void configure_cli11_srb_args(CLI::App& app, srb_appconfig& srb_params)
{
  app.add_option("--srb_id", srb_params.srb_id, "SRB Id")->capture_default_str()->check(CLI::IsMember({1, 2}));
  CLI::App* rlc_subcmd = app.add_subcommand("rlc", "RLC parameters");
  configure_cli11_rlc_am_args(*rlc_subcmd, srb_params.rlc);
  auto verify_callback = [&]() {
    CLI::App* rlc = app.get_subcommand("rlc");
    if (rlc->count_all() == 0) {
      report_error("Error parsing SRB config for SRB{}. RLC configuration not present.\n", srb_params.srb_id);
    }
  };
  app.callback(verify_callback);
}

static void configure_cli11_qos_args(CLI::App& app, qos_appconfig& qos_params)
{
  app.add_option("--five_qi", qos_params.five_qi, "5QI")->capture_default_str()->check(CLI::Range(0, 255));
  CLI::App* rlc_subcmd = app.add_subcommand("rlc", "RLC parameters");
  configure_cli11_rlc_args(*rlc_subcmd, qos_params.rlc);
  CLI::App* f1u_du_subcmd = app.add_subcommand("f1u_du", "F1-U parameters at DU side");
  configure_cli11_f1u_du_args(*f1u_du_subcmd, qos_params.f1u_du);
  CLI::App* f1u_cu_up_subcmd = app.add_subcommand("f1u_cu_up", "F1-U parameters at CU_UP side");
  configure_cli11_f1u_cu_up_args(*f1u_cu_up_subcmd, qos_params.f1u_cu_up);
  CLI::App* pdcp_subcmd = app.add_subcommand("pdcp", "PDCP parameters");
  configure_cli11_pdcp_args(*pdcp_subcmd, qos_params.pdcp);
  CLI::App* mac_subcmd = app.add_subcommand("mac", "MAC parameters");
  configure_cli11_mac_args(*mac_subcmd, qos_params.mac);
  auto verify_callback = [&]() {
    CLI::App* rlc       = app.get_subcommand("rlc");
    CLI::App* f1u_du    = app.get_subcommand("f1u_du");
    CLI::App* f1u_cu_up = app.get_subcommand("f1u_cu_up");
    CLI::App* pdcp      = app.get_subcommand("pdcp");
    CLI::App* mac       = app.get_subcommand("mac");
    if (rlc->count_all() == 0) {
      report_error("Error parsing QoS config for 5QI {}. RLC configuration not present.\n", qos_params.five_qi);
    }
    if (f1u_du->count_all() == 0) {
      report_error("Error parsing QoS config for 5QI {}. F1-U DU configuration not present.\n", qos_params.five_qi);
    }
    if (f1u_cu_up->count_all() == 0) {
      report_error("Error parsing QoS config for 5QI {}. F1-U CU_UP configuration not present.\n", qos_params.five_qi);
    }
    if (pdcp->count_all() == 0) {
      report_error("Error parsing QoS config for 5QI {}. PDCP configuration not present.\n", qos_params.five_qi);
    }
    if (mac->count_all() == 0) {
      report_error("Error parsing QoS config for 5QI {}. MAC configuration not present.\n", qos_params.five_qi);
    }
  };
  app.callback(verify_callback);
}

static void configure_cli11_test_ue_mode_args(CLI::App& app, test_mode_ue_appconfig& test_params)
{
  app.add_option("--rnti", test_params.rnti, "C-RNTI (0x0 if not configured)")
      ->capture_default_str()
      ->check(CLI::Range(to_value((rnti_t::INVALID_RNTI)), to_value(rnti_t::MAX_CRNTI)));
  app.add_option("--nof_ues", test_params.nof_ues, "Number of test UE(s) to create.")
      ->capture_default_str()
      ->check(CLI::Range((uint16_t)1, (uint16_t)MAX_NOF_DU_UES));
  app.add_option("--auto_ack_indication_delay",
                 test_params.auto_ack_indication_delay,
                 "Delay before the UL and DL HARQs are automatically ACKed. This feature should only be used if the UL "
                 "PHY is not operational")
      ->capture_default_str();
  app.add_option("--pdsch_active", test_params.pdsch_active, "PDSCH enabled")->capture_default_str();
  app.add_option("--pusch_active", test_params.pusch_active, "PUSCH enabled")->capture_default_str();
  app.add_option("--cqi", test_params.cqi, "Channel Quality Information (CQI) to be forwarded to test UE.")
      ->capture_default_str()
      ->check(CLI::Range(1, 15));
  app.add_option("--ri", test_params.ri, "Rank Indicator (RI) to be forwarded to test UE.")
      ->capture_default_str()
      ->check(CLI::Range(1, 4));
  app.add_option("--pmi", test_params.pmi, "Precoder Matrix Indicator (PMI) to be forwarded to test UE.")
      ->capture_default_str()
      ->check(CLI::Range(0, 3));
  app.add_option(
         "--i_1_1",
         test_params.i_1_1,
         "Precoder Matrix codebook index \"i_1_1\" to be forwarded to test UE, in the case of more than 2 antennas.")
      ->capture_default_str()
      ->check(CLI::Range(0, 7));
  app.add_option(
         "--i_1_3",
         test_params.i_1_3,
         "Precoder Matrix codebook index \"i_1_3\" to be forwarded to test UE, in the case of more than 2 antennas.")
      ->capture_default_str()
      ->check(CLI::Range(0, 1));
  app.add_option(
         "--i_2",
         test_params.i_2,
         "Precoder Matrix codebook index \"i_2\" to be forwarded to test UE, in the case of more than 2 antennas.")
      ->capture_default_str()
      ->check(CLI::Range(0, 3));
}

static void configure_cli11_test_mode_args(CLI::App& app, test_mode_appconfig& test_params)
{
  CLI::App* test_ue = app.add_subcommand("test_ue", "automatically created UE for testing purposes");
  configure_cli11_test_ue_mode_args(*test_ue, test_params.test_ue);
}

static void configure_cli11_ru_sdr_expert_args(CLI::App& app, ru_sdr_expert_appconfig& config)
{
  auto buffer_size_policy_check = [](const std::string& value) -> std::string {
    if (value == "auto" || value == "single-packet" || value == "half-slot" || value == "slot" ||
        value == "optimal-slot") {
      return {};
    }
    return "Invalid DL buffer size policy. Accepted values [auto,single-packet,half-slot,slot,optimal-slot]";
  };

  app.add_option("--low_phy_dl_throttling",
                 config.lphy_dl_throttling,
                 "Throttles the lower PHY DL baseband generation. The range is (0, 1). Set it to zero to disable it.")
      ->capture_default_str();
  app.add_option("--discontinuous_tx",
                 config.discontinuous_tx_mode,
                 "Enables discontinuous transmission mode for the radio front-ends supporting it.")
      ->capture_default_str();
  app.add_option("--power_ramping_time_us",
                 config.power_ramping_time_us,
                 "Specifies the power ramping time in microseconds, it proactively initiates the transmission and "
                 "mitigates transient effects.")
      ->capture_default_str();
  app.add_option(
         "--dl_buffer_size_policy",
         config.dl_buffer_size_policy,
         "Selects the size policy of the baseband buffers that pass DL samples from the lower PHY to the radio.")
      ->capture_default_str()
      ->check(buffer_size_policy_check);
}

static void configure_cli11_ru_sdr_args(CLI::App& app, ru_sdr_appconfig& config)
{
  app.add_option("--srate", config.srate_MHz, "Sample rate in MHz")->capture_default_str();
  app.add_option("--device_driver", config.device_driver, "Device driver name")->capture_default_str();
  app.add_option("--device_args", config.device_arguments, "Optional device arguments")->capture_default_str();
  app.add_option("--tx_gain", config.tx_gain_dB, "Transmit gain in decibels")->capture_default_str();
  app.add_option("--rx_gain", config.rx_gain_dB, "Receive gain in decibels")->capture_default_str();
  app.add_option("--freq_offset", config.center_freq_offset_Hz, "Center frequency offset in hertz")
      ->capture_default_str();
  app.add_option("--clock_ppm", config.calibrate_clock_ppm, "Clock calibration in PPM.")->capture_default_str();
  app.add_option("--lo_offset", config.lo_offset_MHz, "LO frequency offset in MHz")->capture_default_str();
  app.add_option("--clock", config.clock_source, "Clock source")->capture_default_str();
  app.add_option("--sync", config.synch_source, "Time synchronization source")->capture_default_str();
  app.add_option("--otw_format", config.otw_format, "Over-the-wire format")->capture_default_str();
  app.add_option_function<std::string>(
         "--time_alignment_calibration",
         [&config](const std::string& value) {
           if (!value.empty() && value != "auto") {
             std::stringstream ss(value);
             int               ta_samples;
             ss >> ta_samples;
             config.time_alignment_calibration = ta_samples;
           }
         },
         "Rx to Tx radio time alignment calibration in samples.\n"
         "Positive values reduce the RF transmission delay with respect\n"
         "to the RF reception, while negative values increase it")
      ->check([](const std::string& value) -> std::string {
        // Check for valid option "auto".
        if (value == "auto") {
          return "";
        }

        // Check for a valid integer number;
        CLI::TypeValidator<int> IntegerValidator("INTEGER");
        return IntegerValidator(value);
      })
      ->default_str("auto");

  // Amplitude control configuration.
  CLI::App* amplitude_control_subcmd = app.add_subcommand("amplitude_control", "Amplitude control parameters");
  configure_cli11_amplitude_control_args(*amplitude_control_subcmd, config.amplitude_cfg);

  // Expert configuration.
  CLI::App* expert_subcmd = app.add_subcommand("expert_cfg", "Generic Radio Unit expert configuration");
  configure_cli11_ru_sdr_expert_args(*expert_subcmd, config.expert_cfg);
}

static void configure_cli11_ru_ofh_base_cell_args(CLI::App& app, ru_ofh_base_cell_appconfig& config)
{
  app.add_option_function<unsigned>(
         "--ru_bandwidth_MHz",
         [&config](unsigned value) { config.ru_operating_bw = MHz_to_bs_channel_bandwidth(value); },
         "Channel bandwidth in MHz")
      ->check([](const std::string& value) -> std::string {
        std::stringstream ss(value);
        unsigned          bw;
        ss >> bw;
        const std::string& error_message = "Error in the channel bandwidth property. Valid values "
                                           "[5,10,15,20,25,30,40,50,60,70,80,90,100]";
        // Bandwidth cannot be less than 5MHz.
        if (bw < 5U) {
          return error_message;
        }

        // Check from [5-25] in steps of 5.
        if (bw < 26U) {
          return ((bw % 5) == 0) ? "" : error_message;
        }

        // Check from [30-100] in steps of 10.
        if (bw < 101U) {
          return ((bw % 10) == 0) ? "" : error_message;
        }

        return error_message;
      });

  // Note: For the timing parameters, worst case is 2 slots for scs 15KHz and 14 symbols. Implementation defined.
  app.add_option("--t1a_max_cp_dl", config.T1a_max_cp_dl, "T1a maximum value for downlink Control-Plane")
      ->capture_default_str()
      ->check(CLI::Range(0, 1960));
  app.add_option("--t1a_min_cp_dl", config.T1a_min_cp_dl, "T1a minimum value for downlink Control-Plane")
      ->capture_default_str()
      ->check(CLI::Range(0, 1960));
  app.add_option("--t1a_max_cp_ul", config.T1a_max_cp_ul, "T1a maximum value for uplink Control-Plane")
      ->capture_default_str()
      ->check(CLI::Range(0, 1960));
  app.add_option("--t1a_min_cp_ul", config.T1a_min_cp_ul, "T1a minimum value for uplink Control-Plane")
      ->capture_default_str()
      ->check(CLI::Range(0, 1960));
  app.add_option("--t1a_max_up", config.T1a_max_up, "T1a maximum value for User-Plane")
      ->capture_default_str()
      ->check(CLI::Range(0, 1960));
  app.add_option("--t1a_min_up", config.T1a_min_up, "T1a minimum value for User-Plane")
      ->capture_default_str()
      ->check(CLI::Range(0, 1960));
  app.add_option("--ta4_max", config.Ta4_max, "Ta4 maximum value for User-Plane")
      ->capture_default_str()
      ->check(CLI::Range(0, 1960));
  app.add_option("--ta4_min", config.Ta4_min, "Ta4 minimum value for User-Plane")
      ->capture_default_str()
      ->check(CLI::Range(0, 1960));

  if (config.T1a_min_cp_dl > config.T1a_max_cp_dl) {
    report_error("Invalid Open Fronthaul Radio Unit configuration detected. T1a maximum value must be greater than "
                 "minimum value for downlink Control-Plane.\n");
  }
  if (config.T1a_min_cp_ul > config.T1a_max_cp_ul) {
    report_error("Invalid Open Fronthaul Radio Unit configuration detected. T1a maximum value must be greater than "
                 "minimum value for uplink Control-Plane.\n");
  }
  if (config.T1a_min_up > config.T1a_max_up) {
    report_error("Invalid Open Fronthaul Radio Unit configuration detected. T1a maximum value must be greater than "
                 "minimum value for User-Plane.\n");
  }

  app.add_option("--is_prach_cp_enabled", config.is_prach_control_plane_enabled, "PRACH Control-Plane enabled flag")
      ->capture_default_str();
  app.add_option("--is_dl_broadcast_enabled", config.is_downlink_broadcast_enabled, "Downlink broadcast enabled flag")
      ->capture_default_str();
  app.add_option("--ignore_ecpri_seq_id", config.ignore_ecpri_seq_id_field, "Ignore eCPRI sequence id field value")
      ->capture_default_str();
  app.add_option(
         "--ignore_ecpri_payload_size", config.ignore_ecpri_payload_size_field, "Ignore eCPRI payload size field value")
      ->capture_default_str();
  app.add_option(
         "--warn_unreceived_ru_frames", config.warn_unreceived_ru_frames, "Warn of unreceived Radio Unit frames")
      ->capture_default_str();

  auto compression_method_check = [](const std::string& value) -> std::string {
    if (value == "none" || value == "bfp" || value == "bfp selective" || value == "block scaling" ||
        value == "mu law" || value == "modulation" || value == "modulation selective") {
      return {};
    }

    return "Compression method not supported. Accepted values [none, bfp, bfp selective, block scaling, mu law, "
           "modulation, modulation selective]";
  };

  app.add_option("--compr_method_ul", config.compression_method_ul, "Uplink compression method")
      ->capture_default_str()
      ->check(compression_method_check);
  app.add_option("--compr_bitwidth_ul", config.compression_bitwidth_ul, "Uplink compression bit width")
      ->capture_default_str()
      ->check(CLI::Range(1, 16));
  app.add_option("--compr_method_dl", config.compression_method_dl, "Downlink compression method")
      ->capture_default_str()
      ->check(compression_method_check);
  app.add_option("--compr_bitwidth_dl", config.compression_bitwidth_dl, "Downlink compression bit width")
      ->capture_default_str()
      ->check(CLI::Range(1, 16));
  app.add_option("--compr_method_prach", config.compression_method_prach, "PRACH compression method")
      ->capture_default_str()
      ->check(compression_method_check);
  app.add_option("--compr_bitwidth_prach", config.compression_bitwidth_prach, "PRACH compression bit width")
      ->capture_default_str()
      ->check(CLI::Range(1, 16));
  app.add_option("--enable_ul_static_compr_hdr",
                 config.is_uplink_static_comp_hdr_enabled,
                 "Uplink static compression header enabled flag")
      ->capture_default_str();
  app.add_option("--enable_dl_static_compr_hdr",
                 config.is_downlink_static_comp_hdr_enabled,
                 "Downlink static compression header enabled flag")
      ->capture_default_str();
  app.add_option("--iq_scaling", config.iq_scaling, "IQ scaling factor")
      ->capture_default_str()
      ->check(CLI::Range(0.0, 10.0));

  // Callback function for validating that both compression method and bitwidth parameters were specified.
  auto validate_compression_input = [](CLI::App& cli_app, const std::string& direction) {
    std::string method_param    = "--compr_method_" + direction;
    std::string bitwidth_param  = "--compr_bitwidth_" + direction;
    unsigned    comp_type_count = cli_app.count(method_param);
    unsigned    comp_bit_count  = cli_app.count(bitwidth_param);
    if (comp_type_count != comp_bit_count) {
      report_error("Invalid Open Fronthaul Radio Unit configuration detected: both compression method and compression "
                   "bitwidth must be specified\n");
    }
  };

  // Post-parsing callback to validate that compression method and bitwidth parameters were both specified or both set
  // to default.
  app.callback([&]() {
    validate_compression_input(app, "dl");
    validate_compression_input(app, "ul");
    validate_compression_input(app, "prach");
  });
}

static void configure_cli11_ru_ofh_cells_args(CLI::App& app, ru_ofh_cell_appconfig& config)
{
  configure_cli11_ru_ofh_base_cell_args(app, config.cell);
  app.add_option(
         "--network_interface", config.network_interface, "Network interface name or PCIe identifier when using DPDK")
      ->capture_default_str();
  app.add_option("--enable_promiscuous", config.enable_promiscuous_mode, "Promiscuous mode flag")
      ->capture_default_str();
  app.add_option("--mtu", config.mtu_size, "NIC interface MTU size")
      ->capture_default_str()
      ->check(CLI::Range(1500, 9600));
  app.add_option("--ru_mac_addr", config.ru_mac_address, "Radio Unit MAC address")->capture_default_str();
  app.add_option("--du_mac_addr", config.du_mac_address, "Distributed Unit MAC address")->capture_default_str();
  app.add_option("--vlan_tag", config.vlan_tag, "V-LAN identifier")->capture_default_str()->check(CLI::Range(1, 4094));
  app.add_option("--prach_port_id", config.ru_prach_port_id, "RU PRACH port identifier")->capture_default_str();
  app.add_option("--dl_port_id", config.ru_dl_port_id, "RU downlink port identifier")->capture_default_str();
  app.add_option("--ul_port_id", config.ru_ul_port_id, "RU uplink port identifier")->capture_default_str();
}

static void configure_cli11_ru_ofh_args(CLI::App& app, ru_ofh_appconfig& config)
{
  app.add_option("--gps_alpha", config.gps_Alpha, "GPS Alpha")->capture_default_str()->check(CLI::Range(0.0, 1.2288e7));
  app.add_option("--gps_beta", config.gps_Beta, "GPS Beta")->capture_default_str()->check(CLI::Range(-32768, 32767));

  // Common cell parameters.
  configure_cli11_ru_ofh_base_cell_args(app, config.base_cell_cfg);

  // Cell parameters.
  app.add_option_function<std::vector<std::string>>(
      "--cells",
      [&config](const std::vector<std::string>& values) {
        config.cells.resize(values.size());
        for (auto& cell : config.cells) {
          cell.cell = config.base_cell_cfg;
        };

        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("RU OFH cells");
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras(CLI::config_extras_mode::error);
          configure_cli11_ru_ofh_cells_args(subapp, config.cells[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Sets the cell configuration on a per cell basis, overwriting the default configuration defined by cell_cfg");
}

static void configure_cli11_ru_dummy_args(CLI::App& app, ru_dummy_appconfig& config)
{
  app.add_option("--dl_processing_delay", config.dl_processing_delay, "DL processing processing delay in slots")
      ->capture_default_str();
}

static void configure_cli11_buffer_pool_args(CLI::App& app, buffer_pool_appconfig& config)
{
  app.add_option("--nof_segments", config.nof_segments, "Number of segments allocated by the buffer pool")
      ->capture_default_str();
  app.add_option("--segment_size", config.segment_size, "Size of each buffer pool segment in bytes")
      ->capture_default_str();
}

static void configure_cli11_hal_args(CLI::App& app, optional<hal_appconfig>& config)
{
  config.emplace();

  app.add_option("--eal_args", config->eal_args, "EAL configuration parameters used to initialize DPDK");
}

static error_type<std::string> is_valid_cpu_index(unsigned cpu_idx)
{
  std::string error_message = fmt::format("Invalid CPU core selected '{}'. Valid CPU ids: {}",
                                          cpu_idx,
                                          os_sched_affinity_bitmask::available_cpus().get_cpu_ids());

  os_sched_affinity_bitmask one_cpu_mask;
  if (cpu_idx >= one_cpu_mask.size()) {
    return error_message;
  }
  one_cpu_mask.set(cpu_idx);
  if (not one_cpu_mask.subtract(os_sched_affinity_bitmask::available_cpus()).empty()) {
    return error_message;
  }
  return default_success_t();
}

static expected<unsigned, std::string> parse_one_cpu(const std::string& value)
{
  expected<int, std::string> result = parse_int<int>(value);

  if (result.is_error()) {
    return fmt::format("Could not parse '{}' string as a CPU index", value);
  }

  error_type<std::string> validation_result = is_valid_cpu_index(result.value());
  if (validation_result.is_error()) {
    return validation_result.error();
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
    if (parse_result.is_error()) {
      return fmt::format("{}. Could not parse '{}' as a range", parse_result.error(), value);
    }

    range.push_back(parse_result.value());
  }

  // A range is defined by two numbers.
  if (range.size() != 2) {
    return fmt::format("Could not parse '{}' as a range", value);
  }

  if (range[1] <= range[0]) {
    return fmt::format("Invalid CPU core range detected [{}-{}]", range[0], range[1]);
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
      if (range.is_error()) {
        report_error("{} in the '{}' property", range.error(), property_name);
      }

      // Add 1 to the stop value as the fill method excludes the end position.
      mask.fill(range.value().start(), range.value().stop() + 1);
    } else {
      auto cpu_idx = parse_one_cpu(str);
      if (cpu_idx.is_error()) {
        report_error("{} in the '{}' property", cpu_idx.error(), property_name);
      }

      mask.set(cpu_idx.value());
    }
  }
}

static void configure_cli11_cell_affinity_args(CLI::App& app, cpu_affinities_cell_appconfig& config)
{
  app.add_option_function<std::string>(
      "--l1_dl_cpus",
      [&config](const std::string& value) { parse_affinity_mask(config.l1_dl_cpu_cfg.mask, value, "l1_dl_cpus"); },
      "CPU cores assigned to L1 downlink tasks");

  app.add_option_function<std::string>(
      "--l1_ul_cpus",
      [&config](const std::string& value) { parse_affinity_mask(config.l1_ul_cpu_cfg.mask, value, "l1_ul_cpus"); },
      "CPU cores assigned to L1 uplink tasks");

  app.add_option_function<std::string>(
      "--l2_cell_cpus",
      [&config](const std::string& value) { parse_affinity_mask(config.l2_cell_cpu_cfg.mask, value, "l2_cell_cpus"); },
      "CPU cores assigned to L2 cell tasks");

  app.add_option_function<std::string>(
      "--ru_cpus",
      [&config](const std::string& value) { parse_affinity_mask(config.ru_cpu_cfg.mask, value, "ru_cpus"); },
      "Number of CPUs used for the Radio Unit tasks");

  app.add_option_function<std::string>(
      "--l1_dl_pinning",
      [&config](const std::string& value) {
        config.l1_dl_cpu_cfg.pinning_policy = to_affinity_mask_policy(value);
        if (config.l1_dl_cpu_cfg.pinning_policy == gnb_sched_affinity_mask_policy::last) {
          report_error("Incorrect value={} used in {} property", value, "l1_dl_pinning");
        }
      },
      "Policy used for assigning CPU cores to L1 downlink tasks");

  app.add_option_function<std::string>(
      "--l1_ul_pinning",
      [&config](const std::string& value) {
        config.l1_ul_cpu_cfg.pinning_policy = to_affinity_mask_policy(value);
        if (config.l1_ul_cpu_cfg.pinning_policy == gnb_sched_affinity_mask_policy::last) {
          report_error("Incorrect value={} used in {} property", value, "l1_ul_pinning");
        }
      },
      "Policy used for assigning CPU cores to L1 uplink tasks");

  app.add_option_function<std::string>(
      "--l2_cell_pinning",
      [&config](const std::string& value) {
        config.l2_cell_cpu_cfg.pinning_policy = to_affinity_mask_policy(value);
        if (config.l2_cell_cpu_cfg.pinning_policy == gnb_sched_affinity_mask_policy::last) {
          report_error("Incorrect value={} used in {} property", value, "l2_cell_pinning");
        }
      },
      "Policy used for assigning CPU cores to L2 cell tasks");

  app.add_option_function<std::string>(
      "--ru_pinning",
      [&config](const std::string& value) {
        config.ru_cpu_cfg.pinning_policy = to_affinity_mask_policy(value);
        if (config.ru_cpu_cfg.pinning_policy == gnb_sched_affinity_mask_policy::last) {
          report_error("Incorrect value={} used in {} property", value, "ru_pinning");
        }
      },
      "Policy used for assigning CPU cores to the Radio Unit tasks");
}

static void configure_cli11_non_rt_threads_args(CLI::App& app, non_rt_threads_appconfig& config)
{
  app.add_option("--nof_non_rt_threads",
                 config.nof_non_rt_threads,
                 "Number of non real time threads for processing of CP and UP data in upper layers.")
      ->capture_default_str()
      ->check(CLI::Number);
}

static void configure_cli11_upper_phy_threads_args(CLI::App& app, upper_phy_threads_appconfig& config)
{
  auto pdsch_processor_check = [](const std::string& value) -> std::string {
    if ((value == "auto") || (value == "generic") || (value == "concurrent") || (value == "lite")) {
      return {};
    }
    return "Invalid PDSCH processor type. Accepted values [auto,generic,concurrent,lite]";
  };

  app.add_option("--pdsch_processor_type",
                 config.pdsch_processor_type,
                 "PDSCH processor type: auto, generic, concurrent and lite.")
      ->capture_default_str()
      ->check(pdsch_processor_check);
  app.add_option("--nof_pusch_decoder_threads", config.nof_pusch_decoder_threads, "Number of threads to decode PUSCH.")
      ->capture_default_str()
      ->check(CLI::Number);
  app.add_option("--nof_ul_threads", config.nof_ul_threads, "Number of upper PHY threads to process uplink.")
      ->capture_default_str()
      ->check(CLI::Number);
  app.add_option("--nof_dl_threads", config.nof_dl_threads, "Number of upper PHY threads to process downlink.")
      ->capture_default_str()
      ->check(CLI::Number);
}

static void configure_cli11_lower_phy_threads_args(CLI::App& app, lower_phy_threads_appconfig& config)
{
  app.add_option_function<std::string>(
         "--execution_profile",
         [&config](const std::string& value) {
           if (value == "single") {
             config.execution_profile = lower_phy_thread_profile::single;
           } else if (value == "dual") {
             config.execution_profile = lower_phy_thread_profile::dual;
           } else if (value == "quad") {
             config.execution_profile = lower_phy_thread_profile::quad;
           }
         },
         "Lower physical layer executor profile [single, dual, quad].")
      ->check([](const std::string& value) -> std::string {
        if ((value == "single") || (value == "dual") || (value == "quad")) {
          return "";
        }

        return "Invalid executor profile. Valid profiles are: single, dual and quad.";
      });
}

static void configure_cli11_ofh_threads_args(CLI::App& app, ofh_threads_appconfig& config)
{
  app.add_option(
         "--enable_dl_parallelization", config.is_downlink_parallelized, "Open Fronthaul downlink parallelization flag")
      ->capture_default_str();
}

static void configure_cli11_cpu_affinities_args(CLI::App& app, cpu_affinities_appconfig& config)
{
  auto parsing_isolated_cpus_fcn = [](optional<os_sched_affinity_bitmask>& isolated_cpu_cfg,
                                      const std::string&                   value,
                                      const std::string&                   property_name) {
    isolated_cpu_cfg.emplace();
    parse_affinity_mask(*isolated_cpu_cfg, value, property_name);

    if (isolated_cpu_cfg->all()) {
      report_error("Error in '{}' property: can not assign all available CPUs to the gNB app", property_name);
    }
  };

  app.add_option_function<std::string>(
      "--isolated_cpus",
      [&config, &parsing_isolated_cpus_fcn](const std::string& value) {
        parsing_isolated_cpus_fcn(config.isolated_cpus, value, "isolated_cpus");
      },
      "CPU cores isolated for gNB application");

  app.add_option_function<std::string>(
      "--low_priority_cpus",
      [&config](const std::string& value) {
        parse_affinity_mask(config.low_priority_cpu_cfg.mask, value, "low_priority_cpus");
      },
      "CPU cores assigned to low priority tasks");

  app.add_option_function<std::string>(
      "--low_priority_pinning",
      [&config](const std::string& value) {
        config.low_priority_cpu_cfg.pinning_policy = to_affinity_mask_policy(value);
        if (config.low_priority_cpu_cfg.pinning_policy == gnb_sched_affinity_mask_policy::last) {
          report_error("Incorrect value={} used in {} property", value, "low_priority_pinning");
        }
      },
      "Policy used for assigning CPU cores to low priority tasks");
}

static void configure_cli11_expert_execution_args(CLI::App& app, expert_execution_appconfig& config)
{
  // Affinity section.
  CLI::App* affinities_subcmd = app.add_subcommand("affinities", "gNB CPU affinities configuration")->configurable();
  configure_cli11_cpu_affinities_args(*affinities_subcmd, config.affinities);

  // Threads section.
  CLI::App* threads_subcmd = app.add_subcommand("threads", "Threads configuration")->configurable();

  // Non real time threads.
  CLI::App* non_rt_threads_subcmd =
      threads_subcmd->add_subcommand("non_rt", "Non real time thread configuration")->configurable();
  configure_cli11_non_rt_threads_args(*non_rt_threads_subcmd, config.threads.non_rt_threads);

  // Upper PHY threads.
  CLI::App* upper_phy_threads_subcmd =
      threads_subcmd->add_subcommand("upper_phy", "Upper PHY thread configuration")->configurable();
  configure_cli11_upper_phy_threads_args(*upper_phy_threads_subcmd, config.threads.upper_threads);

  // Lower PHY threads.
  CLI::App* lower_phy_threads_subcmd =
      threads_subcmd->add_subcommand("lower_phy", "Lower PHY thread configuration")->configurable();
  configure_cli11_lower_phy_threads_args(*lower_phy_threads_subcmd, config.threads.lower_threads);

  // OFH threads.
  CLI::App* ofh_threads_subcmd =
      threads_subcmd->add_subcommand("ofh", "Open Fronthaul thread configuration")->configurable();
  configure_cli11_ofh_threads_args(*ofh_threads_subcmd, config.threads.ofh_threads);

  // Cell affinity section.
  app.add_option_function<std::vector<std::string>>(
      "--cell_affinities",
      [&config](const std::vector<std::string>& values) {
        if (values.size() > config.cell_affinities.size()) {
          report_error("Number of cells to configure the CPU affinities '{}' is bigger than the number of cells '{}'",
                       values.size(),
                       config.cell_affinities.size());
        }

        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("Expert execution cell CPU affinities");
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras(CLI::config_extras_mode::error);
          configure_cli11_cell_affinity_args(subapp, config.cell_affinities[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Sets the cell CPU affinities configuration on a per cell basis");

  // Callback function for validating that thread affinities do use allowed set of CPUs in case the 'isolated_cpus'
  // option was specified.
  auto validate_isolation = [](CLI::App& cli_app, expert_execution_appconfig& cfg) {
    if (!cli_app.get_subcommand("affinities") || cli_app.get_subcommand("affinities")->count("--isolated_cpus") == 0) {
      cfg.affinities.isolated_cpus.reset();

      return;
    }

    auto validate_cpu_range = [](const os_sched_affinity_bitmask& allowed_cpus_mask,
                                 const os_sched_affinity_bitmask& mask,
                                 const std::string&               name) {
      auto invalid_cpu_ids = mask.subtract(allowed_cpus_mask);
      if (not invalid_cpu_ids.empty()) {
        report_error("CPU cores {} selected in '{}' option doesn't belong to isolated cpuset.", invalid_cpu_ids, name);
      }
    };

    for (const auto& cell : cfg.cell_affinities) {
      validate_cpu_range(*cfg.affinities.isolated_cpus, cell.l1_dl_cpu_cfg.mask, "l1_dl_cpus");
      validate_cpu_range(*cfg.affinities.isolated_cpus, cell.l1_ul_cpu_cfg.mask, "l1_ul_cpus");
      validate_cpu_range(*cfg.affinities.isolated_cpus, cell.l2_cell_cpu_cfg.mask, "l2_cell_cpus");
      validate_cpu_range(*cfg.affinities.isolated_cpus, cell.ru_cpu_cfg.mask, "ru_cpus");
      validate_cpu_range(*cfg.affinities.isolated_cpus, cfg.affinities.low_priority_cpu_cfg.mask, "low_priority_cpus");
    }
  };

  // Post-parsing callback for the case when both manual pinning and isolated_cpus were used.
  app.callback([&]() { validate_isolation(app, config); });
}

static void manage_ru_variant(CLI::App&                 app,
                              gnb_appconfig&            gnb_cfg,
                              const ru_sdr_appconfig    sdr_cfg,
                              const ru_ofh_appconfig&   ofh_cfg,
                              const ru_dummy_appconfig& dummy_cfg)
{
  // Manage the RU optionals
  unsigned nof_ofh_entries   = app.get_subcommand("ru_ofh")->count_all();
  unsigned nof_sdr_entries   = app.get_subcommand("ru_sdr")->count_all();
  unsigned nof_dummy_entries = app.get_subcommand("ru_dummy")->count_all();

  // Count the number of RU types.
  unsigned nof_ru_types = (nof_ofh_entries != 0) ? 1 : 0;
  nof_ru_types += (nof_sdr_entries != 0) ? 1 : 0;
  nof_ru_types += (nof_dummy_entries != 0) ? 1 : 0;

  if (nof_ru_types > 1) {
    srsran_terminate("Radio Unit configuration allows either a SDR, Open Fronthaul, or Dummy configuration, but not "
                     "different types of them at the same time");
  }

  if (nof_ofh_entries != 0) {
    gnb_cfg.ru_cfg = ofh_cfg;
    return;
  }

  if (nof_sdr_entries != 0) {
    gnb_cfg.ru_cfg = sdr_cfg;
    return;
  }

  gnb_cfg.ru_cfg = dummy_cfg;
}

static void manage_hal_optional(CLI::App& app, gnb_appconfig& gnb_cfg)
{
  // Clean the HAL optional.
  if (app.get_subcommand("hal")->count_all() == 0) {
    gnb_cfg.hal_config.reset();
  }
}

static void manage_ntn_optional(CLI::App&             app,
                                gnb_appconfig&        gnb_cfg,
                                epoch_time_t&         epoch_time,
                                orbital_coordinates_t orbital_coordinates,
                                ecef_coordinates_t    ecef_coordinates)
{
  auto     ntn_app             = app.get_subcommand_ptr("ntn");
  unsigned nof_epoch_entries   = ntn_app->get_subcommand("epoch_time")->count_all();
  unsigned nof_ecef_entries    = ntn_app->get_subcommand("ephemeris_info_ecef")->count_all();
  unsigned nof_orbital_entries = ntn_app->get_subcommand("ephemeris_orbital")->count_all();

  if (nof_epoch_entries) {
    gnb_cfg.ntn_cfg.value().epoch_time = epoch_time;
  }

  if (nof_ecef_entries) {
    gnb_cfg.ntn_cfg.value().ephemeris_info = ecef_coordinates;
  } else if (nof_orbital_entries) {
    gnb_cfg.ntn_cfg.value().ephemeris_info = orbital_coordinates;
  }
  if (app.get_subcommand("ntn")->count_all() == 0) {
    gnb_cfg.ntn_cfg.reset();
  }
}

static void manage_expert_execution_threads(CLI::App& app, gnb_appconfig& gnb_cfg)
{
  if (!variant_holds_alternative<ru_sdr_appconfig>(gnb_cfg.ru_cfg)) {
    return;
  }

  // Ignore the default settings based in the number of CPU cores for ZMQ.
  if (variant_get<ru_sdr_appconfig>(gnb_cfg.ru_cfg).device_driver == "zmq") {
    upper_phy_threads_appconfig& upper = gnb_cfg.expert_execution_cfg.threads.upper_threads;
    upper.nof_pusch_decoder_threads    = 0;
    upper.nof_ul_threads               = 1;
    upper.nof_dl_threads               = 1;
    gnb_cfg.expert_execution_cfg.threads.lower_threads.execution_profile = lower_phy_thread_profile::blocking;
  }
}

/// Sets the request headroom size to the max processing delay value if the request headroom property was not parsed,
static void manage_max_request_headroom_size(CLI::App& app, gnb_appconfig& gnb_cfg)
{
  // If max request headroom slots property is present in the config, do nothing.
  CLI::App* expert_cmd = app.get_subcommand("expert_phy");
  if (expert_cmd->count_all() >= 1 && expert_cmd->count("--max_request_headroom_slots") >= 1) {
    return;
  }

  gnb_cfg.expert_phy_cfg.nof_slots_request_headroom = gnb_cfg.expert_phy_cfg.max_processing_delay_slots;
}

static void manage_processing_delay(CLI::App& app, gnb_appconfig& gnb_cfg)
{
  // If max proc delay property is not present in the config, configure the default value.
  CLI::App* expert_cmd = app.get_subcommand("expert_phy");
  if (expert_cmd->count_all() == 0 || expert_cmd->count("--max_proc_delay") == 0) {
    // As processing delay is not cell related, use the first cell to update the value.
    const auto& cell = gnb_cfg.cells_cfg.front().cell;
    nr_band     band = cell.band ? cell.band.value() : band_helper::get_band_from_dl_arfcn(cell.dl_arfcn);

    switch (band_helper::get_duplex_mode(band)) {
      case duplex_mode::TDD:
        gnb_cfg.expert_phy_cfg.max_processing_delay_slots = 5;
        break;
      case duplex_mode::FDD:
        gnb_cfg.expert_phy_cfg.max_processing_delay_slots = 2;
        break;
      default:
        break;
    }
  }

  manage_max_request_headroom_size(app, gnb_cfg);
}

static void configure_cli11_fapi_args(CLI::App& app, fapi_appconfig& config)
{
  app.add_option("--l2_nof_slots_ahead", config.l2_nof_slots_ahead, "Number of slots the L2 is running ahead of the L1")
      ->capture_default_str()
      ->check(CLI::Range(0, 5));
}

void srsran::configure_cli11_with_gnb_appconfig_schema(CLI::App& app, gnb_parsed_appconfig& gnb_parsed_cfg)
{
  gnb_appconfig& gnb_cfg = gnb_parsed_cfg.config;
  app.add_option("--gnb_id", gnb_cfg.gnb_id, "gNodeB identifier")->capture_default_str();
  app.add_option("--gnb_id_bit_length", gnb_cfg.gnb_id_bit_length, "gNodeB identifier length in bits")
      ->capture_default_str()
      ->check(CLI::Range(22, 32));
  app.add_option("--ran_node_name", gnb_cfg.ran_node_name, "RAN node name")->capture_default_str();

  // Logging section.
  CLI::App* log_subcmd = app.add_subcommand("log", "Logging configuration")->configurable();
  configure_cli11_log_args(*log_subcmd, gnb_cfg.log_cfg);

  // PCAP section.
  CLI::App* pcap_subcmd = app.add_subcommand("pcap", "PCAP configuration")->configurable();
  configure_cli11_pcap_args(*pcap_subcmd, gnb_cfg.pcap_cfg);

  // Metrics section.
  CLI::App* metrics_subcmd = app.add_subcommand("metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*metrics_subcmd, gnb_cfg.metrics_cfg);

  // AMF section.
  CLI::App* amf_subcmd = app.add_subcommand("amf", "AMF parameters")->configurable();
  configure_cli11_amf_args(*amf_subcmd, gnb_cfg.amf_cfg);

  // E2 section.
  CLI::App* e2_subcmd = app.add_subcommand("e2", "E2 parameters")->configurable();
  configure_cli11_e2_args(*e2_subcmd, gnb_cfg.e2_cfg);

  // CU-CP section
  CLI::App* cu_cp_subcmd = app.add_subcommand("cu_cp", "CU-CP parameters")->configurable();
  configure_cli11_cu_cp_args(*cu_cp_subcmd, gnb_cfg.cu_cp_cfg);

  // CU-UP section.
  CLI::App* cu_up_subcmd = app.add_subcommand("cu_up", "CU-CP parameters")->configurable();
  configure_cli11_cu_up_args(*cu_up_subcmd, gnb_cfg.cu_up_cfg);

  // NTN section.
  CLI::App*                    ntn_subcmd = app.add_subcommand("ntn", "NTN parameters")->configurable();
  static epoch_time_t          epoch_time;
  static ecef_coordinates_t    ecef_coordinates;
  static orbital_coordinates_t orbital_coordinates;
  configure_cli11_ntn_args(*ntn_subcmd, gnb_cfg.ntn_cfg, epoch_time, orbital_coordinates, ecef_coordinates);

  // NOTE: CLI11 needs that the life of the variable lasts longer than the call of this function. As both options need
  // to be added and a variant is used to store the Radio Unit configuration, the configuration is parsed in a helper
  // variable, but as it is requested later, the variable needs to be static.
  // RU section.
  static ru_ofh_appconfig ofh_cfg;
  CLI::App* ru_ofh_subcmd = app.add_subcommand("ru_ofh", "Open Fronthaul Radio Unit configuration")->configurable();
  configure_cli11_ru_ofh_args(*ru_ofh_subcmd, ofh_cfg);

  static ru_sdr_appconfig sdr_cfg;
  CLI::App*               ru_sdr_subcmd = app.add_subcommand("ru_sdr", "SDR Radio Unit configuration")->configurable();
  configure_cli11_ru_sdr_args(*ru_sdr_subcmd, sdr_cfg);

  static ru_dummy_appconfig dummy_cfg;
  CLI::App* ru_dummy_subcmd = app.add_subcommand("ru_dummy", "Dummy Radio Unit configuration")->configurable();
  configure_cli11_ru_dummy_args(*ru_dummy_subcmd, dummy_cfg);

  // FAPI section.
  CLI::App* fapi_subcmd = app.add_subcommand("fapi", "FAPI configuration")->configurable();
  configure_cli11_fapi_args(*fapi_subcmd, gnb_cfg.fapi_cfg);

  // Common cell section.
  CLI::App* common_cell_subcmd = app.add_subcommand("cell_cfg", "Default cell configuration")->configurable();
  configure_cli11_common_cell_args(*common_cell_subcmd, gnb_parsed_cfg.common_cell_cfg);
  // Configure the cells to use the common cell parameters once it has been parsed and before parsing the cells.
  common_cell_subcmd->parse_complete_callback([&gnb_parsed_cfg, &app]() {
    for (auto& cell : gnb_parsed_cfg.config.cells_cfg) {
      cell.cell = gnb_parsed_cfg.common_cell_cfg;
    };
    // Run the callback again for the cells if the option callback is already run once.
    if (app.get_option("--cells")->get_callback_run()) {
      app.get_option("--cells")->run_callback();
    }
  });

  // Cell section.
  app.add_option_function<std::vector<std::string>>(
      "--cells",
      [&gnb_parsed_cfg](const std::vector<std::string>& values) {
        // Resize the number of cells that controls the CPU affinites.
        gnb_parsed_cfg.config.expert_execution_cfg.cell_affinities.resize(values.size());
        // Prepare the cells from the common cell.
        gnb_parsed_cfg.config.cells_cfg.resize(values.size());
        for (auto& cell : gnb_parsed_cfg.config.cells_cfg) {
          cell.cell = gnb_parsed_cfg.common_cell_cfg;
        }

        // Format every cell.
        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("srsGNB application");
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras(CLI::config_extras_mode::error);
          configure_cli11_cells_args(subapp, gnb_parsed_cfg.config.cells_cfg[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Sets the cell configuration on a per cell basis, overwriting the default configuration defined by cell_cfg");

  // QoS section.
  auto qos_lambda = [&gnb_cfg](const std::vector<std::string>& values) {
    // Prepare the radio bearers
    gnb_cfg.qos_cfg.resize(values.size());

    // Format every QoS setting.
    for (unsigned i = 0, e = values.size(); i != e; ++i) {
      CLI::App subapp("QoS parameters");
      subapp.config_formatter(create_yaml_config_parser());
      subapp.allow_config_extras(CLI::config_extras_mode::error);
      configure_cli11_qos_args(subapp, gnb_cfg.qos_cfg[i]);
      std::istringstream ss(values[i]);
      subapp.parse_from_stream(ss);
    }
  };
  app.add_option_function<std::vector<std::string>>(
      "--qos", qos_lambda, "Configures RLC and PDCP radio bearers on a per 5QI basis.");

  // SRB section.
  auto srb_lambda = [&gnb_cfg](const std::vector<std::string>& values) {
    // Format every SRB setting.
    for (unsigned i = 0, e = values.size(); i != e; ++i) {
      CLI::App subapp("SRB parameters");
      subapp.config_formatter(create_yaml_config_parser());
      subapp.allow_config_extras(CLI::config_extras_mode::error);
      srb_appconfig srb_cfg;
      configure_cli11_srb_args(subapp, srb_cfg);
      std::istringstream ss(values[i]);
      subapp.parse_from_stream(ss);
      gnb_cfg.srb_cfg[static_cast<srb_id_t>(srb_cfg.srb_id)] = srb_cfg;
    }
  };
  app.add_option_function<std::vector<std::string>>("--srbs", srb_lambda, "Configures signaling radio bearers.");

  // Slicing section.
  auto slicing_lambda = [&gnb_cfg](const std::vector<std::string>& values) {
    // Prepare the radio bearers
    gnb_cfg.slice_cfg.resize(values.size());

    // Format every QoS setting.
    for (unsigned i = 0, e = values.size(); i != e; ++i) {
      CLI::App subapp("Slicing parameters");
      subapp.config_formatter(create_yaml_config_parser());
      subapp.allow_config_extras(CLI::config_extras_mode::error);
      configure_cli11_slicing_args(subapp, gnb_cfg.slice_cfg[i]);
      std::istringstream ss(values[i]);
      subapp.parse_from_stream(ss);
    }
  };
  app.add_option_function<std::vector<std::string>>("--slicing", slicing_lambda, "Network slicing configuration");

  // Expert PHY section.
  CLI::App* expert_phy_subcmd = app.add_subcommand("expert_phy", "Expert physical layer configuration")->configurable();
  configure_cli11_expert_phy_args(*expert_phy_subcmd, gnb_cfg.expert_phy_cfg);

  // Buffer pool section.
  CLI::App* buffer_pool_subcmd = app.add_subcommand("buffer_pool", "Buffer pool configuration")->configurable();
  configure_cli11_buffer_pool_args(*buffer_pool_subcmd, gnb_cfg.buffer_pool_config);

  // Test mode section.
  CLI::App* test_mode_subcmd = app.add_subcommand("test_mode", "Test mode configuration")->configurable();
  configure_cli11_test_mode_args(*test_mode_subcmd, gnb_cfg.test_mode_cfg);

  // Expert section.
  CLI::App* expert_subcmd = app.add_subcommand("expert_execution", "Expert execution configuration")->configurable();
  configure_cli11_expert_execution_args(*expert_subcmd, gnb_cfg.expert_execution_cfg);

  // HAL section.
  CLI::App* hal_subcmd = app.add_subcommand("hal", "HAL configuration")->configurable();
  configure_cli11_hal_args(*hal_subcmd, gnb_cfg.hal_config);

  app.callback([&]() {
    manage_ru_variant(app, gnb_cfg, sdr_cfg, ofh_cfg, dummy_cfg);
    manage_hal_optional(app, gnb_cfg);
    manage_ntn_optional(app, gnb_cfg, epoch_time, orbital_coordinates, ecef_coordinates);
    manage_processing_delay(app, gnb_cfg);
    manage_expert_execution_threads(app, gnb_cfg);
  });
}
