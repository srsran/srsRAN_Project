/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_unit_config_cli11_schema.h"
#include "cu_cp_unit_config.h"
#include "srsran/support/cli11_utils.h"
#include "srsran/support/config_parsers.h"

using namespace srsran;

static void configure_cli11_report_args(CLI::App& app, cu_cp_report_unit_config& report_params)
{
  app.add_option("--report_cfg_id", report_params.report_cfg_id, "Report configuration id to be configured")
      ->check(CLI::Range(1, 64));
  app.add_option("--report_type", report_params.report_type, "Type of the report configuration")
      ->check(CLI::IsMember({"periodical", "event_triggered"}));
  app.add_option("--report_interval_ms", report_params.report_interval_ms, "Report interval in ms")
      ->check(
          CLI::IsMember({120, 240, 480, 640, 1024, 2048, 5120, 10240, 20480, 40960, 60000, 360000, 720000, 1800000}));
  app.add_option("--a3_report_type", report_params.a3_report_type, "A3 report type")
      ->check(CLI::IsMember({"rsrp", "rsrq", "sinr"}));
  app.add_option("--a3_offset_db",
                 report_params.a3_offset_db,
                 "A3 offset in dB used for measurement report trigger. Note the actual value is field value * 0.5 dB")
      ->check(CLI::Range(-30, 30));
  app.add_option(
         "--a3_hysteresis_db",
         report_params.a3_hysteresis_db,
         "A3 hysteresis in dB used for measurement report trigger. Note the actual value is field value * 0.5 dB")
      ->check(CLI::Range(0, 30));
  app.add_option("--a3_time_to_trigger_ms",
                 report_params.a3_time_to_trigger_ms,
                 "Time in ms during which A3 condition must be met before measurement report trigger")
      ->check(CLI::IsMember({0, 40, 64, 80, 100, 128, 160, 256, 320, 480, 512, 640, 1024, 1280, 2560, 5120}));
}

static void configure_cli11_ncell_args(CLI::App& app, cu_cp_neighbor_cell_unit_config_item& config)
{
  app.add_option("--nr_cell_id", config.nr_cell_id, "Neighbor cell id");
  app.add_option(
      "--report_configs", config.report_cfg_ids, "Report configurations to configure for this neighbor cell");
}

static void configure_cli11_cells_args(CLI::App& app, cu_cp_cell_unit_config_item& config)
{
  app.add_option("--nr_cell_id", config.nr_cell_id, "Cell id to be configured");
  app.add_option("--periodic_report_cfg_id",
                 config.periodic_report_cfg_id,
                 "Periodical report configuration for the serving cell")
      ->check(CLI::Range(1, 64));
  add_auto_enum_option(app, "--band", config.band, "NR frequency band");
  app.add_option("--gnb_id_bit_length", config.gnb_id_bit_length, "gNodeB identifier bit length")
      ->check(CLI::Range(22, 32));
  app.add_option("--pci", config.pci, "Physical Cell Id")->check(CLI::Range(0, 1007));
  app.add_option("--ssb_arfcn", config.ssb_arfcn, "SSB ARFCN");
  app.add_option("--ssb_scs", config.ssb_scs, "SSB subcarrier spacing")->check(CLI::IsMember({15, 30, 60, 120, 240}));
  app.add_option("--ssb_period", config.ssb_period, "SSB period in ms")->check(CLI::IsMember({5, 10, 20, 40, 80, 160}));
  app.add_option("--ssb_offset", config.ssb_offset, "SSB offset");
  app.add_option("--ssb_duration", config.ssb_duration, "SSB duration")->check(CLI::IsMember({1, 2, 3, 4, 5}));

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

static void configure_cli11_mobility_args(CLI::App& app, mobility_unit_config& config)
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

static void configure_cli11_security_args(CLI::App& app, security_unit_config& config)
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

static void configure_cli11_f1ap_args(CLI::App& app, f1ap_cu_unit_config& f1ap_params)
{
  app.add_option(
         "--ue_context_setup_timeout", f1ap_params.ue_context_setup_timeout, "UE context setup timeout in milliseconds")
      ->capture_default_str();
}

static void configure_cli11_cu_cp_args(CLI::App& app, cu_cp_unit_config& cu_cp_params)
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

  CLI::App* f1ap_subcmd = app.add_subcommand("f1ap", "F1AP configuration");
  configure_cli11_f1ap_args(*f1ap_subcmd, cu_cp_params.f1ap_config);
}

static void configure_cli11_log_args(CLI::App& app, cu_cp_log_unit_config& log_params)
{
  auto level_check = [](const std::string& value) -> std::string {
    if (value == "info" || value == "debug" || value == "warning" || value == "error") {
      return {};
    }
    return "Log level value not supported. Accepted values [info,debug,warning,error]";
  };

  app.add_option("--pdcp_level", log_params.pdcp_level, "PDCP log level")->capture_default_str()->check(level_check);
  app.add_option("--rrc_level", log_params.rrc_level, "RRC log level")->capture_default_str()->check(level_check);
  app.add_option("--ngap_level", log_params.ngap_level, "NGAP log level")->capture_default_str()->check(level_check);
  app.add_option("--f1ap_level", log_params.f1ap_level, "F1AP log level")->capture_default_str()->check(level_check);
  app.add_option("--cu_level", log_params.cu_level, "Log level for the CU")->capture_default_str()->check(level_check);
  app.add_option("--sec_level", log_params.sec_level, "Security functions log level")
      ->capture_default_str()
      ->check(level_check);

  app.add_option(
         "--hex_max_size", log_params.hex_max_size, "Maximum number of bytes to print in hex (zero for no hex dumps)")
      ->capture_default_str()
      ->check(CLI::Range(0, 1024));

  app.add_option("--f1ap_json_enabled", log_params.f1ap_json_enabled, "Enable JSON logging of F1AP PDUs")
      ->always_capture_default();
}

void configure_cli11_with_cu_cp_unit_config_schema(CLI::App& app, cu_cp_unit_config& parsed_cfg)
{
  // CU-CP section
  CLI::App* cu_cp_subcmd = app.add_subcommand("cu_cp", "CU-CP parameters")->configurable();
  configure_cli11_cu_cp_args(*cu_cp_subcmd, parsed_cfg);

  // Loggers section.
  CLI::App* log_subcmd = app.get_subcommand("log");
  if (!log_subcmd) {
    log_subcmd = app.add_subcommand("log", "Logging configuration")->configurable();
  }

  configure_cli11_log_args(*log_subcmd, parsed_cfg.loggers);
}
