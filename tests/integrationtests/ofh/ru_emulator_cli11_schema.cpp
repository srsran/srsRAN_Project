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

#include "ru_emulator_cli11_schema.h"
#include "helpers.h"
#include "ru_emulator_appconfig.h"
#include "srsran/support/cli11_utils.h"
#include "srsran/support/config_parsers.h"

using namespace srsran;

static void configure_cli11_log_args(CLI::App& app, ru_emulator_log_appconfig& log_params)
{
  auto level_check = [](const std::string& value) -> std::string {
    if (value == "info" || value == "debug" || value == "warning" || value == "error") {
      return {};
    }
    return "Log level value not supported. Accepted values [info,debug,warning,error]";
  };

  app.add_option("--filename", log_params.filename, "Log file output path")->capture_default_str();
  app.add_option("--level", log_params.level, "Log level")->capture_default_str()->check(level_check);
}

static void configure_cli11_ru_emu_dpdk_args(CLI::App& app, ru_emulator_dpdk_appconfig& config)
{
  app.add_option("--eal_args", config.eal_args, "EAL configuration parameters used to initialize DPDK");
}

static void configure_cli11_ru_emu_args(CLI::App& app, ru_emulator_ofh_appconfig& config)
{
  app.add_option_function<unsigned>(
         "--bandwidth",
         [&config](unsigned value) { config.bandwidth = MHz_to_bs_channel_bandwidth(value); },
         "Channel bandwidth in MHz")
      ->check([](const std::string& value) -> std::string {
        std::stringstream ss(value);
        unsigned          bw;
        ss >> bw;
        const std::string& error_message = "Error in the channel bandwidth property. Valid values "
                                           "[5,10,15,20,25,30,40,50,60,70,80,90,100]";

        return is_valid_bw(bw) ? "" : error_message;
      });

  auto compression_method_check = [](const std::string& value) -> std::string {
    if (value == "none" || value == "bfp") {
      return {};
    }

    return "Compression method not supported. Accepted values [none, bfp]";
  };

  app.add_option("--compr_method_ul", config.ul_compr_method, "Uplink compression method")
      ->capture_default_str()
      ->check(compression_method_check);
  app.add_option("--compr_bitwidth_ul", config.ul_compr_bitwidth, "Uplink compression bit width")
      ->capture_default_str()
      ->check(CLI::IsMember({9, 16}));
  app.add_option("--network_interface", config.network_interface, "PCIe identifier of network device")
      ->capture_default_str();
  app.add_option("--ru_mac_addr", config.ru_mac_address, "Radio Unit MAC address")->capture_default_str();
  app.add_option("--du_mac_addr", config.du_mac_address, "Distributed Unit MAC address")->capture_default_str();
  app.add_option("--vlan_tag", config.vlan_tag, "V-LAN identifier")->capture_default_str()->check(CLI::Range(1, 4094));
  app.add_option("--ul_port_id", config.ru_ul_port_id, "RU uplink port identifier")->capture_default_str();
}

void srsran::configure_cli11_with_ru_emulator_appconfig_schema(CLI::App& app, ru_emulator_appconfig& ru_emu_parsed_cfg)
{
  // Logging section.
  CLI::App* log_subcmd = app.add_subcommand("log", "Logging configuration")->configurable();
  configure_cli11_log_args(*log_subcmd, ru_emu_parsed_cfg.log_cfg);

  // RU emulators section.
  CLI::App* ru_subcmd =
      app.add_subcommand("ru_emu", "Open Fronthaul Radio Unit emulator configuration")->configurable();

  // Cell parameters.
  ru_subcmd->add_option_function<std::vector<std::string>>(
      "--cells",
      [&ru_emu_parsed_cfg](const std::vector<std::string>& values) {
        ru_emu_parsed_cfg.ru_cfg.resize(values.size());

        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("RU emulators");
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras(CLI::config_extras_mode::error);
          configure_cli11_ru_emu_args(subapp, ru_emu_parsed_cfg.ru_cfg[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Sets the RU emulator configuration");

  CLI::App* dpdk_subcmd = app.add_subcommand("dpdk", "DPDK configuration")->configurable();
  configure_cli11_ru_emu_dpdk_args(*dpdk_subcmd, ru_emu_parsed_cfg.dpdk_config);
}
