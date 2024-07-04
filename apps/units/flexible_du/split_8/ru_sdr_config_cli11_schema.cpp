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

#include "ru_sdr_config_cli11_schema.h"
#include "apps/services/logger/logger_appconfig_cli11_utils.h"
#include "apps/units/flexible_du/support/cli11_cpu_affinities_parser_helper.h"
#include "ru_sdr_config.h"
#include "srsran/support/cli11_utils.h"
#include "srsran/support/config_parsers.h"

using namespace srsran;

static void configure_cli11_amplitude_control_args(CLI::App& app, amplitude_control_unit_config& amplitude_params)
{
  add_option(app,
             "--tx_gain_backoff",
             amplitude_params.gain_backoff_dB,
             "Gain back-off to accommodate the signal PAPR in decibels")
      ->capture_default_str();
  add_option(app, "--enable_clipping", amplitude_params.enable_clipping, "Signal clipping")->capture_default_str();
  add_option(app, "--ceiling", amplitude_params.power_ceiling_dBFS, "Clipping ceiling referenced to full scale")
      ->capture_default_str();
}

static void configure_cli11_ru_sdr_expert_args(CLI::App& app, ru_sdr_unit_expert_config& config)
{
  auto buffer_size_policy_check = [](const std::string& value) -> std::string {
    if (value == "auto" || value == "single-packet" || value == "half-slot" || value == "slot" ||
        value == "optimal-slot") {
      return {};
    }
    return "Invalid DL buffer size policy. Accepted values [auto,single-packet,half-slot,slot,optimal-slot]";
  };

  auto tx_mode_check = [](const std::string& value) -> std::string {
    if (value == "continuous" || value == "discontinuous" || value == "same-port") {
      return {};
    }
    return "Invalid transmission mode. Accepted values [continuous,discontinuous,same-port]";
  };

  add_option(app,
             "--low_phy_dl_throttling",
             config.lphy_dl_throttling,
             "Throttles the lower PHY DL baseband generation. The range is (0, 1). Set it to zero to disable it.")
      ->capture_default_str();
  add_option(app,
             "--tx_mode",
             config.transmission_mode,
             "Selects a radio transmission mode. Discontinuous modes are not supported by all radios.\n"
             "  continuous:    the TX chain is always active.\n"
             "  discontinuous: the transmitter stops when there is no data to transmit.\n"
             "  same-port:     the radio transmits and receives from the same antenna port.\n")
      ->capture_default_str()
      ->check(tx_mode_check);

  add_option(app,
             "--power_ramping_time_us",
             config.power_ramping_time_us,
             "Specifies the power ramping time in microseconds, it proactively initiates the transmission and "
             "mitigates transient effects.")
      ->capture_default_str();
  add_option(app,
             "--dl_buffer_size_policy",
             config.dl_buffer_size_policy,
             "Selects the size policy of the baseband buffers that pass DL samples from the lower PHY to the radio.")
      ->capture_default_str()
      ->check(buffer_size_policy_check);
}

static void configure_cli11_ru_sdr_args(CLI::App& app, ru_sdr_unit_config& config)
{
  add_option(app, "--srate", config.srate_MHz, "Sample rate in MHz")->capture_default_str();
  add_option(app, "--device_driver", config.device_driver, "Device driver name")->capture_default_str();
  add_option(app, "--device_args", config.device_arguments, "Optional device arguments")->capture_default_str();
  add_option(app, "--tx_gain", config.tx_gain_dB, "Transmit gain in decibels")->capture_default_str();
  add_option(app, "--rx_gain", config.rx_gain_dB, "Receive gain in decibels")->capture_default_str();
  add_option(app, "--freq_offset", config.center_freq_offset_Hz, "Center frequency offset in hertz")
      ->capture_default_str();
  add_option(app, "--clock_ppm", config.calibrate_clock_ppm, "Clock calibration in PPM.")->capture_default_str();
  add_option(app, "--lo_offset", config.lo_offset_MHz, "LO frequency offset in MHz")->capture_default_str();
  add_option(app, "--clock", config.clock_source, "Clock source")->capture_default_str();
  add_option(app, "--sync", config.synch_source, "Time synchronization source")->capture_default_str();
  add_option(app, "--otw_format", config.otw_format, "Over-the-wire format")->capture_default_str();
  add_option_function<std::string>(
      app,
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
  CLI::App* amplitude_control_subcmd = add_subcommand(app, "amplitude_control", "Amplitude control parameters");
  configure_cli11_amplitude_control_args(*amplitude_control_subcmd, config.amplitude_cfg);

  // Expert configuration.
  CLI::App* expert_subcmd = add_subcommand(app, "expert_cfg", "Generic Radio Unit expert configuration");
  configure_cli11_ru_sdr_expert_args(*expert_subcmd, config.expert_cfg);
}

static void configure_cli11_log_args(CLI::App& app, ru_sdr_unit_logger_config& log_params)
{
  app_services::add_log_option(app, log_params.radio_level, "--radio_level", "Radio log level");
  app_services::add_log_option(app, log_params.phy_level, "--phy_level", "PHY log level");
}

static void configure_cli11_cell_affinity_args(CLI::App& app, ru_sdr_unit_cpu_affinities_cell_config& config)
{
  add_option_function<std::string>(
      app,
      "--l1_dl_cpus",
      [&config](const std::string& value) { parse_affinity_mask(config.l1_dl_cpu_cfg.mask, value, "l1_dl_cpus"); },
      "CPU cores assigned to L1 downlink tasks");

  add_option_function<std::string>(
      app,
      "--l1_ul_cpus",
      [&config](const std::string& value) { parse_affinity_mask(config.l1_ul_cpu_cfg.mask, value, "l1_ul_cpus"); },
      "CPU cores assigned to L1 uplink tasks");

  add_option_function<std::string>(
      app,
      "--l1_dl_pinning",
      [&config](const std::string& value) {
        config.l1_dl_cpu_cfg.pinning_policy = to_affinity_mask_policy(value);
        if (config.l1_dl_cpu_cfg.pinning_policy == sched_affinity_mask_policy::last) {
          report_error("Incorrect value={} used in {} property", value, "l1_dl_pinning");
        }
      },
      "Policy used for assigning CPU cores to L1 downlink tasks");

  add_option_function<std::string>(
      app,
      "--l1_ul_pinning",
      [&config](const std::string& value) {
        config.l1_ul_cpu_cfg.pinning_policy = to_affinity_mask_policy(value);
        if (config.l1_ul_cpu_cfg.pinning_policy == sched_affinity_mask_policy::last) {
          report_error("Incorrect value={} used in {} property", value, "l1_ul_pinning");
        }
      },
      "Policy used for assigning CPU cores to L1 uplink tasks");

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

static void configure_cli11_lower_phy_threads_args(CLI::App& app, lower_phy_thread_profile& execution_profile)
{
  add_option_function<std::string>(
      app,
      "--execution_profile",
      [&execution_profile](const std::string& value) {
        if (value == "single") {
          execution_profile = lower_phy_thread_profile::single;
        } else if (value == "dual") {
          execution_profile = lower_phy_thread_profile::dual;
        } else if (value == "quad") {
          execution_profile = lower_phy_thread_profile::quad;
        }
      },
      "Lower physical layer executor profile [single, dual, quad].")
      ->check([](const std::string& value) -> std::string {
        if ((value == "single") || (value == "dual") || (value == "quad")) {
          return "";
        }

        return "Invalid executor profile. Valid profiles are: single, dual and quad.";
      })
      ->default_function([&execution_profile]() -> std::string {
        switch (execution_profile) {
          case lower_phy_thread_profile::blocking:
            return "blocking";
          case lower_phy_thread_profile::dual:
            return "dual";
          case lower_phy_thread_profile::quad:
            return "quad";
          case lower_phy_thread_profile::single:
            return "single";
          default:
            break;
        }
        return {};
      })
      ->capture_default_str();
}

static void configure_cli11_expert_execution_args(CLI::App& app, ru_sdr_unit_expert_execution_config& config)
{
  // Threads section.
  CLI::App* threads_subcmd = add_subcommand(app, "threads", "Threads configuration")->configurable();

  // Lower PHY threads.
  CLI::App* lower_phy_threads_subcmd =
      add_subcommand(*threads_subcmd, "lower_phy", "Lower PHY thread configuration")->configurable();
  configure_cli11_lower_phy_threads_args(*lower_phy_threads_subcmd, config.threads.execution_profile);

  // Cell affinity section.
  add_option_cell(
      app,
      "--cell_affinities",
      [&config](const std::vector<std::string>& values) {
        config.cell_affinities.resize(values.size());

        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("SDR Expert execution cell CPU affinities",
                          "SDR Expert execution cell CPU affinities config, item #" + std::to_string(i));
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras();
          configure_cli11_cell_affinity_args(subapp, config.cell_affinities[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Sets the cell CPU affinities configuration on a per cell basis");
}

void srsran::configure_cli11_with_ru_sdr_config_schema(CLI::App& app, srsran::ru_sdr_unit_config& parsed_cfg)
{
  /// RU SDR section.
  CLI::App* ru_sdr_subcmd = add_subcommand(app, "ru_sdr", "SDR Radio Unit configuration")->configurable();
  configure_cli11_ru_sdr_args(*ru_sdr_subcmd, parsed_cfg);

  // Loggers section.
  CLI::App* log_subcmd = add_subcommand(app, "log", "Logging configuration")->configurable();
  configure_cli11_log_args(*log_subcmd, parsed_cfg.loggers);

  // Expert execution section.
  CLI::App* expert_subcmd = add_subcommand(app, "expert_execution", "Expert execution configuration")->configurable();
  configure_cli11_expert_execution_args(*expert_subcmd, parsed_cfg.expert_execution_cfg);
}

void srsran::autoderive_ru_sdr_parameters_after_parsing(CLI::App&           app,
                                                        ru_sdr_unit_config& parsed_cfg,
                                                        unsigned            nof_cells)
{
  if (parsed_cfg.expert_execution_cfg.cell_affinities.size() < nof_cells) {
    parsed_cfg.expert_execution_cfg.cell_affinities.resize(nof_cells);
  }

  // Set the lower PHY to blocking for ZMQ.
  if (parsed_cfg.device_driver == "zmq") {
    parsed_cfg.expert_execution_cfg.threads.execution_profile = lower_phy_thread_profile::blocking;
  }
}
