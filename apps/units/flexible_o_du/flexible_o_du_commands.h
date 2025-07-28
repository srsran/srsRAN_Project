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

#pragma once

#include "apps/services/cmdline/cmdline_command_dispatcher_utils.h"
#include "apps/services/cmdline/stdout_metrics_command.h"
#include "split_helpers/metrics/flexible_o_du_metrics_consumers.h"
#include "srsran/adt/expected.h"
#include "srsran/adt/to_array.h"
#include "srsran/ru/ru_controller.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

/// Application command to change the transmission gain.
class tx_gain_app_command : public app_services::cmdline_command
{
  ru_gain_controller& controller;

public:
  explicit tx_gain_app_command(ru_gain_controller& controller_) : controller(controller_) {}

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
class rx_gain_app_command : public app_services::cmdline_command
{
  ru_gain_controller& controller;

public:
  explicit rx_gain_app_command(ru_gain_controller& controller_) : controller(controller_) {}

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

/// Application command to change the DU log level.
class change_log_level_app_command : public app_services::cmdline_command
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

/// Application command to set the carrier frequency offset.
class cfo_app_command : public app_services::cmdline_command
{
  ru_cfo_controller& controller;

public:
  explicit cfo_app_command(ru_cfo_controller& controller_) : controller(controller_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "cfo"; }

  // See interface for documentation.
  std::string_view get_description() const override { return " <sector_id> <cfo>:                set CFO"; }

  // See interface for documentation.
  void execute(span<const std::string> args) override
  {
    if (args.size() != 2) {
      fmt::print("Invalid CFO command structure. Usage: cfo <sector_id> <cfo_Hz>\n");
      return;
    }

    expected<unsigned, std::string> sector_id = app_services::parse_int<unsigned>(args.front());
    if (not sector_id.has_value()) {
      fmt::print("Invalid sector identifier.\n");
      return;
    }
    expected<double, std::string> cfo = app_services::parse_double(args.back());
    if (not cfo.has_value()) {
      fmt::print("Invalid CFO value.\n");
      return;
    }

    cfo_compensation_request cfo_reqs;
    cfo_reqs.cfo_hz = cfo.value();
    if (!controller.set_tx_cfo(sector_id.value(), cfo_reqs)) {
      fmt::print("Setting TX CFO was not successful. The radio may not support this feature.\n");
      return;
    }

    if (!controller.set_rx_cfo(sector_id.value(), cfo_reqs)) {
      fmt::print("Setting RX CFO was not successful. The radio may not support this feature.\n");
      return;
    }

    fmt::print("CFO set to {}Hz for sector {}.\n", cfo.value(), sector_id.value());
  }
};

/// Application command to set the transmit time offset.
class tx_time_offset_app_command : public app_services::cmdline_command
{
  ru_tx_time_offset_controller& controller;

public:
  explicit tx_time_offset_app_command(ru_tx_time_offset_controller& controller_) : controller(controller_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "tx_time_offset"; }

  // See interface for documentation.
  std::string_view get_description() const override
  {
    return " <sector_id> <tx_time_offset>:     set transmit time offset in microseconds";
  }

  // See interface for documentation.
  void execute(span<const std::string> args) override
  {
    if (args.size() != 2) {
      fmt::print("Invalid transmit time offset command structure. Usage: tx_time_offset <sector_id> <Time offset in "
                 "microseconds>\n");
      return;
    }

    expected<unsigned, std::string> sector_id = app_services::parse_int<unsigned>(args.front());
    if (not sector_id.has_value()) {
      fmt::print("Invalid sector identifier.\n");
      return;
    }
    expected<double, std::string> tx_time_offset_us = app_services::parse_double(args.back());
    if (not tx_time_offset_us.has_value()) {
      fmt::print("Invalid transmit time offset format.\n");
      return;
    }

    phy_time_unit tx_time_offset = phy_time_unit::from_seconds(*tx_time_offset_us * 1e-6);
    if (!controller.set_tx_time_offset(sector_id.value(), tx_time_offset)) {
      fmt::print("Setting TX time offset was not successful. The radio may not support this feature.\n");
      return;
    }

    fmt::print(
        "Transmit time offset set to {:.3f} us for sector {}.\n", tx_time_offset.to_seconds() * 1e6, sector_id.value());
  }
};

/// STDOUT metrics subcommand to print RU metrics.
class ru_metrics_subcommand_stdout : public app_services::toggle_stdout_metrics_app_command::metrics_subcommand
{
  ru_metrics_consumer_stdout& printer;

public:
  explicit ru_metrics_subcommand_stdout(ru_metrics_consumer_stdout& printer_) : printer(printer_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "ru"; }

  // See interface for documentation.
  void print_header() override { printer.print_header(); }

  // See interface for documentation.
  void enable() override { printer.enable(); }

  // See interface for documentation.
  void disable() override { printer.disable(); }
};

} // namespace srsran
