/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
