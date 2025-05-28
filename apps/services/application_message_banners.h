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

#include "external/fmt/include/fmt/core.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/versioning/build_info.h"

namespace srsran {
namespace app_services {

/// \brief Application message banners service.
///
/// Announces in STDOUT when the application starts and stops.
class application_message_banners
{
public:
  /// Announces the application started.
  explicit application_message_banners(std::string_view app_name, std::string_view log_filename_) :
    log_filename(log_filename_)
  {
    fmt::print("==== {} started ===\n", app_name);
    fmt::print("Type <h> to view help\n");
  }

  /// Announces the application is stopping.
  ~application_message_banners()
  {
    fmt::print("Stopping...\n");
    if (!log_filename.empty()) {
      fmt::print("Logfile stored in {}\n", log_filename);
    }
  }

  /// Announces the application name and version using its build hash.
  static void announce_app_and_version(std::string_view app_name)
  {
    fmt::print("\n--== srsRAN {} (commit {}) ==--\n\n", app_name, get_build_hash());
  }

  /// Logs in the given logger application build parameters.
  static void log_build_info(srslog::basic_logger& logger)
  {
    // Log build info
    logger.info("Built in {} mode using {}", get_build_mode(), get_build_info());
  }

private:
  std::string log_filename;
};

} // namespace app_services
} // namespace srsran
