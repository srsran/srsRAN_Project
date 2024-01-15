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

#include "srsran/support/compiler.h"
#include <chrono>
#include <string>
#include <vector>

namespace srslog {

class log_channel;

} // namespace srslog

namespace srsran {

/// Trace Event clock types.
using trace_clock    = std::chrono::steady_clock;
using trace_point    = trace_clock::time_point;
using trace_duration = std::chrono::microseconds;

/// \brief Open a file to write trace events to.
void open_trace_file(const std::string& trace_file_name = "/tmp/srsran_trace.json");

/// \brief Close the trace file. This function is called automatically when the program exits.
void close_trace_file();

/// \brief Check if the trace file is open.
bool is_trace_file_open();

/// \brief Trace event used for events with defined name, starting point and duration.
/// \remark The creation of this type should be trivial so that compiler optimizes it out for null tracers.
struct trace_event {
  const char* name;
  trace_point start_tp;

  SRSRAN_FORCE_INLINE trace_event(const char* name_, trace_point start_tp_) : name(name_), start_tp(start_tp_) {}
};

struct trace_thres_event {
  const char*    name;
  trace_point    start_tp;
  trace_duration thres;

  SRSRAN_FORCE_INLINE trace_thres_event(const char* name_, trace_point start_tp_, trace_duration thres_) :
    name(name_), start_tp(start_tp_), thres(thres_)
  {
  }
};

/// \brief Trace event type with defined name, starting point but no duration.
/// \remark The creation of this type should be trivial so that compiler optimizes it out for null tracers.
struct instant_trace_event {
  enum class cpu_scope { global, process, thread };

  const char* name;
  cpu_scope   scope;

  SRSRAN_FORCE_INLINE instant_trace_event(const char* name_, cpu_scope scope_) : name(name_), scope(scope_) {}
};

namespace detail {

/// \brief Tracer that does not write any events. The compiler should eliminate all calls to this tracer when
/// optimizations are enabled.
class null_event_tracer
{
public:
  static trace_point now() { return {}; }

  void operator<<(const trace_event& event) const {}

  void operator<<(const trace_thres_event& event) const {}

  void operator<<(const instant_trace_event& event) const {}
};

} // namespace detail

/// \brief Class that writes trace events to a dedicated trace file.
template <bool Enabled = true>
class file_event_tracer
{
public:
  static trace_point now() { return trace_clock::now(); }

  void operator<<(const trace_event& event) const;

  void operator<<(const trace_thres_event& event) const;

  void operator<<(const instant_trace_event& event) const;
};

/// \brief Specialization of file_event_tracer that does not write any events.
template <>
class file_event_tracer<false> : public detail::null_event_tracer
{
};

/// \brief Class that repurposes a log channel to write trace events.
template <bool Enabled = true>
class logger_event_tracer
{
public:
  explicit logger_event_tracer(srslog::log_channel& log_ch_) : log_ch(log_ch_) {}

  static trace_point now() { return trace_clock::now(); }

  void operator<<(const trace_event& event) const;

  void operator<<(const trace_thres_event& event) const;

  void operator<<(const instant_trace_event& event) const;

private:
  srslog::log_channel& log_ch;
};

template <>
class logger_event_tracer<false> : public detail::null_event_tracer
{
};

/// \brief Class that writes trace events to a vector of strings for testing purposes.
class test_event_tracer
{
public:
  static trace_point now() { return trace_clock::now(); }

  void operator<<(const trace_event& event);

  void operator<<(const trace_thres_event& event);

  void operator<<(const instant_trace_event& event);

  std::vector<std::string> pop_last_events() { return std::move(last_events); }

private:
  std::vector<std::string> last_events;
};

} // namespace srsran