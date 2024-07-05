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

#include "external/fmt/include/fmt/core.h"
#include "srsran/support/build_info/build_info.h"

namespace srsran {
namespace app_services {

/// \brief Application message banners service.
///
/// Announces in STDIN when the application starts and stops.
class application_message_banners
{
public:
  /// Announces the application started.
  explicit application_message_banners(std::string_view app_name)
  {
    fmt::print("==== {} started ===\n", app_name);
    fmt::print("Type <h> to view help\n");
  }

  /// Announces the application is stopping.
  ~application_message_banners() { fmt::print("Stopping ..\n"); }

  /// Announces the application name and version using its build hash.
  static void announce_app_and_version(std::string_view app_name)
  {
    fmt::print("\n--== srsRAN {} (commit {}) ==--\n\n", app_name, get_build_hash());
  }

  /// Logs in the given logger application build parameters.
  static void log_build_info(srslog::basic_logger& logger)
  {
    // Log build info
    logger.info("Built in {} mode using {})", get_build_mode(), get_build_info());
  }
};

} // namespace app_services
} // namespace srsran
