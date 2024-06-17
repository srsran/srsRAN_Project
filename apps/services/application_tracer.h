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

#include "srsran/support/event_tracing.h"
#include "srsran/support/srsran_assert.h"
#include <string_view>

namespace srsran {
namespace app_services {

/// \brief Application tracer service.
///
/// Manages opening/closing a trace file.
class application_tracer
{
  bool                  enabled = false;
  srslog::basic_logger* logger  = nullptr;

public:
  /// Enables the tracer using the given filename.
  void enable_tracer(std::string_view file_name, srslog::basic_logger& logger_)
  {
    srsran_assert(!enabled, "Tracer service already enabled");
    logger  = &logger_;
    enabled = true;
    logger_.info("Opening event tracer in file '{}' ...", file_name);
    open_trace_file(file_name);
    logger_.info("Event tracer opened successfully");
  }

  ~application_tracer()
  {
    if (enabled) {
      srsran_assert(logger, "Invalid logger");
      logger->info("Closing event tracer...");
      close_trace_file();
      logger->info("Event tracer closed successfully");
    }
  }
};

} // namespace app_services
} // namespace srsran
