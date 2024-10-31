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

#include "srsran/adt/static_vector.h"
#include "srsran/support/tracing/event_tracing.h"

namespace srsran {

template <typename TracerType, size_t MAX_EVENTS>
class l2_rusage_tracer
{
public:
  template <typename... TracerArgs>
  l2_rusage_tracer(trace_duration latency_thres_, TracerArgs&&... args) :
    thres(latency_thres_), tracer(std::forward<TracerArgs>(args)...)
  {
  }

  void start_slot()
  {
    start_tp   = tracer.now();
    start_rusg = tracer.rusage_now();
  }

  void add_time_point(const char* name)
  {
    trace_points.emplace_back(rusage_thres_trace_event{name, tracer.now(), tracer.rusage_now()});
  }

  void end_slot()
  {
    auto end_tp = tracer.now();
    auto dur    = std::chrono::duration_cast<trace_duration>(end_tp - start_tp);
    if (dur >= thres) {
      // We exceeded the max latency.
      log_events();
      auto end_rusg = tracer.rusage_now();
    }
    trace_points.clear();
  }

private:
  struct trace_point {
    const char*              name;
    trace_point              tp;
    resource_usage::snapshot rusg;
  };

  void log_events()
  {
    auto prev_tp = start_tp;
    for (unsigned i = 0; i != trace_points.size(); prev_tp = trace_points[i].tp, ++i) {
      tracer << rusage_thres_trace_event{trace_points[i].name, trace_duration{0}, prev_tp, trace_points[i].rusg};
    }
  }

  trace_duration thres;
  TracerType     tracer;

  static_vector<rusage_thres_trace_event, MAX_EVENTS> trace_points;

  trace_point              start_tp;
  resource_usage::snapshot start_rusg;
};

} // namespace srsran