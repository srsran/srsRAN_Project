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

#include "srsran/adt/span.h"
#include <string_view>

namespace srsran {
namespace app_services {

/// \brief Application command interface.
///
/// This interface allows to define commands that change the behavior of the application.
class application_command
{
public:
  /// Default destructor.
  virtual ~application_command() = default;

  /// Returns the name of this command.
  virtual std::string_view get_name() const = 0;

  /// Returns the description of this command.
  virtual std::string_view get_description() const = 0;

  /// Execute this command with the given arguments.
  virtual void execute(span<const std::string> args) = 0;
};

} // namespace app_services
} // namespace srsran
