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

#include "srsran/adt/span.h"
#include "srsran/support/compiler.h"
#include "srsran/support/tracing/resource_usage.h"
#include <chrono>
#include <string>
#include <vector>

namespace srslog {

class log_channel;

} // namespace srslog

namespace srsran {

/// \brief Trace Event clock type.
///
/// \remark We use high_resolution_clock instead of steady_clock for time stamps to be aligned with logging.
using trace_clock    = std::chrono::steady_clock;
using trace_point    = trace_clock::time_point;
using trace_duration = std::chrono::microseconds;

/// Trace point value when tracing is disabled.
constexpr trace_point null_trace_point = {};

/// Resource usaged value when tracing is disabled.
constexpr resource_usage::snapshot null_rusage_snapshot{};

/// Open a file to write trace events to.
/// \param[in] trace_file_name Name of the generated file.
/// \param[in] split_after_n   If positive, the events will be written into split files, each with a number of events
///                            lower or equal to \c split_after_n. This way, we avoid generating an enormous single
///                            trace \c trace_file_name is "trace.json", the splitted files will be called
///                            "trace0.json", "trace1.json", etc.
/// \param[in] event_trigger_n If positive, the events will only be written if an instant severe event is detected. In,
///                            this case, only the prior \c event_trigger_n number of events will be written in the
///                            file.
void open_trace_file(std::string_view trace_file_name = "/tmp/srsran_trace.json",
                     unsigned         split_after_n   = 1e6,
                     unsigned         event_trigger_n = 0);

/// Close the trace file. This function is called automatically when the program exits.
void close_trace_file();

/// Check if the trace file is open.
bool is_trace_file_open();

/// \brief Trace event used for events with defined name, starting point and duration.
/// \remark The creation of this type should be trivial so that compiler optimizes it out for null tracers.
struct trace_event {
  const char* name;
  trace_point start_tp;

  SRSRAN_FORCE_INLINE constexpr trace_event(const char* name_, trace_point start_tp_) : name(name_), start_tp(start_tp_)
  {
  }
};

struct trace_thres_event {
  const char*    name;
  trace_point    start_tp;
  trace_duration thres;

  SRSRAN_FORCE_INLINE constexpr trace_thres_event(const char* name_, trace_point start_tp_, trace_duration thres_) :
    name(name_), start_tp(start_tp_), thres(thres_)
  {
  }
};

/// \brief Trace event type with defined name, starting point but no duration.
/// \remark The creation of this type should be trivial so that compiler optimizes it out for null tracers.
struct instant_trace_event {
  enum class cpu_scope : uint8_t { global, process, thread };
  enum class event_criticality : uint8_t { normal = 0, severe };

  const char*       name;
  cpu_scope         scope;
  event_criticality criticality;

  SRSRAN_FORCE_INLINE constexpr instant_trace_event(const char*       name_,
                                                    cpu_scope         scope_,
                                                    event_criticality criticality_ = event_criticality::normal) :
    name(name_), scope(scope_), criticality(criticality_)
  {
  }
};

struct rusage_trace_event {
  const char*              name;
  trace_point              start_tp;
  resource_usage::snapshot rusg_capture;

  SRSRAN_FORCE_INLINE constexpr rusage_trace_event(const char*              name_,
                                                   trace_point              start_tp_,
                                                   resource_usage::snapshot start_rusage_) :
    name(name_), start_tp(start_tp_), rusg_capture(start_rusage_)
  {
  }
};

/// \brief Event like \c trace_thres_event but that also captures resource usage.
struct rusage_thres_trace_event {
  const char*              name;
  trace_duration           thres;
  trace_point              start_tp;
  resource_usage::snapshot rusg_capture;

  SRSRAN_FORCE_INLINE constexpr rusage_thres_trace_event(const char*              name_,
                                                         trace_duration           thres_,
                                                         trace_point              start_tp_,
                                                         resource_usage::snapshot start_rusage_) :
    name(name_), thres(thres_), start_tp(start_tp_), rusg_capture(start_rusage_)
  {
  }
};

namespace detail {

/// \brief Tracer that does not write any events. The compiler should eliminate all calls to this tracer when
/// optimizations are enabled.
class null_event_tracer
{
public:
  static constexpr trace_point    now() { return null_trace_point; }
  static resource_usage::snapshot rusage_now() { return null_rusage_snapshot; }

  void operator<<(const trace_event& event) const {}

  void operator<<(const trace_thres_event& event) const {}

  void operator<<(const instant_trace_event& event) const {}

  void operator<<(const rusage_trace_event& event) const {}

  void operator<<(const rusage_thres_trace_event& event) const {}

  void operator<<(span<const rusage_trace_event> events) const {}

  bool is_enabled() const { return false; }
};

} // namespace detail

/// \brief Class that writes trace events to a dedicated trace file.
template <bool Enabled = true>
class file_event_tracer
{
public:
  static trace_point              now() { return trace_clock::now(); }
  static resource_usage::snapshot rusage_now() { return resource_usage::now().value_or(null_rusage_snapshot); }

  void operator<<(const trace_event& event) const;

  void operator<<(const trace_thres_event& event) const;

  void operator<<(const instant_trace_event& event) const;

  void operator<<(const rusage_trace_event& event) const;

  void operator<<(const rusage_thres_trace_event& event) const;

  void operator<<(span<const rusage_trace_event> events) const;

  bool is_enabled() const;
};

/// Specialization of file_event_tracer that does not write any events.
template <>
class file_event_tracer<false> : public detail::null_event_tracer
{};

/// Class that repurposes a log channel to write trace events.
template <bool Enabled = true>
class logger_event_tracer
{
public:
  explicit logger_event_tracer(srslog::log_channel* log_ch_) :
    log_ch(log_ch_), enabled(log_ch != nullptr ? log_ch->enabled() : false)
  {
  }

  trace_point              now() const { return enabled ? trace_clock::now() : null_trace_point; }
  resource_usage::snapshot rusage_now() const
  {
    return enabled ? resource_usage::now().value_or(null_rusage_snapshot) : null_rusage_snapshot;
  }

  template <typename EventType>
  void operator<<(const EventType& event) const
  {
    if (enabled) {
      push(event);
    }
  }

  bool is_enabled() const { return enabled; }

private:
  void push(const trace_event& ev) const;
  void push(const trace_thres_event& event) const;
  void push(const instant_trace_event& event) const;
  void push(const rusage_trace_event& event) const;
  void push(const rusage_thres_trace_event& event) const;
  void push(span<const rusage_trace_event> events) const;

  srslog::log_channel* log_ch;
  const bool           enabled;
};

template <>
class logger_event_tracer<false> : public detail::null_event_tracer
{};

/// Class that writes trace events to a vector of strings for testing purposes.
class test_event_tracer
{
public:
  static trace_point              now() { return trace_clock::now(); }
  static resource_usage::snapshot rusage_now() { return resource_usage::now().value_or(null_rusage_snapshot); }

  void operator<<(const trace_event& event);

  void operator<<(const trace_thres_event& event);

  void operator<<(const instant_trace_event& event);

  void operator<<(const rusage_trace_event& event);

  void operator<<(const rusage_thres_trace_event& event);

  void operator<<(span<const rusage_trace_event> events);

  bool is_enabled() const { return true; }

  std::vector<std::string> pop_last_events() { return std::move(last_events); }

  void set_log_style_format(bool log_style) { is_log_stype = log_style; }

private:
  std::vector<std::string> last_events;
  bool                     is_log_stype = false;
};

} // namespace srsran
