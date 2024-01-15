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

#include "radio_config_uhd_validator.h"
#include "radio_uhd_multi_usrp.h"
#include "fmt/format.h"
#include <regex>
#include <set>

using namespace srsran;

static bool validate_clock_sources(const radio_configuration::clock_sources& sources)
{
  static const std::set<radio_configuration::clock_sources::source> valid_clock_sources = {
      radio_configuration::clock_sources::source::DEFAULT,
      radio_configuration::clock_sources::source::INTERNAL,
      radio_configuration::clock_sources::source::EXTERNAL,
      radio_configuration::clock_sources::source::GPSDO};

  if (valid_clock_sources.count(sources.clock) == 0) {
    fmt::print("Invalid clock source.\n");
    return false;
  }

  if (valid_clock_sources.count(sources.sync) == 0) {
    fmt::print("Invalid sync source.\n");
    return false;
  }

  return true;
}

static bool validate_lo_freq(const radio_configuration::lo_frequency& lo_freq, bool is_tx)
{
  if (!std::isnormal(lo_freq.center_frequency_hz)) {
    fmt::print("{} center frequency must be non-zero, NAN nor infinite.\n", is_tx ? "TX" : "RX");
    return false;
  }

  return true;
}

static bool validate_args_format(const std::string& args)
{
  // Empty arguments are valid.
  if (args.empty()) {
    return true;
  }

  // Channel argument example: channel_arg0=123,channel_arg2=foo
  std::cmatch cmatch;
  std::regex  exp_channel_args(R"(^((([A-z]|_|[0-9])*\=([A-z]|_|[0-9])*),*)*$)");
  std::regex_match(args.c_str(), cmatch, exp_channel_args);

  if (cmatch.empty()) {
    fmt::print("Arguments '{}' format is invalid.\n", args);
    return false;
  }

  return true;
}

static bool validate_channel(const radio_configuration::channel& channel, bool is_tx)
{
  if (!validate_lo_freq(channel.freq, is_tx)) {
    return false;
  }

  if (std::isnan(channel.gain_dB) || std::isinf(channel.gain_dB)) {
    fmt::print("Channel gain must not be NAN nor infinite.\n");
    return false;
  }

  if (!validate_args_format(channel.args)) {
    return false;
  }

  return true;
}

static bool validate_stream(const radio_configuration::stream& stream, bool is_tx)
{
  if (stream.channels.empty()) {
    fmt::print("Streams must contain at least one channel.\n");
    return false;
  }

  for (const radio_configuration::channel& channel : stream.channels) {
    if (!validate_channel(channel, is_tx)) {
      return false;
    }
  }

  if (!validate_args_format(stream.args)) {
    return false;
  }

  return true;
}

static bool validate_sampling_rate(double sampling_rate)
{
  if (!std::isnormal(sampling_rate)) {
    fmt::print("The sampling rate must be non-zero, NAN nor infinite.\n");
    return false;
  }

  if (sampling_rate < 0.0) {
    fmt::print("The sampling rate must be greater than zero.\n");
    return false;
  }

  return true;
}

static bool validate_otw_format(radio_configuration::over_the_wire_format otw_format)
{
  static const std::set<radio_configuration::over_the_wire_format> valid_otw_formats = {
      radio_configuration::over_the_wire_format::DEFAULT,
      radio_configuration::over_the_wire_format::SC12,
      radio_configuration::over_the_wire_format::SC16};

  if (valid_otw_formats.count(otw_format) == 0) {
    fmt::print("The selected over the wire format is not supported.\n");
    return false;
  }

  return true;
}

static bool validate_log_level(const std::string& log_level)
{
  // Converts to a logger level.
  srslog::basic_levels level = srslog::str_to_basic_level(log_level);

  // Convert the logger level back to a string.
  std::string actual_log_level = srslog::basic_level_to_string(level);

  // Check if the strings are equal without considering the case.
  bool are_equal = std::equal(
      log_level.begin(), log_level.end(), actual_log_level.begin(), actual_log_level.end(), [](char a, char b) {
        return std::tolower(a) == std::tolower(b);
      });

  // The log level is not valid if the strings are different.
  if (!are_equal) {
    fmt::print("Log level {} does not correspond to an actual logger level.\n", log_level);
    return false;
  }

  return true;
}

bool radio_config_uhd_config_validator::is_configuration_valid(const radio_configuration::radio& config) const
{
  if (!validate_clock_sources(config.clock)) {
    return false;
  }

  if (config.tx_streams.size() != config.rx_streams.size()) {
    fmt::print("Transmit and receive number of streams must be equal.\n");
    return false;
  }

  if (config.tx_streams.empty()) {
    fmt::print("At least one transmit and one receive stream must be configured.\n");
    return false;
  }

  for (const radio_configuration::stream& tx_stream : config.tx_streams) {
    if (!validate_stream(tx_stream, true)) {
      return false;
    }
  }

  for (const radio_configuration::stream& rx_stream : config.rx_streams) {
    if (!validate_stream(rx_stream, false)) {
      return false;
    }
  }

  if (config.tx_streams.size() != config.rx_streams.size()) {
    fmt::print("Transmit and receive number of streams must be equal.\n");
    return false;
  }

  if (config.args.find("type=b2") != std::string::npos) {
    for (unsigned i_stream = 0, nof_streams = config.tx_streams.size(); i_stream != nof_streams; ++i_stream) {
      if ((config.tx_streams[i_stream].channels.size() != config.rx_streams[i_stream].channels.size())) {
        fmt::print("B2x0 devices do not support different number of transmit and receive antennas.\n");
        return false;
      }
    }
  }

  if (!validate_sampling_rate(config.sampling_rate_hz)) {
    return false;
  }

  if (!validate_otw_format(config.otw_format)) {
    return false;
  }

  if (!validate_log_level(config.log_level)) {
    return false;
  }

  if (config.discontinuous_tx && (config.power_ramping_us < 0)) {
    fmt::print("Power ramping time, i.e., {:.1f} us, must be positive or zero.\n", config.power_ramping_us);
    return false;
  }

  return true;
}
