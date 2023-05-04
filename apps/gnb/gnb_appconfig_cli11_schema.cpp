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
  app.add_option("--f1u_level", log_params.f1u_level, "F1-U log level")->capture_default_str()->check(level_check);
  app.add_option("--du_level", log_params.du_level, "Log level for the DU")->capture_default_str()->check(level_check);
  app.add_option("--cu_level", log_params.cu_level, "Log level for the CU")->capture_default_str()->check(level_check);
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
  });
}

static void configure_cli11_pcap_args(CLI::App& app, pcap_appconfig& pcap_params)
{
  app.add_option("--ngap_filename", pcap_params.ngap.filename, "NGAP PCAP file output path")->capture_default_str();
  app.add_option("--ngap_enable", pcap_params.ngap.enabled, "Enable NGAP packet capture")->always_capture_default();
  app.add_option("--e1ap_filename", pcap_params.e1ap.filename, "E1AP PCAP file output path")->capture_default_str();
  app.add_option("--e1ap_enable", pcap_params.e1ap.enabled, "Enable E1AP packet capture")->always_capture_default();
  app.add_option("--mac_filename", pcap_params.mac.filename, "MAC PCAP file output path")->capture_default_str();
  app.add_option("--mac_enable", pcap_params.mac.enabled, "Enable MAC packet capture")->always_capture_default();
}

static void configure_cli11_amf_args(CLI::App& app, amf_appconfig& amf_params)
{
  app.add_option("--addr", amf_params.ip_addr, "AMF IP address")->check(CLI::ValidIPV4)->required();
  app.add_option("--port", amf_params.port, "AMF port")->capture_default_str()->check(CLI::Range(20000, 40000));
  app.add_option("--bind_addr", amf_params.bind_addr, "Local IP address to bind for AMF connection")
      ->check(CLI::ValidIPV4);
  app.add_option("--sctp_rto_initial", amf_params.sctp_rto_initial, "SCTP initial RTO value");
  app.add_option("--sctp_rto_min", amf_params.sctp_rto_min, "SCTP RTO min");
  app.add_option("--sctp_rto_max", amf_params.sctp_rto_max, "SCTP RTO max");
  app.add_option("--sctp_init_max_attempts", amf_params.sctp_init_max_attempts, "SCTP init max attempts");
  app.add_option("--sctp_max_init_timeo", amf_params.sctp_max_init_timeo, "SCTP max init timeout");
}

static void configure_cli11_cu_cp_args(CLI::App& app, cu_cp_appconfig& cu_cp_params)
{
  app.add_option("--inactivity_timer", cu_cp_params.inactivity_timer, "UE/PDU Session/DRB inactivity timer")
      ->capture_default_str()
      ->check(CLI::Range(1, 7200));
}

static void configure_cli11_rf_driver_args(CLI::App& app, rf_driver_appconfig& rf_driver_params)
{
  app.add_option("--srate", rf_driver_params.srate_MHz, "Sample rate in MHz")->capture_default_str();
  app.add_option("--device_driver", rf_driver_params.device_driver, "Device driver name")->capture_default_str();
  app.add_option("--device_args", rf_driver_params.device_arguments, "Optional device arguments")
      ->capture_default_str();
  app.add_option("--tx_gain", rf_driver_params.tx_gain_dB, "Transmit gain in decibels")->capture_default_str();
  app.add_option("--rx_gain", rf_driver_params.rx_gain_dB, "Receive gain in decibels")->capture_default_str();
  app.add_option("--freq_offset", rf_driver_params.center_freq_offset_Hz, "Center frequency offset in hertz")
      ->capture_default_str();
  app.add_option("--clock_ppm", rf_driver_params.calibrate_clock_ppm, "Clock calibration in PPM.")
      ->capture_default_str();
  app.add_option("--lo_offset", rf_driver_params.lo_offset_MHz, "LO frequency offset in MHz")->capture_default_str();
  app.add_option("--clock", rf_driver_params.clock_source, "Clock source")->capture_default_str();
  app.add_option("--sync", rf_driver_params.synch_source, "Time synchronization source")->capture_default_str();
  app.add_option("--otw_format", rf_driver_params.otw_format, "Over-the-wire format")->capture_default_str();
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
}

static void configure_cli11_expert_phy_args(CLI::App& app, expert_phy_appconfig& expert_phy_params)
{
  app.add_option_function<std::string>(
         "--low_phy_thread_profile",
         [&expert_phy_params](const std::string& value) {
           if (value == "single") {
             expert_phy_params.lphy_executor_profile = lower_phy_thread_profile::single;
           } else if (value == "dual") {
             expert_phy_params.lphy_executor_profile = lower_phy_thread_profile::dual;
           } else if (value == "quad") {
             expert_phy_params.lphy_executor_profile = lower_phy_thread_profile::quad;
           }
         },
         "Lower physical layer executor profile [single, dual, quad].")
      ->check([](const std::string& value) -> std::string {
        if ((value == "single") || (value == "dual") || (value == "quad")) {
          return "";
        }

        return "Invalid executor profile. Valid profiles are: single, dual and quad.";
      });
  app.add_option("--nof_ul_threads", expert_phy_params.nof_ul_threads, "Number of threads to process uplink")
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

static void configure_cli11_pdcch_args(CLI::App& app, pdcch_appconfig& pdcch_params)
{
  app.add_option_function<std::string>(
         "--ss_type",
         [&pdcch_params](const std::string& value) {
           pdcch_params.ue_ss_type = (value == "common") ? search_space_configuration::type_t::common
                                                         : search_space_configuration::type_t::ue_dedicated;
         },
         "SearchSpace type for UE data")
      ->default_str("ue_dedicated")
      ->check(CLI::IsMember({"common", "ue_dedicated"}, CLI::ignore_case));
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
}

static void configure_cli11_pusch_args(CLI::App& app, pusch_appconfig& pusch_params)
{
  app.add_option("--min_ue_mcs", pusch_params.min_ue_mcs, "Minimum UE MCS")
      ->capture_default_str()
      ->check(CLI::Range(0, 28));
  app.add_option("--max_ue_mcs", pusch_params.max_ue_mcs, "Maximum UE MCS")
      ->capture_default_str()
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
  app.add_option("--total_nof_ra_preambles", prach_params.total_nof_ra_preambles, "Number of different PRACH preambles")
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

static void configure_cli11_tdd_pattern_args(CLI::App& app, tdd_ul_dl_pattern_config& tdd_pattern_params)
{
  app.add_option("--dl_ul_tx_period", tdd_pattern_params.dl_ul_tx_period, "TDD pattern periodicity in milliseconds")
      ->capture_default_str()
      ->check(CLI::Range(0.0, 10.0));
  app.add_option("--nof_dl_slots", tdd_pattern_params.nof_dl_slots, "TDD pattern nof. consecutive full DL slots")
      ->capture_default_str()
      ->check(CLI::Range(0, 80));
  app.add_option("--nof_dl_symbols",
                 tdd_pattern_params.nof_dl_symbols,
                 "TDD pattern nof. DL symbols at the beginning of the slot following full DL slots")
      ->capture_default_str()
      ->check(CLI::Range(0, 13));
  app.add_option("--nof_ul_slots", tdd_pattern_params.nof_ul_slots, "TDD pattern nof. consecutive full UL slots")
      ->capture_default_str()
      ->check(CLI::Range(0, 80));
  app.add_option("--nof_ul_symbols",
                 tdd_pattern_params.nof_ul_symbols,
                 "TDD pattern nof. UL symbols at the end of the slot preceding the first full UL slot")
      ->capture_default_str()
      ->check(CLI::Range(0, 13));
}

static void configure_cli11_tdd_ul_dl_args(CLI::App& app, tdd_ul_dl_config& tdd_ul_dl_params)
{
  CLI::App* pattern1_subcmd = app.add_subcommand("pattern1", "TDD pattern 1 configuration parameters")->configurable();
  configure_cli11_tdd_pattern_args(*pattern1_subcmd, tdd_ul_dl_params.pattern1);

  CLI::App* pattern2_subcmd = app.add_subcommand("pattern2", "TDD pattern 2 configuration parameters")->configurable();
  configure_cli11_tdd_pattern_args(*pattern2_subcmd, tdd_ul_dl_params.pattern2.emplace());
  auto tdd_pattern2_verify_callback = [&]() {
    CLI::App* tdd_pattern_cfg = app.get_subcommand("pattern2");
    if (tdd_pattern_cfg->count_all() == 0) {
      tdd_ul_dl_params.pattern2.reset();
    }
  };
  app.callback(tdd_pattern2_verify_callback);
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

  // PDCCH configuration.
  CLI::App* pdcch_subcmd = app.add_subcommand("pdcch", "PDCCH parameters");
  configure_cli11_pdcch_args(*pdcch_subcmd, cell_params.pdcch_cfg);

  // PDSCH configuration.
  CLI::App* pdsch_subcmd = app.add_subcommand("pdsch", "PDSCH parameters");
  configure_cli11_pdsch_args(*pdsch_subcmd, cell_params.pdsch_cfg);

  // PUSCH configuration.
  CLI::App* pusch_subcmd = app.add_subcommand("pusch", "PUSCH parameters");
  configure_cli11_pusch_args(*pusch_subcmd, cell_params.pusch_cfg);

  // PRACH configuration.
  CLI::App* prach_subcmd = app.add_subcommand("prach", "PRACH parameters");
  configure_cli11_prach_args(*prach_subcmd, cell_params.prach_cfg);

  // Amplitude control configuration.
  CLI::App* amplitude_control_subcmd = app.add_subcommand("amplitude_control", "Amplitude control parameters");
  configure_cli11_amplitude_control_args(*amplitude_control_subcmd, cell_params.amplitude_cfg);

  // TDD UL DL configuration.
  CLI::App* tdd_ul_dl_subcmd =
      app.add_subcommand("tdd_ul_dl_cfg", "TDD UL DL configuration parameters")->configurable();
  configure_cli11_tdd_ul_dl_args(*tdd_ul_dl_subcmd, cell_params.tdd_pattern_cfg.emplace());
  auto tdd_ul_dl_verify_callback = [&]() {
    CLI::App* tdd_cfg = app.get_subcommand("tdd_ul_dl_cfg");
    if (tdd_cfg->count_all() == 0) {
      cell_params.tdd_pattern_cfg.reset();
    }
  };
  app.callback(tdd_ul_dl_verify_callback);
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
  CLI::App* pdcp_subcmd = app.add_subcommand("pdcp", "PDCP parameters");
  configure_cli11_pdcp_args(*pdcp_subcmd, qos_params.pdcp);
  auto verify_callback = [&]() {
    CLI::App* rlc  = app.get_subcommand("rlc");
    CLI::App* pdcp = app.get_subcommand("pdcp");
    if (rlc->count_all() == 0) {
      report_error("Error parsing QoS config for 5QI {}. RLC configuration not present.\n", qos_params.five_qi);
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
}

static void configure_cli11_test_mode_args(CLI::App& app, test_mode_appconfig& test_params)
{
  CLI::App* test_ue = app.add_subcommand("test_ue", "automatically created UE for testing purposes");
  configure_cli11_test_ue_mode_args(*test_ue, test_params.test_ue);
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

  // CU-CP section
  CLI::App* cu_cp_subcmd = app.add_subcommand("cu_cp", "CU-CP parameters")->configurable();
  configure_cli11_cu_cp_args(*cu_cp_subcmd, gnb_cfg.cu_cp_cfg);

  // RF section.
  CLI::App* rf_driver_subcmd = app.add_subcommand("rf_driver", "RF driver parameters")->configurable();
  configure_cli11_rf_driver_args(*rf_driver_subcmd, gnb_cfg.rf_driver_cfg);

  // Common cell parameters.
  CLI::App* common_cell_subcmd = app.add_subcommand("cell_cfg", "Cell configuration")->configurable();
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
  app.add_option_function<std::vector<std::string>>("--qos", qos_lambda, "qos");

  // Expert PHY section.
  CLI::App* expert_phy_subcmd = app.add_subcommand("expert_phy", "Expert physical layer configuration")->configurable();
  configure_cli11_expert_phy_args(*expert_phy_subcmd, gnb_cfg.expert_phy_cfg);

  // Test mode section.
  CLI::App* test_mode_subcmd = app.add_subcommand("test_mode", "Test mode configuration")->configurable();
  configure_cli11_test_mode_args(*test_mode_subcmd, gnb_cfg.test_mode_cfg);
}
