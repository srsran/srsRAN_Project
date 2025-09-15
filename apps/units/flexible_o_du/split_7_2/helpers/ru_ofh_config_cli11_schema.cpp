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

#include "ru_ofh_config_cli11_schema.h"
#include "apps/helpers/logger/logger_appconfig_cli11_utils.h"
#include "apps/helpers/metrics/metrics_config_cli11_schema.h"
#include "apps/services/worker_manager/cli11_cpu_affinities_parser_helper.h"
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
  add_option(app, "--ignore_ecpri_seq_id", config.ignore_ecpri_seq_id_field, "Ignore eCPRI sequence id field value")
      ->capture_default_str();
  add_option(app,
             "--ignore_ecpri_payload_size",
             config.ignore_ecpri_payload_size_field,
             "Ignore eCPRI payload size field value")
      ->capture_default_str();
  add_option(app,
             "--ignore_prach_start_symbol",
             config.ignore_prach_start_symbol,
             "Ignore the start symbol field in the PRACH U-Plane packets")
      ->capture_default_str();

  add_option(app, "--log_lates_as_warnings", config.enable_log_warnings_for_lates, "Log late events as warnings")
      ->capture_default_str();

  add_option_function<std::string>(
      app,
      "--warn_unreceived_ru_frames",
      [&config](const std::string& value) {
        config.log_unreceived_ru_frames = ofh::to_warn_unreceived_ru_frames(value).value();
      },
      "Warn of unreceived Radio Unit frames")
      ->default_function([&config]() { return to_string(config.log_unreceived_ru_frames); })
      ->capture_default_str()
      ->check([](const std::string& value) -> std::string {
        auto warn = ofh::to_warn_unreceived_ru_frames(value);
        if (warn.has_value()) {
          return {};
        }

        return "Warn of unreceived Radio Unit frames type not supported. Accepted values [never, always, "
               "after_traffic_detection]";
      });

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

  app.add_option_function<float>(
         "--ru_reference_level_dBFS",
         [&config](float value) {
           if (!std::holds_alternative<ru_ofh_scaling_config>(config.iq_scaling_config)) {
             config.iq_scaling_config.emplace<ru_ofh_scaling_config>();
           }
           std::get<ru_ofh_scaling_config>(config.iq_scaling_config).ru_reference_level_dBFS = value;
         },
         "RU IQ reference level mapped to the maximum RF power")
      ->check(CLI::Range(-std::numeric_limits<float>::infinity(), 0.0f))
      ->check([&config](const std::string& value) -> std::string {
        if (std::holds_alternative<ru_ofh_legacy_scaling_config>(config.iq_scaling_config)) {
          return "IQ scaling and RU reference level cannot be set at the same time";
        }
        return "";
      });

  app.add_option_function<std::string>(
         "--subcarrier_rms_backoff_dB",
         [&config](std::string value) {
           if (!std::holds_alternative<ru_ofh_scaling_config>(config.iq_scaling_config)) {
             config.iq_scaling_config.emplace<ru_ofh_scaling_config>();
           }
           // By default, subcarrier backoff is set to 'auto', i.e., normalize bandwidth based on number of
           // subcarriers. If another value is specified, it is parsed.
           if (value != "auto") {
             // It is safe to convert the value without try-catch because it has been already checked.
             float backoff                                                                       = std::stof(value);
             std::get<ru_ofh_scaling_config>(config.iq_scaling_config).subcarrier_rms_backoff_dB = backoff;
           }
         },
         "Power back-off attenuation applied to all subcarriers with respect to the RU reference level")
      ->check(CLI::Range(0.0f, std::numeric_limits<float>::infinity()) | CLI::IsMember({"auto"}))
      ->check([&config](const std::string& value) -> std::string {
        if (std::holds_alternative<ru_ofh_legacy_scaling_config>(config.iq_scaling_config)) {
          return "IQ scaling and RU subcarrier back-off cannot be set at the same time";
        }
        return "";
      });

  app.add_option_function<float>(
         "--iq_scaling",
         [&config](float value) {
           if (!std::holds_alternative<ru_ofh_legacy_scaling_config>(config.iq_scaling_config)) {
             config.iq_scaling_config.emplace<ru_ofh_legacy_scaling_config>();
           }
           std::get<ru_ofh_legacy_scaling_config>(config.iq_scaling_config).iq_scaling = value;
         },
         "Linear IQ scaling factor. It replaces the RU reference level and subarrier back-off")
      ->check(CLI::Range(0.0, 100.0))
      ->check([&config](const std::string& value) -> std::string {
        if (std::holds_alternative<ru_ofh_scaling_config>(config.iq_scaling_config)) {
          return "IQ scaling cannot be used in conjunction with other scaling parameters.";
        }
        return "";
      });

  auto cplane_prach_fft_size_check = [](const std::string& value) -> std::string {
    if ((value == "128") || (value == "256") || (value == "512") || (value == "1024") || (value == "1536") ||
        (value == "2048") || (value == "3072") || (value == "4096")) {
      return {};
    }

    return "Supported Open Fronthaul Radio Unit C-Plane PRACH FFT sizes are [128, 256, 512, 1024, 1536, 2048, 3072, "
           "4096]";
  };

  app.add_option_function<unsigned>(
         "--cplane_prach_fft_len",
         [&config](unsigned value) {
           switch (value) {
             case 0:
               config.c_plane_prach_fft_len = ofh::cplane_fft_size::fft_noop;
               break;
             case 1536:
               config.c_plane_prach_fft_len = ofh::cplane_fft_size::fft_1536;
               break;
             case 3072:
               config.c_plane_prach_fft_len = ofh::cplane_fft_size::fft_3072;
               break;
             default:
               config.c_plane_prach_fft_len = static_cast<ofh::cplane_fft_size>(std::log2(value));
               break;
           }
         },
         "PRACH FFT length (used in C-Plane Type-3 messages)")
      ->capture_default_str()
      ->check(cplane_prach_fft_size_check);

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

  auto validate_iq_scaling_input = [](CLI::App& cli_app) {
    unsigned ref_level_count  = cli_app.count("--ru_reference_level_dBFS");
    unsigned backoff_count    = cli_app.count("--subcarrier_rms_backoff_dB");
    unsigned iq_scaling_count = cli_app.count("--iq_scaling");

    if (iq_scaling_count && (ref_level_count || backoff_count)) {
      report_error(
          "RU IQ scaling parameter cannot be used if RU reference level and subcarrier RMS back-off are set\n");
    }

    if (backoff_count > 0 && ref_level_count == 0) {
      report_error("RU reference level is required if subcarrier RMS back-off is set\n");
    }
  };

  // Post-parsing callback to validate that compression method and bitwidth parameters were both specified or both set
  // to default.
  app.callback([&]() {
    validate_compression_input(app, "dl");
    validate_compression_input(app, "ul");
    validate_compression_input(app, "prach");
    validate_iq_scaling_input(app);
  });
}

static void configure_cli11_ru_ofh_cells_args(CLI::App& app, ru_ofh_unit_cell_config& config)
{
  configure_cli11_ru_ofh_base_cell_args(app, config.cell);
  add_option(app,
             "--network_interface",
             config.network_interface,
             "Network interface name for raw sockets. PCIe (or other bus) port identifier when using DPDK")
      ->capture_default_str();
  add_option(app, "--enable_promiscuous", config.enable_promiscuous_mode, "Promiscuous mode flag")
      ->capture_default_str();
  add_option(app, "--check_link_status", config.check_link_status, "Ethernet link status checking flag")
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
  auto* base_cell_group = app.add_option_group("base_cell");
  configure_cli11_ru_ofh_base_cell_args(*base_cell_group, config.base_cell_cfg);
  base_cell_group->parse_complete_callback([&config, &app]() {
    for (auto& cell : config.config.cells) {
      cell.cell = config.base_cell_cfg;
    };
    if (app.get_option("--cells")->get_callback_run()) {
      app.get_option("--cells")->run_callback();
    }
  });

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
  app_helpers::add_log_option(app, log_params.ofh_level, "--ofh_level", "Open Fronthaul log level");
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

static void configure_cli11_txrx_affinity_args(CLI::App& app, os_sched_affinity_bitmask& mask)
{
  add_option_function<std::string>(
      app,
      "--ru_txrx_cpus",
      [&mask](const std::string& value) { parse_affinity_mask(mask, value, "txrx_cpus"); },
      "Number of CPUs used for the Radio Unit tasks");
}

static void configure_cli11_expert_execution_args(CLI::App& app, ru_ofh_unit_expert_execution_config& config)
{
  // Affinities section.
  CLI::App* affinities_subcmd = add_subcommand(app, "affinities", "gNB CPU affinities configuration")->configurable();
  add_option_function<std::string>(
      *affinities_subcmd,
      "--ru_timing_cpu",
      [&config](const std::string& value) { parse_affinity_mask(config.ru_timing_cpu, value, "ru_timing_cpu"); },
      "CPU used for timing in the Radio Unit");

  // RU txrx affinity section.
  add_option_cell(
      *affinities_subcmd,
      "--ofh",
      [&config](const std::vector<std::string>& values) {
        config.txrx_affinities.resize(values.size());

        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("RU tx-rx thread CPU affinities",
                          "RU tx-rx thread CPU affinities config #" + std::to_string(i));
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras();
          configure_cli11_txrx_affinity_args(subapp, config.txrx_affinities[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Sets the CPU affinities configuration for RU cells tx-rx threads. Number of entries specified defines the "
      "number of tx-rx threads created. By default if no entries is specifies, the number of tx-rx threads equals the "
      "number of RU cells");

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

#ifdef DPDK_FOUND
static void configure_cli11_hal_args(CLI::App& app, std::optional<ru_ofh_unit_hal_config>& config)
{
  config.emplace();

  add_option(app, "--eal_args", config->eal_args, "EAL configuration parameters used to initialize DPDK");
}
#endif

static void configure_cli11_metrics_args(CLI::App& app, ru_ofh_unit_metrics_config& config)
{
  CLI::App* layers_subcmd = add_subcommand(app, "layers", "Layer basis metrics configuration")->configurable();
  add_option(*layers_subcmd, "--enable_ru", config.enable_ru_metrics, "Enable Radio Unit metrics")
      ->capture_default_str();
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

  // HAL section only available when DPDK is present.
#ifdef DPDK_FOUND
  CLI::App* hal_subcmd = add_subcommand(app, "hal", "HAL configuration")->configurable();
  configure_cli11_hal_args(*hal_subcmd, parsed_cfg.config.hal_config);
#endif

  // Metrics section.
  app_helpers::configure_cli11_with_metrics_appconfig_schema(app, parsed_cfg.config.metrics_cfg.metrics_cfg);
  CLI::App* metrics_subcmd = add_subcommand(app, "metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*metrics_subcmd, parsed_cfg.config.metrics_cfg);
}

#ifdef DPDK_FOUND
static void manage_hal_optional(CLI::App& app, std::optional<ru_ofh_unit_hal_config>& hal_config)
{
  // Clean the HAL optional.
  if (auto subcmd = app.get_subcommand("hal"); subcmd->count_all() == 0) {
    hal_config.reset();
    // As HAL configuration is optional, disable the command when it is not present in the configuration.
    subcmd->disabled();
  }
}
#endif

void srsran::autoderive_ru_ofh_parameters_after_parsing(CLI::App& app, ru_ofh_unit_parsed_config& parsed_cfg)
{
#ifdef DPDK_FOUND
  manage_hal_optional(app, parsed_cfg.config.hal_config);
#endif
}
