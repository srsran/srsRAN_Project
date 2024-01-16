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

#include "radio_config_zmq_validator.h"
#include "fmt/format.h"
#include <regex>

using namespace srsran;

static bool validate_clock_sources(const radio_configuration::clock_sources& sources)
{
  if (sources.clock != radio_configuration::clock_sources::source::DEFAULT) {
    fmt::print("Only 'default' clock source is available.\n");
    return false;
  }

  if (sources.sync != radio_configuration::clock_sources::source::DEFAULT) {
    fmt::print("Only 'default' sync source is available.\n");
    return false;
  }

  return true;
}

static bool validate_lo_freq(const radio_configuration::lo_frequency& lo_freq)
{
  if (!std::isnormal(lo_freq.center_frequency_hz)) {
    fmt::print("The center frequency must be non-zero, NAN nor infinite.\n");
    return false;
  }

  if (lo_freq.lo_frequency_hz != 0.0) {
    fmt::print("The custom LO frequency is not currently supported.\n");
    return false;
  }

  return true;
}

static bool validate_channel_args(const std::string& channel_args)
{
  std::cmatch cmatch;
  std::regex  exp_tcp_socket(R"(^tcp:\/\/((([a-zA-Z0-9_\-])\.*)*([a-zA-Z0-9])*|\*):[0-9]*$)");
  std::regex_match(channel_args.c_str(), cmatch, exp_tcp_socket);

  if (cmatch.empty()) {
    fmt::print("Channel arguments {} does not describe a valid TCP socket.\n", channel_args);
    return false;
  }

  return true;
}

static bool validate_channel(const radio_configuration::channel& channel)
{
  if (!validate_lo_freq(channel.freq)) {
    return false;
  }

  if (std::isnan(channel.gain_dB) || std::isinf(channel.gain_dB)) {
    fmt::print("Channel gain must not be NAN nor infinite.\n");
    return false;
  }

  if (!validate_channel_args(channel.args)) {
    return false;
  }

  return true;
}

static bool validate_stream(const radio_configuration::stream& stream)
{
  if (stream.channels.empty()) {
    fmt::print("Streams must contain at least one channel.\n");
    return false;
  }

  for (const radio_configuration::channel& channel : stream.channels) {
    if (!validate_channel(channel)) {
      return false;
    }
  }

  if (!stream.args.empty()) {
    fmt::print("Stream arguments are not currently supported.\n");
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
  if (otw_format != radio_configuration::over_the_wire_format::DEFAULT) {
    fmt::print("Only default OTW format is currently supported.\n");
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

bool radio_config_zmq_config_validator::is_configuration_valid(const radio_configuration::radio& config) const
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
    if (!validate_stream(tx_stream)) {
      return false;
    }
  }

  for (const radio_configuration::stream& rx_stream : config.rx_streams) {
    if (!validate_stream(rx_stream)) {
      return false;
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

  if (config.discontinuous_tx) {
    fmt::print("Discontinuous transmission mode is not supported by the ZMQ radio.\n");
    return false;
  }

  if (config.power_ramping_us != 0.0F) {
    fmt::print("Power ramping is not supported by the ZMQ radio.\n");
    return false;
  }

  return true;
}
