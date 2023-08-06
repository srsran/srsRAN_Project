/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/ran/pdsch/pdsch_mcs.h"
#include "srsran/support/cli11_utils.h"
#include "srsran/support/config_parsers.h"
#include "srsran/support/error_handling.h"
#include "CLI/CLI11.hpp"

using namespace srsran;

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
                 "Set to a valid file path to print the received symbols")
      ->always_capture_default();
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
  app.add_option("--mac_filename", pcap_params.mac.filename, "MAC PCAP file output path")->capture_default_str();
  app.add_option("--mac_enable", pcap_params.mac.enabled, "Enable MAC packet capture")->always_capture_default();
  app.add_option("--e2ap_filename", pcap_params.e2ap.filename, "E2AP PCAP file output path")->capture_default_str();
  app.add_option("--e2ap_enable", pcap_params.e2ap.enabled, "Enable E2AP packet capture")->always_capture_default();
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
  app.add_option("--bind_addr", amf_params.bind_addr, "Local IP address to bind for AMF connection")
      ->check(CLI::ValidIPV4);
  app.add_option("--sctp_rto_initial", amf_params.sctp_rto_initial, "SCTP initial RTO value");
  app.add_option("--sctp_rto_min", amf_params.sctp_rto_min, "SCTP RTO min");
  app.add_option("--sctp_rto_max", amf_params.sctp_rto_max, "SCTP RTO max");
  app.add_option("--sctp_init_max_attempts", amf_params.sctp_init_max_attempts, "SCTP init max attempts");
  app.add_option("--sctp_max_init_timeo", amf_params.sctp_max_init_timeo, "SCTP max init timeout");
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
}

static void configure_cli11_cu_cp_args(CLI::App& app, cu_cp_appconfig& cu_cp_params)
{
  app.add_option("--inactivity_timer", cu_cp_params.inactivity_timer, "UE/PDU Session/DRB inactivity timer in seconds")
      ->capture_default_str()
      ->check(CLI::Range(1, 7200));

  CLI::App* mobility_subcmd = app.add_subcommand("mobility", "Mobility configuration");
  configure_cli11_mobility_args(*mobility_subcmd, cu_cp_params.mobility_config);

  CLI::App* rrc_subcmd = app.add_subcommand("rrc", "RRC specific configuration");
  configure_cli11_rrc_args(*rrc_subcmd, cu_cp_params.rrc_config);
}

static void configure_cli11_expert_phy_args(CLI::App& app, expert_upper_phy_appconfig& expert_phy_params)
{
  app.add_option("--max_proc_delay",
                 expert_phy_params.max_processing_delay_slots,
                 "Maximum allowed DL processing delay in slots.")
      ->capture_default_str()
      ->check(CLI::Range(1, 30));
  app.add_option("--nof_pdsch_threads", expert_phy_params.nof_pdsch_threads, "Number of threads to encode PDSCH.")
      ->capture_default_str()
      ->check(CLI::Number);
  app.add_option("--nof_ul_threads", expert_phy_params.nof_ul_threads, "Number of upper PHY threads to process uplink.")
      ->capture_default_str()
      ->check(CLI::Number);
  app.add_option(
         "--nof_dl_threads", expert_phy_params.nof_dl_threads, "Number of upper PHY threads to process downlink.")
      ->capture_default_str()
      ->check(CLI::Number);
  app.add_option("--pusch_dec_max_iterations",
                 expert_phy_params.pusch_decoder_max_iterations,
                 "Maximum number of PUSCH LDPC decoder iterations")
      ->capture_default_str()
      ->check(CLI::Number);
  app.add_option("--pusch_dec_enable_early_stop",
                 expert_phy_params.pusch_decoder_early_stop,
                 "Enables PUSCH LDPC decoder early stop")
      ->capture_default_str();
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
  app.add_option("--nof_ports",
                 pdsch_params.nof_ports,
                 "Number of ports for PDSCH. By default it is set to be equal to number of DL antennas")
      ->capture_default_str()
      ->check(CLI::IsMember({1, 2, 4}));
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
      ->check(CLI::Range(1, 10));
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
      ->check(CLI::Range(1, 10));
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
}

static void configure_cli11_ul_common_args(CLI::App& app, ul_common_appconfig& ul_common_params)
{
  app.add_option("--p_max", ul_common_params.p_max, "Maximum transmit power allowed in this serving cell")
      ->capture_default_str()
      ->check(CLI::Range(-30, 23));
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

static void configure_cli11_prach_args(CLI::App& app, prach_appconfig& prach_params)
{
  app.add_option("--prach_config_index", prach_params.prach_config_index, "PRACH configuration index")
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
  app.add_option("--pwr_ctrl_offset",
                 csi_params.pwr_ctrl_offset,
                 "powerControlOffset, Power offset of PDSCH RE to NZP CSI-RS RE in dB")
      ->capture_default_str()
      ->check(CLI::Range(-8, 15));
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

  // SSB configuration.
  CLI::App* ssb_subcmd = app.add_subcommand("ssb", "SSB parameters");
  configure_cli11_ssb_args(*ssb_subcmd, cell_params.ssb_cfg);

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

  // CSI configuration;
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
  CLI::App* rlc_rx_am_subcmd = app.add_subcommand("rx", "AM RX parameters");
  rlc_rx_am_subcmd->add_option("--sn", rlc_am_params.rx.sn_field_length, "RLC AM RX SN")->capture_default_str();
  rlc_rx_am_subcmd->add_option("--t-reassembly", rlc_am_params.rx.t_reassembly, "RLC AM RX t-Reassembly")
      ->capture_default_str();
  rlc_rx_am_subcmd->add_option("--t-status-prohibit", rlc_am_params.rx.t_status_prohibit, "RLC AM RX t-StatusProhibit")
      ->capture_default_str();
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
  auto verify_callback = [&]() {
    CLI::App* rlc       = app.get_subcommand("rlc");
    CLI::App* f1u_du    = app.get_subcommand("f1u_du");
    CLI::App* f1u_cu_up = app.get_subcommand("f1u_cu_up");
    CLI::App* pdcp      = app.get_subcommand("pdcp");
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
  };
  app.callback(verify_callback);
}

static void configure_cli11_test_ue_mode_args(CLI::App& app, test_mode_ue_appconfig& test_params)
{
  app.add_option("--rnti", test_params.rnti, "C-RNTI (0x0 if not configured)")
      ->capture_default_str()
      ->check(CLI::Range(INVALID_RNTI, MAX_CRNTI));
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

static void configure_cli11_ru_sdr_cells_args(CLI::App& app, ru_sdr_cell_appconfig& config)
{
  // Amplitude control configuration.
  CLI::App* amplitude_control_subcmd = app.add_subcommand("amplitude_control", "Amplitude control parameters");
  configure_cli11_amplitude_control_args(*amplitude_control_subcmd, config.amplitude_cfg);
}

static void configure_cli11_ru_sdr_expert_args(CLI::App& app, ru_sdr_expert_appconfig& config)
{
  app.add_option("--low_phy_dl_throttling",
                 config.lphy_dl_throttling,
                 "Throttles the lower PHY DL baseband generation. The range is (0, 1). Set it to zero to disable it.")
      ->capture_default_str();

  app.add_option_function<std::string>(
         "--low_phy_thread_profile",
         [&config](const std::string& value) {
           if (value == "single") {
             config.lphy_executor_profile = lower_phy_thread_profile::single;
           } else if (value == "dual") {
             config.lphy_executor_profile = lower_phy_thread_profile::dual;
           } else if (value == "quad") {
             config.lphy_executor_profile = lower_phy_thread_profile::quad;
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

  // Expert configuration.
  CLI::App* expert_subcmd = app.add_subcommand("expert_cfg", "Generic Radio Unit expert configuration");
  configure_cli11_ru_sdr_expert_args(*expert_subcmd, config.expert_cfg);

  // Cell parameters.
  app.add_option_function<std::vector<std::string>>(
      "--cells",
      [&config](const std::vector<std::string>& values) {
        config.cells.resize(values.size());

        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("RU SDR cells");
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras(CLI::config_extras_mode::error);
          configure_cli11_ru_sdr_cells_args(subapp, config.cells[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Sets the cell configuration on a per cell basis, overwriting the default configuration defined by cell_cfg");
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
  app.add_option(
         "--ignore_ecpri_payload_size", config.ignore_ecpri_payload_size_field, "Ignore eCPRI payload size field value")
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
  app.add_option("--compr_bitwidth_ul", config.compresion_bitwidth_ul, "Uplink compression bit width")
      ->capture_default_str()
      ->check(CLI::Range(1, 16));
  app.add_option("--compr_method_dl", config.compression_method_dl, "Downlink compression method")
      ->capture_default_str()
      ->check(compression_method_check);
  app.add_option("--compr_bitwidth_dl", config.compresion_bitwidth_dl, "Downlink compression bit width")
      ->capture_default_str()
      ->check(CLI::Range(1, 16));
  app.add_option("--compr_method_prach", config.compression_method_prach, "PRACH compression method")
      ->capture_default_str()
      ->check(compression_method_check);
  app.add_option("--compr_bitwidth_prach", config.compresion_bitwidth_prach, "PRACH compression bit width")
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
      ->check(CLI::Range(0.0, 5.0));

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

  // Post-parsing callback to configure PRACH compression parameters.
  app.callback([&]() {
    // First validate that compression method and bitwidth parameters were both specified or both set to default.
    validate_compression_input(app, "dl");
    validate_compression_input(app, "ul");
    validate_compression_input(app, "prach");

    if (app.count("--compr_method_prach") > 0) {
      return;
    }
    config.compression_method_prach  = config.compression_method_ul;
    config.compresion_bitwidth_prach = config.compresion_bitwidth_ul;
  });
}

static void configure_cli11_ru_ofh_cells_args(CLI::App& app, ru_ofh_cell_appconfig& config)
{
  configure_cli11_ru_ofh_base_cell_args(app, config.cell);
  app.add_option("--network_interface", config.network_interface, "Network interface")->capture_default_str();
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
  app.add_option(
         "--enable_dl_parallelization", config.is_downlink_parallelized, "Open Fronthaul downlink parallelization flag")
      ->capture_default_str();

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

static void parse_ru_config(CLI::App& app, gnb_appconfig& config)
{
  // NOTE: CLI11 needs that the life of the variable last longer than the call of this function. As both options need
  // to be added and a variant is used to store the Radio Unit configuration, the configuration is parsed in a helper
  // variable, but as it is requested later, the variable needs to be static.
  static ru_sdr_appconfig sdr_cfg;
  CLI::App*               ru_sdr_subcmd = app.add_subcommand("ru_sdr", "SDR Radio Unit configuration")->configurable();
  configure_cli11_ru_sdr_args(*ru_sdr_subcmd, sdr_cfg);

  // Check the above note.
  static ru_ofh_appconfig ofh_cfg;
  CLI::App* ru_ofh_subcmd = app.add_subcommand("ru_ofh", "Open Fronthaul Radio Unit configuration")->configurable();
  configure_cli11_ru_ofh_args(*ru_ofh_subcmd, ofh_cfg);

  // Check which Radio Unit configuration was present and update the configuration file.
  auto ru_ofh_verify_callback = [&]() {
    unsigned nof_ofh_entries = app.get_subcommand("ru_ofh")->count_all();
    unsigned nof_sdr_entries = app.get_subcommand("ru_sdr")->count_all();

    if (nof_sdr_entries && nof_ofh_entries) {
      srsran_terminate("Radio Unit configuration allows either a SDR or Open Fronthaul configuration, but not both "
                       "of them at the same time");
    }

    if (nof_ofh_entries != 0) {
      config.ru_cfg = ofh_cfg;

      return;
    }

    config.ru_cfg = sdr_cfg;
  };

  app.callback(ru_ofh_verify_callback);
}

static void parse_buffer_pool_config(CLI::App& app, buffer_pool_appconfig& config)
{
  app.add_option("--nof_segments", config.nof_segments, "Number of segments allocated by the buffer pool")
      ->capture_default_str();
  app.add_option("--segment_size", config.segment_size, "Size of each buffer pool segment in bytes")
      ->capture_default_str();
}

static void parse_expert_config(CLI::App& app, expert_appconfig& config)
{
  app.add_option("--enable_tuned_affinity_profile",
                 config.enable_tuned_affinity_profile,
                 "Enable usage of tuned affinity profile")
      ->capture_default_str();
  app.add_option("--number_of_threads_per_cpu", config.nof_threads_per_cpu, "Number of threads per physical CPU")
      ->capture_default_str()
      ->check(CLI::Range(1, 2));
  app.add_option("--number_of_reserved_cores",
                 config.nof_cores_for_non_prio_workers,
                 "Number of CPU cores reserved for non-priority tasks")
      ->capture_default_str()
      ->check(CLI::Range(0, 1024));
}

void srsran::configure_cli11_with_gnb_appconfig_schema(CLI::App& app, gnb_appconfig& gnb_cfg)
{
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

  // AMF section.
  CLI::App* amf_subcmd = app.add_subcommand("amf", "AMF parameters")->configurable();
  configure_cli11_amf_args(*amf_subcmd, gnb_cfg.amf_cfg);

  // E2 section.
  CLI::App* e2_subcmd = app.add_subcommand("e2", "E2 parameters")->configurable();
  configure_cli11_e2_args(*e2_subcmd, gnb_cfg.e2_cfg);

  // CU-CP section
  CLI::App* cu_cp_subcmd = app.add_subcommand("cu_cp", "CU-CP parameters")->configurable();
  configure_cli11_cu_cp_args(*cu_cp_subcmd, gnb_cfg.cu_cp_cfg);

  // RU section.
  parse_ru_config(app, gnb_cfg);

  // Common cell parameters.
  CLI::App* common_cell_subcmd = app.add_subcommand("cell_cfg", "Default cell configuration")->configurable();
  configure_cli11_common_cell_args(*common_cell_subcmd, gnb_cfg.common_cell_cfg);
  // Configure the cells to use the common cell parameters once it has been parsed and before parsing the cells.
  common_cell_subcmd->parse_complete_callback([&gnb_cfg]() {
    for (auto& cell : gnb_cfg.cells_cfg) {
      cell.cell = gnb_cfg.common_cell_cfg;
    };
  });

  // Cell parameters.
  app.add_option_function<std::vector<std::string>>(
      "--cells",
      [&gnb_cfg](const std::vector<std::string>& values) {
        // Prepare the cells from the common cell.
        if (values.size() > gnb_cfg.cells_cfg.size()) {
          gnb_cfg.cells_cfg.resize(values.size());
          for (auto& cell : gnb_cfg.cells_cfg) {
            cell.cell = gnb_cfg.common_cell_cfg;
          }
        }

        // Format every cell.
        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("srsGNB application");
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras(CLI::config_extras_mode::error);
          configure_cli11_cells_args(subapp, gnb_cfg.cells_cfg[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Sets the cell configuration on a per cell basis, overwriting the default configuration defined by cell_cfg");

  // QoS parameters.
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

  // Slicing parameters.
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
  parse_buffer_pool_config(*buffer_pool_subcmd, gnb_cfg.buffer_pool_config);

  // Test mode section.
  CLI::App* test_mode_subcmd = app.add_subcommand("test_mode", "Test mode configuration")->configurable();
  configure_cli11_test_mode_args(*test_mode_subcmd, gnb_cfg.test_mode_cfg);

  // Expert section.
  CLI::App* expert_subcmd = app.add_subcommand("expert", "Expert configuration")->configurable();
  parse_expert_config(*expert_subcmd, gnb_cfg.expert_config);
}
