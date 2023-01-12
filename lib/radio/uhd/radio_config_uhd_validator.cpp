/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "radio_config_uhd_validator.h"
#include "radio_uhd_multi_usrp.h"
#include "fmt/format.h"
#include <regex>
#include <set>

using namespace srsgnb;

static bool validate_clock_sources(const radio_uhd_device::properties&       properties,
                                   const radio_configuration::clock_sources& sources)
{
  if (properties.supported_clock_sources.count(sources.clock) == 0) {
    fmt::print("Invalid clock source.\n");
    return false;
  }

  if (properties.supported_sync_sources.count(sources.sync) == 0) {
    fmt::print("Invalid sync source.\n");
    return false;
  }

  return true;
}

static bool validate_lo_freq(const radio_uhd_device::properties&      properties,
                             const radio_configuration::lo_frequency& lo_freq,
                             bool                                     is_tx)
{
  if (!std::isnormal(lo_freq.center_frequency_hz)) {
    fmt::print("The center frequency must be non-zero, NAN nor infinite.\n");
    return false;
  }

  if (lo_freq.lo_frequency_hz != 0.0) {
    fmt::print("The custom LO frequency is not currently supported.\n");
    return false;
  }

  const uhd::meta_range_t& range = (is_tx ? properties.tx_lo_freq_range : properties.rx_lo_freq_range);

  bool is_step_valid = true;
  if (std::isnormal(range.step())) {
    double nsteps = (lo_freq.center_frequency_hz - range.start()) / range.step();
    is_step_valid = (std::ceil(nsteps) == std::floor(nsteps));
  }

  if ((lo_freq.center_frequency_hz < range.start()) || (lo_freq.center_frequency_hz > range.stop()) ||
      (!is_step_valid)) {
    fmt::print("{} Channel frequency {} MHz is out of the range (min. {} MHz, max. {} MHz, step {} Hz).\n",
               is_tx ? "TX" : "RX",
               lo_freq.center_frequency_hz / 1e6,
               range.start() / 1e6,
               range.stop() / 1e6,
               range.step());
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

static bool validate_channel(const radio_uhd_device::properties& properties,
                             const radio_configuration::channel& channel,
                             bool                                is_tx)
{
  if (!validate_lo_freq(properties, channel.freq, is_tx)) {
    return false;
  }

  if (std::isnan(channel.gain_dB) || std::isinf(channel.gain_dB)) {
    fmt::print("Channel gain must not be NAN nor infinite.\n");
    return false;
  }

  const uhd::meta_range_t& range = (is_tx ? properties.tx_gain_range : properties.rx_gain_range);

  bool is_step_valid = true;
  if (std::isnormal(range.step())) {
    double nsteps = (channel.gain_dB - range.start()) / range.step();
    is_step_valid = (std::ceil(nsteps) == std::floor(nsteps));
  }

  if ((channel.gain_dB < range.start()) || (channel.gain_dB > range.stop()) || (!is_step_valid)) {
    fmt::print("{} Channel gain {} is out of the range (min. {}, max. {}, step {}).\n",
               is_tx ? "TX" : "RX",
               channel.gain_dB,
               range.start(),
               range.stop(),
               range.step());
    return false;
  }

  if (!validate_args_format(channel.args)) {
    return false;
  }

  return true;
}

static bool
validate_stream(const radio_uhd_device::properties& properties, const radio_configuration::stream& stream, bool is_tx)
{
  if (stream.channels.empty()) {
    fmt::print("Streams must contain at least one channel.\n");
    return false;
  }

  for (const radio_configuration::channel& channel : stream.channels) {
    if (!validate_channel(properties, channel, is_tx)) {
      return false;
    }
  }

  if (!validate_args_format(stream.args)) {
    return false;
  }

  return true;
}

static bool validate_sampling_rate(const radio_uhd_device::properties& properties, double sampling_rate)
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

static bool validate_otw_format(const radio_uhd_device::properties&       properties,
                                radio_configuration::over_the_wire_format otw_format)
{
  if (properties.supported_otw_formats.count(otw_format) == 0) {
    fmt::print("The selected over the wire format is not supported.\n");
    return false;
  }

  return true;
}

static bool validate_log_level(const radio_uhd_device::properties& properties, const std::string& log_level)
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
  if (!validate_clock_sources(properties, config.clock)) {
    return false;
  }

  if (config.tx_streams.empty()) {
    fmt::print("At least one transmit stream must be available.\n");
    return false;
  }

  for (const radio_configuration::stream& tx_stream : config.tx_streams) {
    if (!validate_stream(properties, tx_stream, true)) {
      return false;
    }
  }

  if (config.rx_streams.empty()) {
    fmt::print("At least one receive stream must be available.\n");
    return false;
  }

  for (const radio_configuration::stream& rx_stream : config.rx_streams) {
    if (!validate_stream(properties, rx_stream, false)) {
      return false;
    }
  }

  if (!validate_sampling_rate(properties, config.sampling_rate_hz)) {
    return false;
  }

  if (!validate_otw_format(properties, config.otw_format)) {
    return false;
  }

  if (!validate_log_level(properties, config.log_level)) {
    return false;
  }

  return true;
}
