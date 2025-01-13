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

#include "srsran/support/tracing/event_tracing.h"
#include <vector>

namespace srsran {

/// \brief Helper class to register all the sections within a recording period.
///
/// The events are only logged if the time difference between stop_recording and start_recording is higher than
/// \c latency_thres.
/// \remark This class is not thread safe.
template <typename TracerType, bool Enabled = not std::is_base_of_v<detail::null_event_tracer, TracerType>>
class rusage_trace_recorder
{
public:
  template <typename T = TracerType>
  rusage_trace_recorder(const std::string& name_, T&& tracer_, trace_duration latency_thres_, unsigned max_sections) :
    name(name_), tracer(std::forward<T>(tracer_)), thres(latency_thres_)
  {
    if (tracer.is_enabled()) {
      // Add two extra positions for start and stop.
      trace_points.reserve(max_sections + 2);
    }
  }

  void start()
  {
    if (tracer.is_enabled()) {
      srsran_assert(trace_points.empty(), "Recorder has already started");
      trace_points.emplace_back(rusage_trace_event{name.c_str(), this->tracer.now(), this->tracer.rusage_now()});
    }
  }

  void add_section(const char* section_name)
  {
    if (not tracer.is_enabled() or trace_points.size() + 1 >= trace_points.capacity()) {
      // No more space (one entry needs to be left for the stop).
      return;
    }
    srsran_assert(not trace_points.empty(), "Recorder did not start");
    trace_points.emplace_back(rusage_trace_event{section_name, this->tracer.now(), this->tracer.rusage_now()});
  }

  void stop(const char* stop_section_name)
  {
    if (not tracer.is_enabled()) {
      return;
    }
    srsran_assert(not trace_points.empty(), "Recorder did not start");
    auto end_tp = this->tracer.now();
    auto dur    = std::chrono::duration_cast<trace_duration>(end_tp - this->trace_points.front().start_tp);
    if (dur >= this->thres) {
      // We exceeded the max latency.
      trace_points.emplace_back(rusage_trace_event{stop_section_name, end_tp, this->tracer.rusage_now()});
      tracer << trace_points;
    }
    trace_points.clear();
  }

private:
  std::string    name;
  TracerType     tracer;
  trace_duration thres;

  std::vector<rusage_trace_event> trace_points;
};

// Specialization for when the tracer is disabled.
template <typename TracerType>
class rusage_trace_recorder<TracerType, false>
{
public:
  template <typename... TracerArgs>
  rusage_trace_recorder(TracerArgs&&... /* unused */)
  {
  }

  void start() {}
  void add_section(const char* /* unused */) {}
  void stop(const char* /* unused */) {}
};

template <typename TracerType>
rusage_trace_recorder<TracerType> create_rusage_trace_recorder(const std::string& name,
                                                               TracerType&&       tracer,
                                                               trace_duration     latency_thres,
                                                               unsigned           max_sections)
{
  return rusage_trace_recorder<TracerType>{name, std::forward<TracerType>(tracer), latency_thres, max_sections};
}

} // namespace srsran
