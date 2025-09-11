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

#include "cu_cp_unit_config_cli11_schema.h"
#include "apps/helpers/logger/logger_appconfig_cli11_utils.h"
#include "apps/helpers/metrics/metrics_config_cli11_schema.h"
#include "cu_cp_unit_config.h"
#include "srsran/ran/nr_cell_identity.h"
#include "srsran/support/cli11_utils.h"
#include "srsran/support/config_parsers.h"

using namespace srsran;

static void configure_cli11_log_args(CLI::App& app, cu_cp_unit_logger_config& log_params)
{
  app_helpers::add_log_option(app, log_params.pdcp_level, "--pdcp_level", "PDCP log level");
  app_helpers::add_log_option(app, log_params.rrc_level, "--rrc_level", "RRC log level");
  app_helpers::add_log_option(app, log_params.ngap_level, "--ngap_level", "NGAP log level");
  app_helpers::add_log_option(app, log_params.nrppa_level, "--nrppa_level", "NRPPA log level")->group("");
  app_helpers::add_log_option(app, log_params.e1ap_level, "--e1ap_level", "E1AP log level");
  app_helpers::add_log_option(app, log_params.f1ap_level, "--f1ap_level", "F1AP log level");
  app_helpers::add_log_option(app, log_params.cu_level, "--cu_level", "Log level for the CU");
  app_helpers::add_log_option(app, log_params.sec_level, "--sec_level", "Security functions log level");

  add_option(
      app, "--hex_max_size", log_params.hex_max_size, "Maximum number of bytes to print in hex (zero for no hex dumps)")
      ->capture_default_str()
      ->check(CLI::Range(0, 1024));

  add_option(app, "--e1ap_json_enabled", log_params.e1ap_json_enabled, "Enable JSON logging of E1AP PDUs")
      ->always_capture_default();
  add_option(app, "--f1ap_json_enabled", log_params.f1ap_json_enabled, "Enable JSON logging of F1AP PDUs")
      ->always_capture_default();
}

static void configure_cli11_pcap_args(CLI::App& app, cu_cp_unit_pcap_config& pcap_params)
{
  add_option(app, "--ngap_filename", pcap_params.ngap.filename, "N3 GTP-U PCAP file output path")
      ->capture_default_str();
  add_option(app, "--ngap_enable", pcap_params.ngap.enabled, "Enable N3 GTP-U packet capture")
      ->always_capture_default();
  add_option(app, "--f1ap_filename", pcap_params.f1ap.filename, "F1AP PCAP file output path")->capture_default_str();
  add_option(app, "--f1ap_enable", pcap_params.f1ap.enabled, "Enable F1AP packet capture")->always_capture_default();
  add_option(app, "--e1ap_filename", pcap_params.e1ap.filename, "E1AP PCAP file output path")->capture_default_str();
  add_option(app, "--e1ap_enable", pcap_params.e1ap.enabled, "Enable E1AP packet capture")->always_capture_default();
}

static void configure_cli11_tai_slice_support_args(CLI::App& app, cu_cp_unit_plmn_item::tai_slice_t& config)
{
  add_option(app, "--sst", config.sst, "Slice Service Type")->capture_default_str()->check(CLI::Range(0, 255));
  add_option(app, "--sd", config.sd, "Service Differentiator")->capture_default_str()->check(CLI::Range(0, 0xffffff));
}

static void configure_cli11_plmn_item_args(CLI::App& app, cu_cp_unit_plmn_item& config)
{
  add_option(app, "--plmn", config.plmn_id, "PLMN to be configured");

  // TAI slice support list.
  app.add_option_function<std::vector<std::string>>(
      "--tai_slice_support_list",
      [&config](const std::vector<std::string>& values) {
        config.tai_slice_support_list.resize(values.size());

        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("TAI slice support list");
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras(CLI::config_extras_mode::error);
          configure_cli11_tai_slice_support_args(subapp, config.tai_slice_support_list[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Sets the list of TAI slices for this PLMN");
}

static void configure_cli11_supported_ta_args(CLI::App& app, cu_cp_unit_supported_ta_item& config)
{
  add_option(app, "--tac", config.tac, "TAC to be configured")->check([](const std::string& value) {
    std::stringstream ss(value);
    unsigned          tac;
    ss >> tac;

    // Values 0 and 0xfffffe are reserved.
    if (tac == 0U || tac == 0xfffffeU) {
      return "TAC values 0 or 0xfffffe are reserved";
    }

    return (tac <= 0xffffffU) ? "" : "TAC value out of range";
  });

  // PLMN item list.
  app.add_option_function<std::vector<std::string>>(
      "--plmn_list",
      [&config](const std::vector<std::string>& values) {
        config.plmn_list.resize(values.size());

        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("PLMN item list");
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras(CLI::config_extras_mode::error);
          configure_cli11_plmn_item_args(subapp, config.plmn_list[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Sets the list of PLMN items for this tracking area");
}

static void configure_cli11_amf_item_args(CLI::App& app, cu_cp_unit_amf_config_item& config)
{
  add_option(app, "--addr", config.ip_addr, "AMF IP address");
  add_option(app, "--port", config.port, "AMF port")->capture_default_str()->check(CLI::Range(20000, 40000));
  add_option(app, "--bind_addr", config.bind_addr, "Local IP address to bind for N2 interface")->check(CLI::ValidIPV4);
  add_option(app, "--bind_interface", config.bind_interface, "Network device to bind for N2 interface")
      ->capture_default_str();
  add_option(app,
             "--sctp_rto_initial",
             config.sctp_rto_initial_ms,
             "SCTP initial RTO value in milliseconds (-1 to use system default)");
  add_option(app, "--sctp_rto_min", config.sctp_rto_min_ms, "SCTP RTO min in milliseconds (-1 to use system default)");
  add_option(app, "--sctp_rto_max", config.sctp_rto_max_ms, "SCTP RTO max in milliseconds (-1 to use system default)");
  add_option(app,
             "--sctp_init_max_attempts",
             config.sctp_init_max_attempts,
             "SCTP init max attempts (-1 to use system default)");
  add_option(app,
             "--sctp_max_init_timeo",
             config.sctp_max_init_timeo_ms,
             "SCTP max init timeout in milliseconds (-1 to use system default)");
  add_option(app,
             "--sctp_hb_interval",
             config.sctp_hb_interval_ms,
             "SCTP heartbeat interval in milliseconds (-1 to use system default)")
      ->capture_default_str();
  add_option(app,
             "--sctp_assoc_max_retx",
             config.sctp_assoc_max_retx,
             "SCTP assocination max retransmissions (-1 to use system default)")
      ->capture_default_str();
  add_option(app,
             "--sctp_nodelay",
             config.sctp_nodelay,
             "Send SCTP messages as soon as possible without any Nagle-like algorithm");

  // Supported tracking areas configuration parameters.
  app.add_option_function<std::vector<std::string>>(
      "--supported_tracking_areas",
      [&config](const std::vector<std::string>& values) {
        // If supported tracking areas are configured clear default values.
        config.supported_tas.clear();
        config.is_default_supported_tas = false;
        config.supported_tas.resize(values.size());

        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("Supported tracking areas of AMF");
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras(CLI::config_extras_mode::error);
          configure_cli11_supported_ta_args(subapp, config.supported_tas[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Sets the list of tracking areas supported by this AMF");
}

static void configure_cli11_amf_args(CLI::App& app, cu_cp_unit_amf_config& config)
{
  add_option(app, "--no_core", config.no_core, "Allow CU-CP to run without a core")->capture_default_str();
  add_option(app,
             "--amf_reconnection_retry_time",
             config.amf_reconnection_retry_time,
             "Time to wait after a failed AMF reconnection attempt in ms")
      ->capture_default_str();
  // AMF parameters.
  configure_cli11_amf_item_args(app, config.amf);
}

static void configure_cli11_report_args(CLI::App& app, cu_cp_unit_report_config& report_params)
{
  add_option(app, "--report_cfg_id", report_params.report_cfg_id, "Report configuration id to be configured")
      ->check(CLI::Range(1, 64));
  add_option(app, "--report_type", report_params.report_type, "Type of the report configuration")
      ->check(CLI::IsMember({"periodical", "event_triggered"}));
  add_option(app,
             "--event_triggered_report_type",
             report_params.event_triggered_report_type,
             "Type of the event triggered report")
      ->check(CLI::IsMember({"a1", "a2", "a3", "a4", "a5", "a6"}));
  add_option(app, "--report_interval_ms", report_params.report_interval_ms, "Report interval in ms")
      ->check(
          CLI::IsMember({120, 240, 480, 640, 1024, 2048, 5120, 10240, 20480, 40960, 60000, 360000, 720000, 1800000}));
  add_option(app,
             "--periodic_ho_rsrp_offset_db",
             report_params.periodic_ho_rsrp_offset,
             "Measurement trigger quantity offset in dB used to trigger handovers by periodic measurement reports. "
             "When set to -1 no handover will be triggered from periodical measurements. Note the "
             "actual value is field value * 0.5 dB")
      ->check(CLI::Range(-1, 30))
      ->capture_default_str();
  add_option(app,
             "--meas_trigger_quantity",
             report_params.meas_trigger_quantity,
             "Measurement trigger quantity (RSRP/RSRQ/SINR)")
      ->check(CLI::IsMember({"rsrp", "rsrq", "sinr"}));
  add_option(app,
             "--meas_trigger_quantitiy_threshold_db",
             report_params.meas_trigger_quantity_threshold_db,
             "Measurement trigger quantity threshold in dB used for measurement report trigger of event A1/A2/A4/A5")
      ->check(CLI::Range(0, 127));
  add_option(app,
             "--meas_trigger_quantitiy_threshold_2_db",
             report_params.meas_trigger_quantity_threshold_2_db,
             "Measurement trigger quantity threshold 2 in dB used for measurement report trigger of event A5")
      ->check(CLI::Range(0, 127));
  add_option(app,
             "--meas_trigger_quantity_offset_db",
             report_params.meas_trigger_quantity_offset_db,
             "Measurement trigger quantity offset in dB used for measurement report trigger of event A3/A6. Note the "
             "actual value is field value * 0.5 dB")

      ->check(CLI::Range(-30, 30));
  add_option(app,
             "--hysteresis_db",
             report_params.hysteresis_db,
             "Hysteresis in dB used for measurement report trigger. Note the actual value is field value * 0.5 dB")
      ->check(CLI::Range(0, 30));
  add_option(app,
             "--time_to_trigger_ms",
             report_params.time_to_trigger_ms,
             "Time in ms during which a condition must be met before measurement report trigger")
      ->check(CLI::IsMember({0, 40, 64, 80, 100, 128, 160, 256, 320, 480, 512, 640, 1024, 1280, 2560, 5120}));
}

static void configure_cli11_ncell_args(CLI::App& app, cu_cp_unit_neighbor_cell_config_item& config)
{
  add_option(app, "--nr_cell_id", config.nr_cell_id, "Neighbor cell id")
      ->check(CLI::Range(static_cast<uint64_t>(0U), nr_cell_identity::max().value()));
  add_option(
      app, "--report_configs", config.report_cfg_ids, "Report configurations to configure for this neighbor cell");
}

static void configure_cli11_cells_args(CLI::App& app, cu_cp_unit_cell_config_item& config)
{
  add_option(app, "--nr_cell_id", config.nr_cell_id, "Cell id to be configured")
      ->check(CLI::Range(static_cast<uint64_t>(0U), nr_cell_identity::max().value()));
  add_option(app,
             "--periodic_report_cfg_id",
             config.periodic_report_cfg_id,
             "Periodical report configuration for the serving cell")
      ->check(CLI::Range(1, 64));

  add_auto_enum_option(app, "--band", config.band, "NR frequency band");

  add_option(app,
             "--gnb_id_bit_length",
             config.gnb_id_bit_length,
             "gNodeB identifier bit length. If not set, it will be automatically set to be equal to the gNodeB Id of "
             "the CU-CP")
      ->check(CLI::Range(22, 32));
  add_option(app, "--pci", config.pci, "Physical Cell Id")->check(CLI::Range(0, 1007));
  add_option(app, "--ssb_arfcn", config.ssb_arfcn, "SSB ARFCN");
  add_option(app, "--ssb_scs", config.ssb_scs, "SSB subcarrier spacing")->check(CLI::IsMember({15, 30, 60, 120, 240}));
  add_option(app, "--ssb_period", config.ssb_period, "SSB period in ms")
      ->check(CLI::IsMember({5, 10, 20, 40, 80, 160}));
  add_option(app, "--ssb_offset", config.ssb_offset, "SSB offset");
  add_option(app, "--ssb_duration", config.ssb_duration, "SSB duration")->check(CLI::IsMember({1, 2, 3, 4, 5}));

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

static void configure_cli11_mobility_args(CLI::App& app, cu_cp_unit_mobility_config& config)
{
  add_option(app,
             "--trigger_handover_from_measurements",
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

static void configure_cli11_rrc_args(CLI::App& app, cu_cp_unit_rrc_config& config)
{
  add_option(app,
             "--force_reestablishment_fallback",
             config.force_reestablishment_fallback,
             "Force RRC re-establishment fallback to RRC setup")
      ->capture_default_str();

  add_option(app,
             "--rrc_procedure_guard_time_ms",
             config.rrc_procedure_guard_time_ms,
             "Guard time in ms used for RRC message exchange with UE. This is added to the RRC procedure timeout.")
      ->capture_default_str();
}

static void configure_cli11_security_args(CLI::App& app, cu_cp_unit_security_config& config)
{
  auto sec_check = [](const std::string& value) -> std::string {
    if (value == "required" || value == "preferred" || value == "not_needed") {
      return {};
    }
    return "Security indication value not supported. Accepted values [required,preferred,not_needed]";
  };

  add_option(app, "--integrity", config.integrity_protection, "Default integrity protection indication for DRBs")
      ->capture_default_str()
      ->check(sec_check);

  add_option(app,
             "--confidentiality",
             config.confidentiality_protection,
             "Default confidentiality protection indication for DRBs")
      ->capture_default_str()
      ->check(sec_check);

  add_option(app,
             "--nea_pref_list",
             config.nea_preference_list,
             "Ordered preference list for the selection of encryption algorithm (NEA) (default: NEA0, NEA2, NEA1)");

  add_option(app,
             "--nia_pref_list",
             config.nia_preference_list,
             "Ordered preference list for the selection of encryption algorithm (NIA) (default: NIA2, NIA1)")
      ->capture_default_str();
}

static void configure_cli11_f1ap_args(CLI::App& app, cu_cp_unit_f1ap_config& f1ap_params)
{
  add_option(app,
             "--procedure_timeout",
             f1ap_params.procedure_timeout,
             "Time that the F1AP waits for a DU response in milliseconds")
      ->capture_default_str();
}

static void configure_cli11_e1ap_args(CLI::App& app, cu_cp_unit_e1ap_config& e1ap_params)
{
  add_option(app,
             "--procedure_timeout",
             e1ap_params.procedure_timeout,
             "Time that the E1AP waits for a CU-UP response in milliseconds")
      ->capture_default_str();
}

static void configure_cli11_cu_cp_args(CLI::App& app, cu_cp_unit_config& cu_cp_params)
{
  add_option(
      app, "--max_nof_dus", cu_cp_params.max_nof_dus, "Maximum number of DU connections that the CU-CP may accept");

  add_option(app,
             "--max_nof_cu_ups",
             cu_cp_params.max_nof_cu_ups,
             "Maximum number of CU-UP connections that the CU-CP may accept");

  add_option(app, "--max_nof_ues", cu_cp_params.max_nof_ues, "Maximum number of UEs that the CU-CP may accept");

  add_option(app, "--max_nof_drbs_per_ue", cu_cp_params.max_nof_drbs_per_ue, "Maximum number of DRBs per UE")
      ->capture_default_str()
      ->check(CLI::Range(1, 29));

  add_option(app, "--inactivity_timer", cu_cp_params.inactivity_timer, "UE/PDU Session/DRB inactivity timer in seconds")
      ->capture_default_str()
      ->check(CLI::Range(1, 7200));

  add_option(app,
             "--request_pdu_session_timeout",
             cu_cp_params.request_pdu_session_timeout,
             "Timeout for requesting a PDU session after the InitialUeMessage was sent to the core, in "
             "seconds. The timeout must be larger than T310. If the value is reached, the UE will be released.")
      ->capture_default_str();

  CLI::App* amf_subcmd = app.add_subcommand("amf", "AMF configuration");
  configure_cli11_amf_args(*amf_subcmd, cu_cp_params.amf_config);

  // AMF parameters.
  app.add_option_function<std::vector<std::string>>(
         "--extra_amfs",
         [&cu_cp_params](const std::vector<std::string>& values) {
           cu_cp_params.extra_amfs.resize(values.size());

           for (unsigned i = 0, e = values.size(); i != e; ++i) {
             CLI::App subapp("CU-CP AMF list");
             subapp.config_formatter(create_yaml_config_parser());
             subapp.allow_config_extras(CLI::config_extras_mode::error);
             configure_cli11_amf_item_args(subapp, cu_cp_params.extra_amfs[i]);
             std::istringstream ss(values[i]);
             subapp.parse_from_stream(ss);
           }
         },
         "Sets the list of extra AMFs for the CU-CP to connect to")
      ->group("");

  CLI::App* mobility_subcmd = app.add_subcommand("mobility", "Mobility configuration");
  configure_cli11_mobility_args(*mobility_subcmd, cu_cp_params.mobility_config);

  CLI::App* rrc_subcmd = app.add_subcommand("rrc", "RRC specific configuration");
  configure_cli11_rrc_args(*rrc_subcmd, cu_cp_params.rrc_config);

  CLI::App* security_subcmd = app.add_subcommand("security", "Security configuration");
  configure_cli11_security_args(*security_subcmd, cu_cp_params.security_config);

  CLI::App* f1ap_subcmd = add_subcommand(app, "f1ap", "F1AP configuration parameters");
  configure_cli11_f1ap_args(*f1ap_subcmd, cu_cp_params.f1ap_config);

  CLI::App* e1ap_subcmd = add_subcommand(app, "e1ap", "E1AP configuration parameters");
  configure_cli11_e1ap_args(*e1ap_subcmd, cu_cp_params.e1ap_config);
}

static void configure_cli11_rlc_um_args(CLI::App& app, cu_cp_unit_rlc_um_config& rlc_um_params)
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

static void configure_cli11_rlc_am_args(CLI::App& app, cu_cp_unit_rlc_am_config& rlc_am_params)
{
  CLI::App* tx_subcmd = app.add_subcommand("tx", "AM TX parameters");
  add_option(*tx_subcmd, "--sn", rlc_am_params.tx.sn_field_length, "RLC AM TX SN size")->capture_default_str();
  add_option(*tx_subcmd, "--t-poll-retransmit", rlc_am_params.tx.t_poll_retx, "RLC AM TX t-PollRetransmit (ms)")
      ->capture_default_str();
  add_option(*tx_subcmd, "--max-retx-threshold", rlc_am_params.tx.max_retx_thresh, "RLC AM max retx threshold")
      ->capture_default_str();
  add_option(*tx_subcmd, "--poll-pdu", rlc_am_params.tx.poll_pdu, "RLC AM TX PollPdu")->capture_default_str();
  add_option(*tx_subcmd, "--poll-byte", rlc_am_params.tx.poll_byte, "RLC AM TX PollByte")->capture_default_str();
  add_option(*tx_subcmd,
             "--max_window",
             rlc_am_params.tx.max_window,
             "Non-standard parameter that limits the tx window size. Can be used for limiting memory usage with "
             "large windows. 0 means no limits other than the SN size (i.e. 2^[sn_size-1]).");
  add_option(*tx_subcmd, "--queue-size", rlc_am_params.tx.queue_size, "RLC AM TX SDU queue size")
      ->capture_default_str();

  CLI::App* rx_subcmd = app.add_subcommand("rx", "AM RX parameters");
  add_option(*rx_subcmd, "--sn", rlc_am_params.rx.sn_field_length, "RLC AM RX SN")->capture_default_str();
  add_option(*rx_subcmd, "--t-reassembly", rlc_am_params.rx.t_reassembly, "RLC AM RX t-Reassembly")
      ->capture_default_str();
  add_option(*rx_subcmd, "--t-status-prohibit", rlc_am_params.rx.t_status_prohibit, "RLC AM RX t-StatusProhibit")
      ->capture_default_str();
  add_option(*rx_subcmd, "--max_sn_per_status", rlc_am_params.rx.max_sn_per_status, "RLC AM RX status SN limit")
      ->capture_default_str();
}

static void configure_cli11_rlc_args(CLI::App& app, cu_cp_unit_rlc_config& rlc_params)
{
  add_option(app, "--mode", rlc_params.mode, "RLC mode")->capture_default_str();

  // UM section.
  CLI::App* rlc_um_subcmd = app.add_subcommand("um-bidir", "UM parameters");
  configure_cli11_rlc_um_args(*rlc_um_subcmd, rlc_params.um);

  // AM section.
  CLI::App* rlc_am_subcmd = app.add_subcommand("am", "AM parameters");
  configure_cli11_rlc_am_args(*rlc_am_subcmd, rlc_params.am);
}

static void configure_cli11_pdcp_tx_args(CLI::App& app, cu_cp_unit_pdcp_tx_config& pdcp_tx_params)
{
  add_option(app, "--sn", pdcp_tx_params.sn_field_length, "PDCP TX SN size")->capture_default_str();
  add_option(app, "--discard_timer", pdcp_tx_params.discard_timer, "PDCP TX discard timer (ms)")->capture_default_str();
  add_option(app, "--status_report_required", pdcp_tx_params.status_report_required, "PDCP TX status report required")
      ->capture_default_str();
}

static void configure_cli11_pdcp_rx_args(CLI::App& app, cu_cp_unit_pdcp_rx_config& pdcp_rx_params)
{
  add_option(app, "--sn", pdcp_rx_params.sn_field_length, "PDCP RX SN size")->capture_default_str();
  add_option(app, "--t_reordering", pdcp_rx_params.t_reordering, "PDCP RX t-Reordering (ms)")->capture_default_str();
  add_option(
      app, "--out_of_order_delivery", pdcp_rx_params.out_of_order_delivery, "PDCP RX enable out-of-order delivery")
      ->capture_default_str();
}

static void configure_cli11_pdcp_args(CLI::App& app, cu_cp_unit_pdcp_config& pdcp_params)
{
  // Transmission section.
  CLI::App* pdcp_tx_subcmd = app.add_subcommand("tx", "PDCP TX parameters");
  configure_cli11_pdcp_tx_args(*pdcp_tx_subcmd, pdcp_params.tx);

  /// Reception section.
  CLI::App* pdcp_rx_subcmd = app.add_subcommand("rx", "PDCP RX parameters");
  configure_cli11_pdcp_rx_args(*pdcp_rx_subcmd, pdcp_params.rx);
}

static void configure_cli11_qos_args(CLI::App& app, cu_cp_unit_qos_config& qos_params)
{
  add_option(app, "--five_qi", qos_params.five_qi, "5QI")->capture_default_str()->check(CLI::Range(0, 255));

  // RLC section.
  CLI::App* rlc_subcmd = app.add_subcommand("rlc", "RLC parameters");
  configure_cli11_rlc_args(*rlc_subcmd, qos_params.rlc);

  // PDCP section.
  CLI::App* pdcp_subcmd = app.add_subcommand("pdcp", "PDCP parameters");
  configure_cli11_pdcp_args(*pdcp_subcmd, qos_params.pdcp);

  // Mark the application that these subcommands need to be present.
  app.needs(rlc_subcmd);
  app.needs(pdcp_subcmd);
}

static void configure_cli11_metrics_layers_args(CLI::App& app, cu_cp_unit_metrics_layer_config& metrics_params)
{
  add_option(app, "--enable_ngap", metrics_params.enable_ngap, "Enable NGAP metrics")->capture_default_str();
  add_option(app, "--enable_pdcp", metrics_params.enable_pdcp, "Enable PDCP metrics")->capture_default_str();
  add_option(app, "--enable_rrc", metrics_params.enable_rrc, "Enable CU-CP RRC metrics")->capture_default_str();
}

static void configure_cli11_metrics_args(CLI::App& app, cu_cp_unit_metrics_config& metrics_params)
{
  auto* periodicity_subcmd = add_subcommand(app, "periodicity", "Metrics periodicity configuration")->configurable();
  add_option(*periodicity_subcmd,
             "--cu_cp_report_period",
             metrics_params.cu_cp_report_period,
             "CU-CP metrics report period in milliseconds")
      ->capture_default_str();

  auto* layers_subcmd = add_subcommand(app, "layers", "Layer basis metrics configuration")->configurable();
  configure_cli11_metrics_layers_args(*layers_subcmd, metrics_params.layers_cfg);
}

void srsran::configure_cli11_with_cu_cp_unit_config_schema(CLI::App& app, cu_cp_unit_config& unit_cfg)
{
  add_option(app, "--gnb_id", unit_cfg.gnb_id.id, "gNodeB identifier")->capture_default_str();
  add_option(app, "--gnb_id_bit_length", unit_cfg.gnb_id.bit_length, "gNodeB identifier length in bits")
      ->capture_default_str()
      ->check(CLI::Range(22, 32));
  add_option(app, "--ran_node_name", unit_cfg.ran_node_name, "RAN node name")->capture_default_str();

  // CU-CP section
  CLI::App* cu_cp_subcmd = add_subcommand(app, "cu_cp", "CU-CP parameters")->configurable();
  configure_cli11_cu_cp_args(*cu_cp_subcmd, unit_cfg);

  // Loggers section.
  CLI::App* log_subcmd = add_subcommand(app, "log", "Logging configuration")->configurable();
  configure_cli11_log_args(*log_subcmd, unit_cfg.loggers);

  // PCAP section.
  CLI::App* pcap_subcmd = add_subcommand(app, "pcap", "PCAP configuration")->configurable();
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

void srsran::autoderive_cu_cp_parameters_after_parsing(CLI::App& app, cu_cp_unit_config& unit_cfg)
{
  auto cu_cp_app = app.get_subcommand_ptr("cu_cp");
  for (auto& cell : unit_cfg.mobility_config.cells) {
    // Set gNB ID bit length of the neighbor cell to be equal to the current unit gNB ID bit length, if not explicitly
    // set.
    if (not cell.gnb_id_bit_length.has_value()) {
      cell.gnb_id_bit_length = unit_cfg.gnb_id.bit_length;
    }
  }
}
