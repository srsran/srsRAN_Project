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

#include "srsran/support/srsran_assert.h"
#include "srsran/support/tracing/event_tracing.h"
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
