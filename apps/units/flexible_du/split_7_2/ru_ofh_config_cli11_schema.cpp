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

#include "ru_ofh_config_cli11_schema.h"
#include "apps/services/logger/logger_appconfig_cli11_utils.h"
#include "apps/units/flexible_du/support/cli11_cpu_affinities_parser_helper.h"
#include "ru_ofh_config.h"
#include "srsran/support/cli11_utils.h"
#include "srsran/support/config_parsers.h"

using namespace srsran;

static void configure_cli11_ru_ofh_base_cell_args(CLI::App& app, ru_ofh_unit_base_cell_config& config)
{
  add_option_function<std::string>(
      app,
      "--ru_bandwidth_MHz",
      [&config](const std::string& value) {
        unsigned bandwidth;
        if (CLI::detail::lexical_cast(value, bandwidth)) {
          config.ru_operating_bw = MHz_to_bs_channel_bandwidth(bandwidth);
        } else {
          config.ru_operating_bw.reset();
        }
      },
      "Channel bandwidth in MHz")
      ->check([](const std::string& value) -> std::string {
        const std::string& error_message = "Error in the channel bandwidth property. Valid values "
                                           "[5,10,15,20,25,30,40,50,60,70,80,90,100]";

        if (value.empty()) {
          return error_message;
        }

        std::stringstream ss(value);
        unsigned          bw;
        ss >> bw;

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
  add_option(app, "--t1a_max_cp_dl", config.T1a_max_cp_dl, "T1a maximum value for downlink Control-Plane")
      ->capture_default_str()
      ->check(CLI::Range(0, 1960));
  add_option(app, "--t1a_min_cp_dl", config.T1a_min_cp_dl, "T1a minimum value for downlink Control-Plane")
      ->capture_default_str()
      ->check(CLI::Range(0, 1960));
  add_option(app, "--t1a_max_cp_ul", config.T1a_max_cp_ul, "T1a maximum value for uplink Control-Plane")
      ->capture_default_str()
      ->check(CLI::Range(0, 1960));
  add_option(app, "--t1a_min_cp_ul", config.T1a_min_cp_ul, "T1a minimum value for uplink Control-Plane")
      ->capture_default_str()
      ->check(CLI::Range(0, 1960));
  add_option(app, "--t1a_max_up", config.T1a_max_up, "T1a maximum value for User-Plane")
      ->capture_default_str()
      ->check(CLI::Range(0, 1960));
  add_option(app, "--t1a_min_up", config.T1a_min_up, "T1a minimum value for User-Plane")
      ->capture_default_str()
      ->check(CLI::Range(0, 1960));
  add_option(app, "--ta4_max", config.Ta4_max, "Ta4 maximum value for User-Plane")
      ->capture_default_str()
      ->check(CLI::Range(0, 1960));
  add_option(app, "--ta4_min", config.Ta4_min, "Ta4 minimum value for User-Plane")
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

  add_option(app, "--is_prach_cp_enabled", config.is_prach_control_plane_enabled, "PRACH Control-Plane enabled flag")
      ->capture_default_str();
  add_option(app, "--is_dl_broadcast_enabled", config.is_downlink_broadcast_enabled, "Downlink broadcast enabled flag")
      ->capture_default_str();
  add_option(app, "--ignore_ecpri_seq_id", config.ignore_ecpri_seq_id_field, "Ignore eCPRI sequence id field value")
      ->capture_default_str();
  add_option(app,
             "--ignore_ecpri_payload_size",
             config.ignore_ecpri_payload_size_field,
             "Ignore eCPRI payload size field value")
      ->capture_default_str();
  add_option(
      app, "--warn_unreceived_ru_frames", config.warn_unreceived_ru_frames, "Warn of unreceived Radio Unit frames")
      ->capture_default_str();

  auto compression_method_check = [](const std::string& value) -> std::string {
    if (value == "none" || value == "bfp" || value == "bfp selective" || value == "block scaling" ||
        value == "mu law" || value == "modulation" || value == "modulation selective") {
      return {};
    }

    return "Compression method not supported. Accepted values [none, bfp, bfp selective, block scaling, mu law, "
           "modulation, modulation selective]";
  };

  add_option(app, "--compr_method_ul", config.compression_method_ul, "Uplink compression method")
      ->capture_default_str()
      ->check(compression_method_check);
  add_option(app, "--compr_bitwidth_ul", config.compression_bitwidth_ul, "Uplink compression bit width")
      ->capture_default_str()
      ->check(CLI::Range(1, 16));
  add_option(app, "--compr_method_dl", config.compression_method_dl, "Downlink compression method")
      ->capture_default_str()
      ->check(compression_method_check);
  add_option(app, "--compr_bitwidth_dl", config.compression_bitwidth_dl, "Downlink compression bit width")
      ->capture_default_str()
      ->check(CLI::Range(1, 16));
  add_option(app, "--compr_method_prach", config.compression_method_prach, "PRACH compression method")
      ->capture_default_str()
      ->check(compression_method_check);
  add_option(app, "--compr_bitwidth_prach", config.compression_bitwidth_prach, "PRACH compression bit width")
      ->capture_default_str()
      ->check(CLI::Range(1, 16));
  add_option(app,
             "--enable_ul_static_compr_hdr",
             config.is_uplink_static_comp_hdr_enabled,
             "Uplink static compression header enabled flag")
      ->capture_default_str();
  add_option(app,
             "--enable_dl_static_compr_hdr",
             config.is_downlink_static_comp_hdr_enabled,
             "Downlink static compression header enabled flag")
      ->capture_default_str();
  add_option(app, "--iq_scaling", config.iq_scaling, "IQ scaling factor")
      ->capture_default_str()
      ->check(CLI::Range(0.0, 20.0));

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

static void configure_cli11_ru_ofh_cells_args(CLI::App& app, ru_ofh_unit_cell_config& config)
{
  configure_cli11_ru_ofh_base_cell_args(app, config.cell);
  add_option(
      app, "--network_interface", config.network_interface, "Network interface name or PCIe identifier when using DPDK")
      ->capture_default_str();
  add_option(app, "--enable_promiscuous", config.enable_promiscuous_mode, "Promiscuous mode flag")
      ->capture_default_str();
  add_option(app, "--mtu", config.mtu_size, "NIC interface MTU size")
      ->capture_default_str()
      ->check(CLI::Range(1500, 9600));
  add_option(app, "--ru_mac_addr", config.ru_mac_address, "Radio Unit MAC address")->capture_default_str();
  add_option(app, "--du_mac_addr", config.du_mac_address, "Distributed Unit MAC address")->capture_default_str();
  add_option(app, "--vlan_tag_cp", config.vlan_tag_cp, "C-Plane V-LAN identifier")
      ->capture_default_str()
      ->check(CLI::Range(1, 4094));
  add_option(app, "--vlan_tag_up", config.vlan_tag_up, "U-Plane V-LAN identifier")
      ->capture_default_str()
      ->check(CLI::Range(1, 4094));
  add_option(app, "--prach_port_id", config.ru_prach_port_id, "RU PRACH port identifier")->capture_default_str();
  add_option(app, "--dl_port_id", config.ru_dl_port_id, "RU downlink port identifier")->capture_default_str();
  add_option(app, "--ul_port_id", config.ru_ul_port_id, "RU uplink port identifier")->capture_default_str();
}

static void configure_cli11_ru_ofh_args(CLI::App& app, ru_ofh_unit_parsed_config& config)
{
  ru_ofh_unit_config& ofh_cfg = config.config;
  add_option(app, "--gps_alpha", ofh_cfg.gps_Alpha, "GPS Alpha")
      ->capture_default_str()
      ->check(CLI::Range(0.0, 1.2288e7));
  add_option(app, "--gps_beta", ofh_cfg.gps_Beta, "GPS Beta")->capture_default_str()->check(CLI::Range(-32768, 32767));

  // Common cell parameters.
  configure_cli11_ru_ofh_base_cell_args(app, config.base_cell_cfg);

  // Cell parameters.
  add_option_cell(
      app,
      "--cells",
      [&config, &ofh_cfg](const std::vector<std::string>& values) {
        ofh_cfg.cells.resize(values.size());
        ofh_cfg.expert_execution_cfg.cell_affinities.resize(values.size());
        for (auto& cell : ofh_cfg.cells) {
          cell.cell = config.base_cell_cfg;
        };

        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("RU OFH cells", "RU OFH cells config, item #" + std::to_string(i));
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras(CLI::config_extras_mode::capture);
          configure_cli11_ru_ofh_cells_args(subapp, ofh_cfg.cells[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Sets the cell configuration on a per cell basis, overwriting the default configuration defined by cell_cfg");
}

static void configure_cli11_log_args(CLI::App& app, ru_ofh_unit_logger_config& log_params)
{
  app_services::add_log_option(app, log_params.ofh_level, "--ofh_level", "Open Fronthaul log level");
}

static void configure_cli11_cell_affinity_args(CLI::App& app, ru_ofh_unit_cpu_affinities_cell_config& config)
{
  add_option_function<std::string>(
      app,
      "--ru_cpus",
      [&config](const std::string& value) { parse_affinity_mask(config.ru_cpu_cfg.mask, value, "ru_cpus"); },
      "Number of CPUs used for the Radio Unit tasks");

  app.add_option_function<std::string>(
      "--ru_pinning",
      [&config](const std::string& value) {
        config.ru_cpu_cfg.pinning_policy = to_affinity_mask_policy(value);
        if (config.ru_cpu_cfg.pinning_policy == sched_affinity_mask_policy::last) {
          report_error("Incorrect value={} used in {} property", value, "ru_pinning");
        }
      },
      "Policy used for assigning CPU cores to the Radio Unit tasks");
}

static void configure_cli11_ofh_threads_args(CLI::App& app, ru_ofh_unit_expert_threads_config& config)
{
  add_option(app,
             "--enable_dl_parallelization",
             config.is_downlink_parallelized,
             "Open Fronthaul downlink parallelization flag")
      ->capture_default_str();
}

static void configure_cli11_expert_execution_args(CLI::App& app, ru_ofh_unit_expert_execution_config& config)
{
  // Affinity section.
  CLI::App* affinities_subcmd = add_subcommand(app, "affinities", "gNB CPU affinities configuration")->configurable();
  add_option_function<std::string>(
      *affinities_subcmd,
      "--ru_timing_cpu",
      [&config](const std::string& value) { parse_affinity_mask(config.ru_timing_cpu, value, "ru_timing_cpu"); },
      "CPU used for timing in the Radio Unit");

  // Threads section.
  CLI::App* threads_subcmd = add_subcommand(app, "threads", "Threads configuration")->configurable();

  // OFH threads.
  CLI::App* ofh_threads_subcmd =
      add_subcommand(*threads_subcmd, "ofh", "Open Fronthaul thread configuration")->configurable();
  configure_cli11_ofh_threads_args(*ofh_threads_subcmd, config.threads);

  // Cell affinity section.
  add_option_cell(
      app,
      "--cell_affinities",
      [&config](const std::vector<std::string>& values) {
        config.cell_affinities.resize(values.size());

        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("OFH expert execution cell CPU affinities",
                          "OFH expert execution cell CPU affinities config, item #" + std::to_string(i));
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras();
          configure_cli11_cell_affinity_args(subapp, config.cell_affinities[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Sets the cell CPU affinities configuration on a per cell basis");
}

static void configure_cli11_hal_args(CLI::App& app, std::optional<ru_ofh_unit_hal_config>& config)
{
  config.emplace();

  add_option(app, "--eal_args", config->eal_args, "EAL configuration parameters used to initialize DPDK");
}

void srsran::configure_cli11_with_ru_ofh_config_schema(CLI::App& app, ru_ofh_unit_parsed_config& parsed_cfg)
{
  // OFH RU section.
  CLI::App* ru_ofh_subcmd = add_subcommand(app, "ru_ofh", "Open Fronthaul Radio Unit configuration")->configurable();
  configure_cli11_ru_ofh_args(*ru_ofh_subcmd, parsed_cfg);

  // Loggers section.
  CLI::App* log_subcmd = add_subcommand(app, "log", "Logging configuration")->configurable();
  configure_cli11_log_args(*log_subcmd, parsed_cfg.config.loggers);

  // Expert execution section.
  CLI::App* expert_subcmd = add_subcommand(app, "expert_execution", "Expert execution configuration")->configurable();
  configure_cli11_expert_execution_args(*expert_subcmd, parsed_cfg.config.expert_execution_cfg);

  // HAL section.
  CLI::App* hal_subcmd = add_subcommand(app, "hal", "HAL configuration")->configurable();
  configure_cli11_hal_args(*hal_subcmd, parsed_cfg.config.hal_config);
}

static void manage_hal_optional(CLI::App& app, std::optional<ru_ofh_unit_hal_config>& hal_config)
{
  // Clean the HAL optional.
  if (auto subcmd = app.get_subcommand("hal"); subcmd->count_all() == 0) {
    hal_config.reset();
    // As HAL configuration is optional, disable the command when it is not present in the configuration.
    subcmd->disabled();
  }
}

void srsran::autoderive_ru_ofh_parameters_after_parsing(CLI::App& app, ru_ofh_unit_parsed_config& parsed_cfg)
{
  manage_hal_optional(app, parsed_cfg.config.hal_config);
}
