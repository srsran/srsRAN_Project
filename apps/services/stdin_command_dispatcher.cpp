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

#include "stdin_command_dispatcher.h"
#include "stdin_command_dispatcher_utils.h"
#include "srsran/srslog/srslog.h"
#include <csignal>
#include <fcntl.h>
#include <unistd.h>

using namespace srsran;
using namespace app_services;

namespace {

/// Command that manages closing the application.
class close_app_command : public application_command
{
public:
  // See interface for documentation.
  std::string_view get_name() const override { return "q"; }

  // See interface for documentation.
  std::string_view get_description() const override { return ":                                    quit application"; }

  // See interface for documentation.
  void execute(span<const std::string> args) override { ::raise(SIGTERM); }
};

/// Command that sleeps the application.
class sleep_app_command : public application_command
{
public:
  // See interface for documentation.
  std::string_view get_name() const override { return "sleep"; }

  // See interface for documentation.
  std::string_view get_description() const override
  {
    return " <seconds>:                      stops the execution of console sequential commands for the specified time";
  }

  // See interface for documentation.
  void execute(span<const std::string> args) override
  { // Verify that the number of arguments is valid.
    if (args.size() != 1) {
      fmt::print("Invalid sleep command syntax. Usage: sleep <seconds>\n");
      return;
    }

    // Parse seconds.
    expected<int64_t, std::string> seconds = app_services::parse_int<int64_t>(args.front());

    // Verify the argument is numeric.
    if (not seconds.has_value()) {
      fmt::print("{}.\n", seconds.error());
      return;
    }

    // Verify it is not negative.
    if (seconds.value() < 0) {
      fmt::print("The number of seconds to sleep cannot be negative.\n");
      return;
    }

    std::this_thread::sleep_for(std::chrono::seconds(seconds.value()));
  }
};

} // namespace

stdin_command_dispatcher::stdin_command_dispatcher(io_broker&                                 io_broker,
                                                   span<std::unique_ptr<application_command>> commands_) :
  logger(srslog::fetch_basic_logger("APP"))
{
  // Add the sleep command.
  {
    auto  sleep_cmd = std::make_unique<sleep_app_command>();
    auto& cmd       = commands[std::string(sleep_cmd->get_name())];
    cmd             = std::move(sleep_cmd);
  }
  // Add the close command.
  {
    auto  close_cmd = std::make_unique<close_app_command>();
    auto& cmd       = commands[std::string(close_cmd->get_name())];
    cmd             = std::move(close_cmd);
  }

  // Store the commands.
  for (auto& app_cmd : commands_) {
    auto& cmd = commands[std::string(app_cmd->get_name())];
    cmd       = std::move(app_cmd);
  }

  // Set STDIN file descripter into non-blocking mode
  int flags = ::fcntl(STDIN_FILENO, F_GETFL, 0);
  if (::fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK) == -1) {
    logger.error("Couldn't configure fd to non-blocking");
  }

  stdin_handle = io_broker.register_fd(STDIN_FILENO, [this]() { parse_stdin(STDIN_FILENO); });
  if (!stdin_handle.registered()) {
    logger.error("Couldn't register stdin handler");
  }
}

/// Splits a given string into multiple elements given a delimiter.
static void string_parse_list(const std::string& input, char delimiter, std::vector<std::string>& commands)
{
  std::stringstream ss(input);

  // Removes all possible elements of the list
  commands.clear();

  while (ss.good()) {
    std::string substr;
    std::getline(ss, substr, delimiter);

    if (!substr.empty()) {
      commands.push_back(substr);
    }
  }
}

void stdin_command_dispatcher::parse_stdin(int file_descriptor)
{
  static constexpr unsigned           read_chunk = 256;
  std::array<uint8_t, 4 * read_chunk> buffer;
  int                                 bytes_read       = 0;
  int                                 total_bytes_read = 0;

  logger.debug("Stdin has data to read");

  do {
    // Read from stdin until EWOULDBLOCK is set
    bytes_read = ::read(file_descriptor, &buffer[total_bytes_read], read_chunk);
    if (bytes_read < 0) {
      if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR) {
        break;
      }
    } else if (bytes_read == 0) {
      logger.debug("Connection closed");
      return;
    }

    total_bytes_read += bytes_read;
    if (total_bytes_read + read_chunk > buffer.size()) {
      logger.error("Can't read more than {} B from stdin", buffer.size());
      return;
    }
  } while (true);

  logger.debug("read {} B from stdin", total_bytes_read);

  // Convert buffer to string
  std::string input_line(buffer.begin(), buffer.begin() + total_bytes_read);

  std::vector<std::string> cmd_list;
  string_parse_list(input_line, ';', cmd_list);
  for (auto& cmd : cmd_list) {
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '\n'), cmd.cend());
    handle_command(cmd);
  }
}

void stdin_command_dispatcher::handle_command(const std::string& command)
{
  // Print help message if the command is empty.
  if (command.empty()) {
    print_help();
    return;
  }

  // Break the command into a list of arguments.
  std::vector<std::string> arg_list;
  string_parse_list(command, ' ', arg_list);

  srsran_assert(!arg_list.empty(), "Parsing empty command argument list");

  if (const auto& cmd = commands.find(arg_list.front()); cmd != commands.end()) {
    cmd->second->execute(span<const std::string>(arg_list).last(arg_list.size() - 1));
  } else {
    print_help();
  }
}

void stdin_command_dispatcher::print_help()
{
  fmt::print("Available commands:\n");
  for (const auto& command : commands) {
    fmt::print("\t{}{}\n", command.second->get_name(), command.second->get_description());
  }
  fmt::print("\n");
}
