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

#include "gnb_console_helper.h"
#include "string_helpers.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/support/build_info/build_info.h"
#include "srsran/support/io/io_broker.h"
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <vector>

using namespace srsran;

// List of possible log channels that can be dynamically changed.
static const std::vector<std::string> dynamic_log_channels = {"PHY"};

// Parses integer values from a console command.
template <typename Integer>
static expected<Integer, std::string> parse_int(const std::string& value)
{
  try {
    return std::stoi(value);
  } catch (const std::invalid_argument& e) {
    return {e.what()};
  } catch (const std::out_of_range& e) {
    return {e.what()};
  }
}

// Parses floating point values from a console command and attempts to store them in a double.
static expected<double, std::string> parse_double(const std::string& value)
{
  try {
    return std::stod(value);
  } catch (const std::invalid_argument& e) {
    return {e.what()};
  } catch (const std::out_of_range& e) {
    return {e.what()};
  }
}

gnb_console_helper::gnb_console_helper(io_broker&           io_broker_,
                                       srslog::log_channel& log_chan_,
                                       bool                 autostart_stdout_metrics_) :
  logger(srslog::fetch_basic_logger("GNB")),
  io_broker_handle(io_broker_),
  metrics_json(log_chan_),
  autostart_stdout_metrics(autostart_stdout_metrics_)
{
  // set STDIN file descripter into non-blocking mode
  int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  if (fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK) == -1) {
    logger.error("Couldn't configure fd to non-blocking");
  }

  if (io_broker_handle.register_fd(STDIN_FILENO, [this](int fd) { stdin_handler(fd); }) == false) {
    logger.error("Couldn't register stdin handler");
  }
}

gnb_console_helper::~gnb_console_helper()
{
  bool success = io_broker_handle.unregister_fd(STDIN_FILENO);
  if (!success) {
    report_fatal_error("Failed to unregister stdin file descriptor at IO broker. fd={}", STDIN_FILENO);
  }
}

void gnb_console_helper::stdin_handler(int fd)
{
  static const unsigned               read_chunk = 256;
  std::array<uint8_t, 4 * read_chunk> buffer;
  int                                 bytes_read       = 0;
  int                                 total_bytes_read = 0;

  logger.debug("Stdin has data to read");

  do {
    // read from stdin until EWOULDBLOCK is set
    bytes_read = read(fd, &buffer[total_bytes_read], read_chunk);
    if (bytes_read < 0) {
      if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR) {
        break;
      }
    } else if (bytes_read == 0) {
      logger.debug("Connection closed");
      return;
    } else {
      total_bytes_read += bytes_read;
      if (total_bytes_read + read_chunk > buffer.size()) {
        logger.error("Can't read more than {} B from stdin", buffer.size());
        return;
      }
    }
  } while (true);

  logger.debug("read {} B from stdin", total_bytes_read);

  // convert buffer to string
  std::string input_line(buffer.begin(), buffer.begin() + total_bytes_read);

  std::list<std::string> cmd_list;
  srsran::string_parse_list(input_line, ';', cmd_list);
  for (auto& cmd : cmd_list) {
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '\n'), cmd.cend());
    handle_command(cmd);
  }
}

void gnb_console_helper::handle_tx_gain_command(const std::list<std::string>& gain_args)
{
  if (!radio_controller.has_value()) {
    fmt::print("Interactive radio control is not supported for the current GNB configuration.\n");
    return;
  }

  if (gain_args.size() != 2) {
    fmt::print("Invalid TX gain command structure. Usage: tx_gain <port_id> <gain_dB>\n");
    return;
  }

  expected<unsigned, std::string> port_id = parse_int<unsigned>(gain_args.front());
  if (port_id.is_error()) {
    fmt::print("Invalid port ID.\n");
    return;
  }
  expected<double, std::string> gain_dB = parse_double(gain_args.back());
  if (gain_dB.is_error()) {
    fmt::print("Invalid gain value.\n");
    return;
  }

  if (!radio_controller.value()->set_tx_gain(port_id.value(), gain_dB.value())) {
    fmt::print("Setting TX gain was not successful. The radio may not support this feature.\n");
    return;
  }

  fmt::print("Tx gain set to {} dB for port {}.\n", gain_dB.value(), port_id.value());
}

void gnb_console_helper::handle_rx_gain_command(const std::list<std::string>& gain_args)
{
  if (!radio_controller.has_value()) {
    fmt::print("Interactive radio control is not supported for the current GNB configuration.\n");
    return;
  }

  if (gain_args.size() != 2) {
    fmt::print("Invalid RX gain command structure. Usage: rx_gain <port_id> <gain_dB>\n");
    return;
  }

  expected<unsigned, std::string> port_id = parse_int<unsigned>(gain_args.front());
  if (port_id.is_error()) {
    fmt::print("Invalid port ID.\n");
    return;
  }
  expected<double, std::string> gain_dB = parse_double(gain_args.back());
  if (gain_dB.is_error()) {
    fmt::print("Invalid gain value.\n");
    return;
  }

  if (!radio_controller.value()->set_rx_gain(port_id.value(), gain_dB.value())) {
    fmt::print("Setting RX gain was not successful. The radio may not support this feature.\n");
    return;
  }

  fmt::print("Rx gain set to {} dB for port {}.\n", gain_dB.value(), port_id.value());
}

void gnb_console_helper::handle_log_command(const std::list<std::string>& args)
{
  // Verify that the number of arguments is valid.
  if (args.size() != 2) {
    fmt::print("Invalid log command syntax. Usage: log <channel> <level>\n");
    return;
  }

  // Copy and change to upper case the chanel string.
  std::string channel_str = args.front();
  std::transform(channel_str.begin(), channel_str.end(), channel_str.begin(), ::toupper);

  // Copy and change to upper case the level string.
  std::string level_str = args.back();
  std::transform(level_str.begin(), level_str.end(), level_str.begin(), ::toupper);

  // Check if the log channel exists.
  if (std::find(dynamic_log_channels.begin(), dynamic_log_channels.end(), channel_str) == dynamic_log_channels.end()) {
    fmt::print(
        "Invalid {} log channel. Valid channels are: {}\n", channel_str, span<const std::string>(dynamic_log_channels));
    return;
  }

  // Check if the log channel exists.
  if (srslog::find_logger<srslog::basic_logger>(channel_str) == nullptr) {
    fmt::print("Nonexistent {} log channel.\n", channel_str);
    return;
  }

  // Convert to enum and check if it is valid.
  srslog::basic_levels level = srslog::str_to_basic_level(level_str);
  if (level_str != basic_level_to_string(level)) {
    fmt::print("Invalid {} log level. Valid levels are: none, error, warning, info and debug\n", args.back());
    return;
  }

  srslog::basic_logger& channel = srslog::fetch_basic_logger(channel_str);
  channel.set_level(level);
}

void gnb_console_helper::handle_sleep_command(const std::list<std::string>& args)
{
  // Verify that the number of arguments is valid.
  if (args.size() != 1) {
    fmt::print("Invalid sleep command syntax. Usage: sleep <seconds>\n");
    return;
  }

  // Parse seconds.
  char*   val     = nullptr;
  int64_t seconds = std::strtol(args.front().c_str(), &val, 10);

  // Verify the argument is numeric.
  if (val != args.front().c_str() + args.front().size()) {
    fmt::print("The provided seconds is not a number.\n");
    return;
  }

  // Verify it is not negative.
  if (seconds < 0) {
    fmt::print("The number of seconds to sleep cannot be negative.\n");
    return;
  }

  std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void gnb_console_helper::handle_command(const std::string& command)
{
  // Print help message if the command is empty.
  if (command.empty()) {
    print_help();
    return;
  }

  // Break the command into a list of arguments.
  std::list<std::string> arg_list;
  srsran::string_parse_list(command, ' ', arg_list);

  srsran_assert(!arg_list.empty(), "Parsing empty command argument list");

  if (command == "q") {
    raise(SIGTERM);
  } else if (command == "t") {
    metrics_plotter.toggle_print();
  } else if (arg_list.front() == "tx_gain") {
    arg_list.pop_front();
    handle_tx_gain_command(arg_list);
  } else if (arg_list.front() == "rx_gain") {
    arg_list.pop_front();
    handle_rx_gain_command(arg_list);
  } else if (arg_list.front() == "log") {
    arg_list.pop_front();
    handle_log_command(arg_list);
  } else if (arg_list.front() == "sleep") {
    arg_list.pop_front();
    handle_sleep_command(arg_list);
  } else {
    print_help();
  }
}

void gnb_console_helper::print_help()
{
  fmt::print("Available commands:\n");
  fmt::print("\tt:                     start/stop console trace\n");
  fmt::print("\tq:                     quit application\n");
  fmt::print("\ttx_gain <port> <gain>: set Tx gain\n");
  fmt::print("\trx_gain <port> <gain>: set Rx gain\n");
  fmt::print("\tlog <channel> <level>: set log level for the given channel (experimental)\n");
  fmt::print("\t                       Allowed channels are: {}\n", span<const std::string>(dynamic_log_channels));
  fmt::print("\t                       Note: Certain log messages might not be available if they depend on\n");
  fmt::print("\t                       logging decorators that are not instanced with the initial log level.\n");
  fmt::print("\tsleep <seconds>:       stops the execution of console sequential commands for the specified time\n");
  fmt::print("\n");
}

void gnb_console_helper::set_cells(const span<du_cell_config>& cells_)
{
  cells = {cells_.begin(), cells_.end()};
}

void gnb_console_helper::set_ru_controller(ru_controller& controller)
{
  radio_controller.emplace(&controller);
}

void gnb_console_helper::on_app_starting()
{
  fmt::print("\n--== srsRAN gNB (commit {}) ==--\n\n", get_build_hash());
}

void gnb_console_helper::on_app_running()
{
  for (const auto& cell : cells) {
    fmt::print("Cell pci={}, bw={} MHz, dl_arfcn={} (n{}), dl_freq={} MHz, dl_ssb_arfcn={}, ul_freq={} MHz\n",
               cell.pci,
               cell.dl_carrier.carrier_bw_mhz,
               cell.dl_carrier.arfcn,
               srsran::nr_band_to_uint(cell.dl_carrier.band),
               srsran::band_helper::nr_arfcn_to_freq(cell.dl_carrier.arfcn) / 1e6,
               cell.dl_cfg_common.freq_info_dl.absolute_frequency_ssb,
               srsran::band_helper::nr_arfcn_to_freq(cell.ul_carrier.arfcn) / 1e6);
  }
  fmt::print("\n");

  fmt::print("==== gNodeB started ===\n");
  fmt::print("Type <t> to view trace\n");

  if (autostart_stdout_metrics) {
    metrics_plotter.enable_print();
  }
}

void gnb_console_helper::on_app_stopping()
{
  fmt::print("Stopping ..\n");
}
