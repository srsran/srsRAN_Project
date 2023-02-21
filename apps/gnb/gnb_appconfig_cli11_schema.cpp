/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "gnb_appconfig_cli11_schema.h"
#include "gnb_appconfig.h"
#include "srsran/support/cli11_utils.h"
#include "srsran/support/config_parsers.h"
#include "srsran/support/error_handling.h"

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
  app.add_option("--all_level", log_params.all_level, "Log level of all loggers")
      ->capture_default_str()
      ->check(level_check);
  app.add_option("--app_level", log_params.app_level, "Generic log level")->capture_default_str()->check(level_check);
  app.add_option("--du_level", log_params.du_level, "Log level for the DU")->capture_default_str()->check(level_check);
  app.add_option("--cu_level", log_params.cu_level, "Log level for the CU")->capture_default_str()->check(level_check);
  app.add_option("--phy_level", log_params.phy_level, "PHY log level")->capture_default_str()->check(level_check);
  app.add_option("--radio_level", log_params.radio_level, "Radio log level")->capture_default_str()->check(level_check);
  app.add_option("--mac_level", log_params.mac_level, "MAC log level")->capture_default_str()->check(level_check);
  app.add_option("--rlc_level", log_params.rlc_level, "RLC log level")->capture_default_str()->check(level_check);
  app.add_option("--f1u_level", log_params.f1u_level, "F1-U log level")->capture_default_str()->check(level_check);
  app.add_option("--pdcp_level", log_params.pdcp_level, "PDCP log level")->capture_default_str()->check(level_check);
  app.add_option("--rrc_level", log_params.rrc_level, "RRC log level")->capture_default_str()->check(level_check);
  app.add_option("--sdap_level", log_params.sdap_level, "SDAP log level")->capture_default_str()->check(level_check);
  app.add_option("--gtpu_level", log_params.gtpu_level, "GTPU log level")->capture_default_str()->check(level_check);
  app.add_option("--fapi_level", log_params.fapi_level, "FAPI log level")->capture_default_str()->check(level_check);
  app.add_option("--hex_max_size", log_params.hex_max_size, "Number of bytes to print in hex")
      ->capture_default_str()
      ->check(CLI::Range(0, 1024));
  app.add_option("--phy_broadcast",
                 log_params.phy_broadcast,
                 "Enable logging in the physical layer of broadcast messages and all PRACH opportunities")
      ->always_capture_default();
  app.add_option("--phy_rx_symbols_filename",
                 log_params.phy_rx_symbols_filename,
                 "Set to a valid file path to print the received symbols.")
      ->always_capture_default();

  /// Post-parsing callback. This allows us to
  /// set the log level to "all" level, if no level
  /// is provided.
  app.callback([&]() {
    if (app.count("--app_level") == 0) {
      log_params.app_level = log_params.all_level;
    }
    if (app.count("--du_level") == 0) {
      log_params.du_level = log_params.all_level;
    }
    if (app.count("--cu_level") == 0) {
      log_params.cu_level = log_params.all_level;
    }
    if (app.count("--phy_level") == 0) {
      log_params.phy_level = log_params.all_level;
    }
    if (app.count("--radio_level") == 0) {
      log_params.radio_level = log_params.all_level;
    }
    if (app.count("--mac_level") == 0) {
      log_params.mac_level = log_params.all_level;
    }
    if (app.count("--rlc_level") == 0) {
      log_params.rlc_level = log_params.all_level;
    }
    if (app.count("--f1u_level") == 0) {
      log_params.f1u_level = log_params.all_level;
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
    if (app.count("--gtpu_level") == 0) {
      log_params.gtpu_level = log_params.all_level;
    }
    if (app.count("--fapi_level") == 0) {
      log_params.gtpu_level = log_params.all_level;
    }
  });
}

static void configure_cli11_pcap_args(CLI::App& app, pcap_appconfig& pcap_params)
{
  app.add_option("--ngap_filename", pcap_params.ngap.filename, "NGAP PCAP file output path")->capture_default_str();
  app.add_option("--ngap_enable", pcap_params.ngap.enabled, "Enable NGAP packet capture")->always_capture_default();
  app.add_option("--mac_filename", pcap_params.mac.filename, "MAC PCAP file output path")->capture_default_str();
  app.add_option("--mac_enable", pcap_params.mac.enabled, "Enable MAC packet capture")->always_capture_default();
}

static void configure_cli11_amf_args(CLI::App& app, amf_appconfig& amf_params)
{
  app.add_option("--addr", amf_params.ip_addr, "AMF IP address")->check(CLI::ValidIPV4)->required();
  app.add_option("--port", amf_params.port, "AMF port")->capture_default_str()->check(CLI::Range(20000, 40000));
  app.add_option("--bind_addr", amf_params.bind_addr, "Local IP address to bind for AMF connection")
      ->check(CLI::ValidIPV4);
}

static void configure_cli11_rf_driver_args(CLI::App& app, rf_driver_appconfig& rf_driver_params)
{
  app.add_option("--srate", rf_driver_params.srate_MHz, "Sample rate in MHz")->capture_default_str();
  app.add_option("--device_driver", rf_driver_params.device_driver, "Device driver name")->capture_default_str();
  app.add_option("--device_args", rf_driver_params.device_arguments, "Optional device arguments.")
      ->capture_default_str();
  app.add_option("--tx_gain", rf_driver_params.tx_gain_dB, "Transmit gain in decibels.")->capture_default_str();
  app.add_option("--rx_gain", rf_driver_params.rx_gain_dB, "Receive gain in decibels.")->capture_default_str();
  app.add_option("--lo_offset", rf_driver_params.lo_offset_MHz, "LO frequency offset in MHz.")->capture_default_str();
  app.add_option("--clock", rf_driver_params.clock_source, "Clock source.")->capture_default_str();
  app.add_option("--sync", rf_driver_params.synch_source, "Time synchronization source.")->capture_default_str();
  app.add_option("--otw_format", rf_driver_params.otw_format, "Over-the-wire format.")->capture_default_str();
  app.add_option_function<std::string>(
         "--time_alignment_calibration",
         [&rf_driver_params](const std::string& value) {
           if (!value.empty() && value != "auto") {
             std::stringstream ss(value);
             int               ta_samples;
             ss >> ta_samples;
             rf_driver_params.time_alignment_calibration = ta_samples;
           }
         },
         "Rx to Tx radio time alignment calibration in samples.\n"
         "Positive values reduce the RF transmission delay with respect\n"
         "to the RF reception, while negative values increase it.")
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
}

static void configure_cli11_expert_phy_args(CLI::App& app, expert_phy_appconfig& expert_phy_params)
{
  app.add_option("--nof_ul_threads", expert_phy_params.nof_ul_threads, "Number of threads to process uplink.")
      ->capture_default_str()
      ->check(CLI::Number);
  app.add_option("--pusch_dec_max_iterations",
                 expert_phy_params.pusch_decoder_max_iterations,
                 "Maximum number of PUSCH LDPC decoder iterations.")
      ->capture_default_str()
      ->check(CLI::Number);
  app.add_option("--pusch_dec_enable_early_stop",
                 expert_phy_params.pusch_decoder_early_stop,
                 "Enables PUSCH LDPC decoder early stop.")
      ->capture_default_str();
}

static void configure_cli11_pdsch_args(CLI::App& app, pdsch_appconfig& pdsch_params)
{
  app.add_option("--fixed_ue_mcs", pdsch_params.fixed_ue_mcs, "Fixed UE MCS")
      ->capture_default_str()
      ->check(CLI::Range(0, 28));
  app.add_option("--fixed_rar_mcs", pdsch_params.fixed_rar_mcs, "Fixed RAR MCS")
      ->capture_default_str()
      ->check(CLI::Range(0, 28));
  app.add_option("--fixed_sib1_mci", pdsch_params.fixed_sib1_mcs, "Fixed SIB1 MCS")
      ->capture_default_str()
      ->check(CLI::Range(0, 28));
}

static void configure_cli11_csi_args(CLI::App& app, csi_appconfig& csi_params) {}

static void configure_cli11_pucch_args(CLI::App& app, pucch_appconfig& pucch_params) {}

static void configure_cli11_pusch_args(CLI::App& app, pusch_appconfig& pusch_params)
{
  app.add_option("--fixed_ue_mcs", pusch_params.fixed_ue_mcs, "Fixed UE MCS")
      ->default_str("dynamic")
      ->check(CLI::Range(0, 28));
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
}

static void configure_cli11_amplitude_control_args(CLI::App& app, amplitude_control_appconfig& amplitude_params)
{
  app.add_option("--tx_gain_backoff", amplitude_params.gain_backoff_dB, "Gain back-off to accommodate the signal PAPR")
      ->capture_default_str();
  app.add_option("--enable_clipping", amplitude_params.enable_clipping, "Signal clipping")->capture_default_str();
  app.add_option("--ceiling", amplitude_params.power_ceiling_dBFS, "Clipping ceiling referenced to full scale")
      ->capture_default_str();
}

static void configure_cli11_common_cell_args(CLI::App& app, base_cell_appconfig& cell_params)
{
  app.add_option("--dl_arfcn", cell_params.dl_arfcn, "Donwlink ARFCN")->capture_default_str();
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

  // PDSCH configuration.
  CLI::App* pdsch_subcmd = app.add_subcommand("pdsch", "PDSCH parameters");
  configure_cli11_pdsch_args(*pdsch_subcmd, cell_params.pdsch_cfg);

  // CSI configuration.
  CLI::App* csi_subcmd = app.add_subcommand("CSI", "CSI parameters");
  configure_cli11_csi_args(*csi_subcmd, cell_params.csi_cfg);

  // PUCCH configuration.
  CLI::App* pucch_subcmd = app.add_subcommand("pucch", "PUCCH parameters");
  configure_cli11_pucch_args(*pucch_subcmd, cell_params.pucch_cfg);

  // PUSCH configuration.
  CLI::App* pusch_subcmd = app.add_subcommand("pusch", "PUSCH parameters");
  configure_cli11_pusch_args(*pusch_subcmd, cell_params.pusch_cfg);

  // PRACH configuration.
  CLI::App* prach_subcmd = app.add_subcommand("prach", "PRACH parameters");
  configure_cli11_prach_args(*prach_subcmd, cell_params.prach_cfg);

  // Amplitude control configuration.
  CLI::App* amplitude_control_subcmd = app.add_subcommand("amplitude_control", "Amplitude control parameters");
  configure_cli11_amplitude_control_args(*amplitude_control_subcmd, cell_params.amplitude_cfg);
}

static void configure_cli11_cells_args(CLI::App& app, cell_appconfig& cell_params)
{
  app.add_option("--pci", cell_params.pci, "PCI")->capture_default_str()->check(CLI::Range(0, 1007));
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
  rlc_tx_am_subcmd->add_option("--sn", rlc_am_params.tx.sn_field_length, "RLC AM TX SN")->capture_default_str();
  rlc_tx_am_subcmd->add_option("--t-poll-retransmit", rlc_am_params.tx.t_poll_retx, "RLC AM TX SN")
      ->capture_default_str();
  rlc_tx_am_subcmd->add_option("--max-retx-threshold", rlc_am_params.tx.max_retx_thresh, "RLC AM TX SN")
      ->capture_default_str();
  rlc_tx_am_subcmd->add_option("--poll-pdu", rlc_am_params.tx.poll_pdu, "RLC AM TX SN")->capture_default_str();
  rlc_tx_am_subcmd->add_option("--poll-byte", rlc_am_params.tx.poll_byte, "RLC AM TX SN")->capture_default_str();
  CLI::App* rlc_rx_am_subcmd = app.add_subcommand("rx", "AM RX parameters");
  rlc_rx_am_subcmd->add_option("--sn", rlc_am_params.rx.sn_field_length, "RLC AM RX SN")->capture_default_str();
  rlc_rx_am_subcmd->add_option("--t-reassembly", rlc_am_params.rx.t_reassembly, "RLC AM RX SN")->capture_default_str();
  rlc_rx_am_subcmd->add_option("--t-status-prohibit", rlc_am_params.rx.t_status_prohibit, "RLC AM RX SN")
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

static void configure_cli11_qos_args(CLI::App& app, qos_appconfig& qos_params)
{
  app.add_option("--five_qi", qos_params.five_qi, "5QI")->capture_default_str()->check(CLI::Range(0, 255));
  CLI::App* rlc_subcmd = app.add_subcommand("rlc", "RLC parameters");
  configure_cli11_rlc_args(*rlc_subcmd, qos_params.rlc);
}

void srsran::configure_cli11_with_gnb_appconfig_schema(CLI::App& app, gnb_appconfig& gnb_cfg)
{
  app.add_option("--gnb_id", gnb_cfg.gnb_id, "gNodeB identifier")->capture_default_str();
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

  // RF section.
  CLI::App* rf_driver_subcmd = app.add_subcommand("rf_driver", "RF driver parameters")->configurable();
  configure_cli11_rf_driver_args(*rf_driver_subcmd, gnb_cfg.rf_driver_cfg);

  // Common cell parameters.
  CLI::App* common_cell_subcmd = app.add_subcommand("common_cell", "Common cell parameters")->configurable();
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
      "cells");

  // QoS parameters.
  auto qos_lambda = [&gnb_cfg](const std::vector<std::string>& values) {
    // Prepare the radio bearers
    gnb_cfg.qos_cfg.resize(values.size());

    //// Format every QoS setting.
    for (unsigned i = 0, e = values.size(); i != e; ++i) {
      CLI::App subapp("QoS parameters");
      subapp.config_formatter(create_yaml_config_parser());
      subapp.allow_config_extras(CLI::config_extras_mode::error);
      configure_cli11_qos_args(subapp, gnb_cfg.qos_cfg[i]);
      std::istringstream ss(values[i]);
      subapp.parse_from_stream(ss);
    }
  };
  app.add_option_function<std::vector<std::string>>("--qos", qos_lambda, "qos");

  // Expert PHY section.
  CLI::App* expert_phy_subcmd =
      app.add_subcommand("expert_phy", "Expert physical layer configuration.")->configurable();
  configure_cli11_expert_phy_args(*expert_phy_subcmd, gnb_cfg.expert_phy_cfg);
}
