/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "cmdline_command.h"
#include "srsran/adt/span.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/io/io_broker.h"
#include <string>
#include <unordered_map>

namespace srsran {
namespace app_services {

/// \brief Command-line command dispatcher.
///
/// The command-line command dispatcher parses commands from the STDIN and executes them if they were registered upon
/// creation, otherwise prints a list of the registered commands with their description.
class cmdline_command_dispatcher
{
public:
  cmdline_command_dispatcher(io_broker&                             io_broker,
                             task_executor&                         executor,
                             span<std::unique_ptr<cmdline_command>> commands_);

private:
  /// Parses any contents in the STDIN file descriptor.
  void parse_stdin();

  /// Handles the given command.
  void handle_command(const std::string& command);

  /// Prints the registered events registered in this console.
  void print_help();

private:
  srslog::basic_logger&                                             logger;
  io_broker::subscriber                                             stdin_handle;
  std::unordered_map<std::string, std::unique_ptr<cmdline_command>> commands;
};

} // namespace app_services
} // namespace srsran
