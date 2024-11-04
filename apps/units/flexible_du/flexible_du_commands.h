/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "apps/services/application_command.h"
#include "apps/services/stdin_command_dispatcher_utils.h"
#include "srsran/adt/expected.h"
#include "srsran/adt/to_array.h"
#include "srsran/ru/ru_controller.h"

namespace srsran {

/// Application command to change the transmission gain.
class tx_gain_app_command : public app_services::application_command
{
  ru_controller& controller;

public:
  explicit tx_gain_app_command(ru_controller& controller_) : controller(controller_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "tx_gain"; }

  // See interface for documentation.
  std::string_view get_description() const override { return " <port> <gain>:                set Tx gain"; }

  // See interface for documentation.
  void execute(span<const std::string> args) override
  {
    if (args.size() != 2) {
      fmt::print("Invalid TX gain command structure. Usage: tx_gain <port_id> <gain_dB>\n");
      return;
    }

    expected<unsigned, std::string> port_id = app_services::parse_int<unsigned>(args.front());
    if (not port_id.has_value()) {
      fmt::print("Invalid port ID.\n");
      return;
    }
    expected<double, std::string> gain_dB = app_services::parse_double(args.back());
    if (not gain_dB.has_value()) {
      fmt::print("Invalid gain value.\n");
      return;
    }

    if (!controller.set_tx_gain(port_id.value(), gain_dB.value())) {
      fmt::print("Setting TX gain was not successful. The radio may not support this feature.\n");
      return;
    }

    fmt::print("Tx gain set to {} dB for port {}.\n", gain_dB.value(), port_id.value());
  }
};

/// Application command to change the reception gain.
class rx_gain_app_command : public app_services::application_command
{
  ru_controller& controller;

public:
  explicit rx_gain_app_command(ru_controller& controller_) : controller(controller_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "rx_gain"; }

  // See interface for documentation.
  std::string_view get_description() const override { return " <port> <gain>:                set Rx gain"; }

  // See interface for documentation.
  void execute(span<const std::string> args) override
  {
    if (args.size() != 2) {
      fmt::print("Invalid RX gain command structure. Usage: rx_gain <port_id> <gain_dB>\n");
      return;
    }

    expected<unsigned, std::string> port_id = app_services::parse_int<unsigned>(args.front());
    if (not port_id.has_value()) {
      fmt::print("Invalid port ID.\n");
      return;
    }
    expected<double, std::string> gain_dB = app_services::parse_double(args.back());
    if (not gain_dB.has_value()) {
      fmt::print("Invalid gain value.\n");
      return;
    }

    if (!controller.set_rx_gain(port_id.value(), gain_dB.value())) {
      fmt::print("Setting RX gain was not successful. The radio may not support this feature.\n");
      return;
    }

    fmt::print("Rx gain set to {} dB for port {}.\n", gain_dB.value(), port_id.value());
  }
};

/// Application command to change display the Radio Unit metrics.
class ru_metrics_app_command : public app_services::application_command
{
  ru_controller& controller;

public:
  explicit ru_metrics_app_command(ru_controller& controller_) : controller(controller_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "ru_metrics"; }

  // See interface for documentation.
  std::string_view get_description() const override { return ":                           prints RU metrics once"; }

  // See interface for documentation.
  void execute(span<const std::string> args) override { controller.print_metrics(); }
};

/// Application command to change the DU log level.
class change_log_level_app_command : public app_services::application_command
{
  /// List of possible log channels that can be dynamically changed.
  static constexpr auto dynamic_log_channels = to_array<std::string_view>({"PHY"});
  const std::string     description;

public:
  change_log_level_app_command() :
    description(fmt::format(
        " <channel> <level>:                set log level for the given channel (experimental)\n"
        "\t                                      Allowed channels are: {}\n"
        "\t                                      Note: Certain log messages might not be available if they depend on\n"
        "\t                                      logging decorators that are not instanced with the initial log level",
        span<const std::string_view>(dynamic_log_channels)))
  {
  }

  // See interface for documentation.
  std::string_view get_name() const override { return "log"; }

  // See interface for documentation.
  std::string_view get_description() const override { return description; }

  // See interface for documentation.
  void execute(span<const std::string> args) override
  { // Verify that the number of arguments is valid.
    if (args.size() != 2) {
      fmt::print("Invalid log command syntax. Usage: log <channel> <level>\n");
      return;
    }

    // Copy and change to upper case the channel string.
    std::string channel_str(args.front());
    std::transform(channel_str.begin(), channel_str.end(), channel_str.begin(), ::toupper);

    // Copy and change to upper case the level string.
    std::string level_str(args.back());
    std::transform(level_str.begin(), level_str.end(), level_str.begin(), ::toupper);

    // Check if the log channel exists.
    if (std::find(dynamic_log_channels.begin(), dynamic_log_channels.end(), channel_str) ==
        dynamic_log_channels.end()) {
      fmt::print("Invalid {} log channel. Valid channels are: {}\n",
                 channel_str,
                 span<const std::string_view>(dynamic_log_channels));
      return;
    }

    // Check if the log channel exists.
    if (srslog::find_logger<srslog::basic_logger>(channel_str) == nullptr) {
      fmt::print("Nonexistent {} log channel.\n", channel_str);
      return;
    }

    // Convert to enum and check if it is valid.
    auto level = srslog::str_to_basic_level(level_str);
    if (!level.has_value()) {
      fmt::print("Invalid {} log level. Valid levels are: none, error, warning, info and debug\n", args.back());
      return;
    }

    srslog::basic_logger& channel = srslog::fetch_basic_logger(channel_str);
    channel.set_level(level.value());
  }
};

} // namespace srsran
