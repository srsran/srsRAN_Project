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
