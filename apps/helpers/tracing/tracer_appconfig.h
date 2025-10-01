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

#include <string>

namespace srsran {

/// Configuration of the tracing service.
struct tracer_appconfig {
  /// Set to a valid file path to enable tracing and write the trace to the file.
  std::string filename;
  /// Maximum number of events per file. Set to zero for no limit.
  unsigned max_tracing_events_per_file = 1e6;
  /// Number of events to write prior to a severe event. Set to zero for writing all events.
  unsigned nof_tracing_events_after_severe = 0;
};

} // namespace srsran
