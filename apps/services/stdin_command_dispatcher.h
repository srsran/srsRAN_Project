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

#include "application_command.h"
#include "srsran/adt/span.h"
#include "srsran/support/io/io_broker.h"
#include <string>
#include <unordered_map>

namespace srsran {
namespace app_services {

/// \brief Standard input command dispatcher.
///
/// The STDIN command dispatcher parses commands from the STDIN and executes them if they were registered upon creation,
/// otherwise prints a list of the registered commands with their description.
class stdin_command_dispatcher
{
public:
  stdin_command_dispatcher(io_broker& io_broker, span<std::unique_ptr<application_command>> commands_);

private:
  /// Parses STDIN with the given file decriptor.
  void parse_stdin(int file_descriptor);

  /// Handles the given command;
  void handle_command(const std::string& command);

  /// Prints the registered events registered in this console.
  void print_help();

private:
  srslog::basic_logger&                                                 logger;
  io_broker::subscriber                                                 stdin_handle;
  std::unordered_map<std::string, std::unique_ptr<application_command>> commands;
};

} // namespace app_services
} // namespace srsran
