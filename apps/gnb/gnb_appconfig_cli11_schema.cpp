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
#include "srsgnb/support/config_parsers.h"
#include "srsgnb/support/error_handling.h"

using namespace srsgnb;

static void configure_cli11_log_args(CLI::App& app, log_appconfig& log_params)
{
  auto level_check = [](const std::string& value) -> std::string {
    if (value == "info" || value == "debug" || value == "warning" || value == "error") {
      return {};
    }
    return "Log level value not supported. Accepted values [info,debug,warning,error]";
  };

  app.add_option("--filename", log_params.filename, "Log file output path")->capture_default_str();
  app.add_option("--app_level", log_params.app_level, "Generic log level")->capture_default_str()->check(level_check);
  app.add_option("--du_level", log_params.du_level, "Log level for the DU")->capture_default_str()->check(level_check);
  app.add_option("--cu_level", log_params.cu_level, "Log level for the CU")->capture_default_str()->check(level_check);
  app.add_option("--phy_level", log_params.phy_level, "PHY log level")->capture_default_str()->check(level_check);
  app.add_option("--mac_level", log_params.mac_level, "MAC log level")->capture_default_str()->check(level_check);
  app.add_option("--rlc_level", log_params.rlc_level, "RLC log level")->capture_default_str()->check(level_check);
  app.add_option("--pdcp_level", log_params.pdcp_level, "PDCP log level")->capture_default_str()->check(level_check);
  app.add_option("--rrc_level", log_params.rrc_level, "RRC log level")->capture_default_str()->check(level_check);
  app.add_option("--hex_max_size", log_params.hex_max_size, "Number of bytes to print in hex")
      ->capture_default_str()
      ->check(CLI::Range(0, 1024));
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
  app.add_option("--device_args", rf_driver_params.device_args, "Device arguments")->capture_default_str();
}

static void to_aggregation_level(aggregation_level& aggr_lvl, unsigned value)
{
  switch (value) {
    case 1U:
      aggr_lvl = srsgnb::aggregation_level::n1;
      return;
    case 2U:
      aggr_lvl = srsgnb::aggregation_level::n2;
      return;
    case 4U:
      aggr_lvl = srsgnb::aggregation_level::n4;
      return;
    case 8U:
      aggr_lvl = srsgnb::aggregation_level::n8;
      return;
    case 16U:
      aggr_lvl = srsgnb::aggregation_level::n16;
      return;
  }
}

static void configure_cli11_pdcch_args(CLI::App& app, pdcch_appconfig& pdcch_params)
{
  auto agg_validator = [](const std::string& value) -> std::string {
    std::stringstream ss(value);
    unsigned          agg_lvl;
    ss >> agg_lvl;
    if (agg_lvl == 1U || agg_lvl == 2U || agg_lvl == 4U || agg_lvl == 8U || agg_lvl == 16U) {
      return {};
    }
    return "Aggregation level value is not correct [1,2,4,8,16]";
  };

  app.add_option_function<unsigned>(
         "--ue_aggr_lvl_index",
         [&pdcch_params](unsigned value) { to_aggregation_level(pdcch_params.ue_aggregation_level_index, value); },
         "UE aggregation level index")
      ->capture_default_str()
      ->check(agg_validator);
  app.add_option_function<unsigned>(
         "--rar_aggr_lvl_index",
         [&pdcch_params](unsigned value) { to_aggregation_level(pdcch_params.rar_aggregation_level_index, value); },
         "RAR aggregation level index")
      ->capture_default_str()
      ->check(agg_validator);
  app.add_option_function<unsigned>(
         "--si_aggr_lvl_index",
         [&pdcch_params](unsigned value) { to_aggregation_level(pdcch_params.si_aggregation_level_index, value); },
         "SI aggregation level index")
      ->capture_default_str()
      ->check(agg_validator);
}

static void configure_cli11_pdsch_args(CLI::App& app, pdsch_appconfig& pdsch_params)
{
  app.add_option("--fixed_ue_mcs", pdsch_params.fixed_ue_mcs, "Fixed UE MCS")
      ->capture_default_str()
      ->check(CLI::Range(0, 31));
  app.add_option("--fixed_rar_mcs", pdsch_params.fixed_rar_mcs, "Fixed RAR MCS")
      ->capture_default_str()
      ->check(CLI::Range(0, 31));
  app.add_option("--fixed_si_mci", pdsch_params.fixed_si_mcs, "Fixed SI MCS")
      ->capture_default_str()
      ->check(CLI::Range(0, 31));
}

static void configure_cli11_csi_args(CLI::App& app, csi_appconfig& csi_params) {}

static void configure_cli11_pucch_args(CLI::App& app, pucch_appconfig& pucch_params) {}

static void configure_cli11_pusch_args(CLI::App& app, pusch_appconfig& pusch_params)
{
  app.add_option("--fixed_ue_mcs", pusch_params.fixed_ue_mcs, "Fixed UE MCS")
      ->capture_default_str()
      ->check(CLI::Range(0, 31));
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
      ->check(CLI::Range(0, 31));
  app.add_option(
         "--max_msg3_harq_retx", prach_params.max_msg3_harq_retx, "Maximum number of message 3 HARQ retransmissions")
      ->capture_default_str();
}

static void configure_cli11_common_cell_args(CLI::App& app, base_cell_appconfig& cell_params)
{
  app.add_option("--dl_arfcn", cell_params.dl_arfcn, "Donwlink ARFCN")->capture_default_str();
  app.add_option("--band", cell_params.band, "NR band")->capture_default_str();
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
}

static void configure_cli11_cells_args(CLI::App& app, cell_appconfig& cell_params)
{
  app.add_option("--pci", cell_params.pci, "Donwlink ARFCN")->capture_default_str()->check(CLI::Range(0, 1007));
  configure_cli11_common_cell_args(app, cell_params.cell);
}

void srsgnb::configure_cli11_with_gnb_appconfig_schema(CLI::App& app, gnb_appconfig& gnb_cfg)
{
  app.add_option("--gnb_id", gnb_cfg.gnb_id, "gNodeB identifier")->capture_default_str();
  app.add_option("--ran_node_name", gnb_cfg.ran_node_name, "RAN node name")->capture_default_str();

  // Logging section.
  CLI::App* log_subcmd = app.add_subcommand("log", "Logging configuration")->configurable();
  configure_cli11_log_args(*log_subcmd, gnb_cfg.log_cfg);

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
          configure_cli11_cells_args(subapp, gnb_cfg.cells_cfg[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "cells");
}
