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

#include "ru_sdr_config_cli11_schema.h"
#include "apps/helpers/logger/logger_appconfig_cli11_utils.h"
#include "apps/helpers/metrics/metrics_config_cli11_schema.h"
#include "apps/services/worker_manager/cli11_cpu_affinities_parser_helper.h"
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
  auto tx_mode_check = [](const std::string& value) -> std::string {
    if (value == "continuous" || value == "discontinuous" || value == "same-port") {
      return {};
    }
    return "Invalid transmission mode. Accepted values [continuous,discontinuous,same-port]";
  };

  add_option(app,
             "--low_phy_dl_throttling",
             config.lphy_dl_throttling,
             "System time-based throttling.\n"
             "Determines a minimum baseband processor period time between downlink packets. It is expressed as a \n"
             "fraction of the time equivalent to the number of samples in the baseband buffer. Set to 0.9 to ensure \n"
             "that the downlink packets are processed with a minimum period of 90% of the buffer duration.\n"
             "Set to zero to disable this feature.")
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
             "Specifies the power ramping time in microseconds, it proactively initiates the transmission and \n"
             "mitigates transient effects.")
      ->capture_default_str();
}

static void configure_cli11_ru_sdr_args(CLI::App& app, ru_sdr_unit_config& config)
{
  static const char* time_format = "%Y-%m-%d %H:%M:%S";

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
  add_option_function<std::string>(
      app,
      "--start_time",
      [&config](std::string value) {
        //
        if (value.empty()) {
          config.start_time = std::nullopt;
          return;
        }

        //
        std::tm            tm;
        std::istringstream ss(value);
        ss >> std::get_time(&tm, time_format);

        if (ss.fail()) {
          config.start_time = std::nullopt;
          return;
        }

        config.start_time.emplace(std::chrono::system_clock::from_time_t(std::mktime(&tm)));
      },
      "Optional radio start time.\n"
      "The time must be with format %Y-%m-%d %H:%M:%S.")
      ->check([](std::string value) -> std::string {
        // Try parsing the time.
        std::tm            tm;
        std::istringstream ss(value);
        ss >> std::get_time(&tm, time_format);

        if (ss.fail()) {
          return fmt::format(
              "The starting time '{}' format does not match the expected format: '{}'.\n", value, time_format);
        }

        // Success.
        return {};
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
  app_helpers::add_log_option(app, log_params.radio_level, "--radio_level", "Radio log level");
  app_helpers::add_log_option(app, log_params.phy_level, "--phy_level", "PHY log level");
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
        } else if (value == "triple") {
          execution_profile = lower_phy_thread_profile::triple;
        }
      },
      "Lower physical layer executor profile [single, dual, triple].\n"
      " - single: one task worker for all the lower physical layer task executors.\n"
      " - dual: two task workers - one for the downlink and one for the uplink.\n"
      " - triple: dedicated task workers for each of the subtasks (demodulation, reception and transmission).")
      ->check([](const std::string& value) -> std::string {
        if ((value == "single") || (value == "dual") || (value == "triple")) {
          return "";
        }

        return "Invalid executor profile. Valid profiles are: single, dual, and triple.";
      })
      ->default_function([&execution_profile]() -> std::string {
        switch (execution_profile) {
          case lower_phy_thread_profile::blocking:
            return "blocking";
          case lower_phy_thread_profile::single:
            return "single";
          case lower_phy_thread_profile::dual:
            return "dual";
          case lower_phy_thread_profile::triple:
            return "triple";
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

static void configure_cli11_metrics_args(CLI::App& app, ru_sdr_unit_metrics_config& config)
{
  CLI::App* layers_subcmd = add_subcommand(app, "layers", "Layer basis metrics configuration")->configurable();
  add_option(*layers_subcmd, "--enable_ru", config.enable_ru_metrics, "Enable Radio Unit metrics");
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

  // Metrics section.
  app_helpers::configure_cli11_with_metrics_appconfig_schema(app, parsed_cfg.metrics_cfg.metrics_cfg);
  CLI::App* metrics_subcmd = add_subcommand(app, "metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*metrics_subcmd, parsed_cfg.metrics_cfg);
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
