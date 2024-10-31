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
  rusage_trace_recorder(T&& tracer_, trace_duration latency_thres_, unsigned max_sections) :
    tracer(std::forward<T>(tracer_)), thres(latency_thres_)
  {
    // Add two extra positions for start and stop.
    trace_points.reserve(max_sections + 2);
  }

  void set(trace_duration latency_thres_, unsigned max_sections)
  {
    thres = latency_thres_;
    trace_points.reserve(max_sections + 2);
  }

  void start(const char* record_name)
  {
    srsran_assert(trace_points.empty(), "Recorder has already started");
    trace_points.emplace_back(rusage_trace_event{record_name, this->tracer.now(), this->tracer.rusage_now()});
  }

  void add_section(const char* name)
  {
    if (trace_points.size() + 1 == trace_points.capacity()) {
      // No more space (one entry needs to be left for the stop).
      return;
    }
    srsran_assert(not trace_points.empty(), "Recorder did not start");
    trace_points.emplace_back(rusage_trace_event{name, this->tracer.now(), this->tracer.rusage_now()});
  }

  void stop(const char* stop_section_name)
  {
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

  void set(trace_duration latency_thres_, unsigned max_sections) {}
  void start(const char* /* unused */) {}
  void add_section(const char* /* unused */) {}
  void stop(const char* /* unused */) {}
};

template <typename TracerType>
rusage_trace_recorder<TracerType>
create_rusage_trace_recorder(TracerType&& tracer, trace_duration latency_thres, unsigned max_sections)
{
  return rusage_trace_recorder<TracerType>{std::forward<TracerType>(tracer), latency_thres, max_sections};
}

} // namespace srsran
