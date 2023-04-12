/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/support/compiler.h"
#include <chrono>
#include <string>

namespace srslog {

class log_channel;

}

namespace srsran {

/// Trace Event clock types.
using trace_clock    = std::chrono::steady_clock;
using trace_point    = trace_clock::time_point;
using trace_duration = std::chrono::microseconds;

/// \brief Open a file to write trace events to.
void open_trace_file(const char* trace_file_name = "/tmp/srsran_trace.json");

/// \brief Close the trace file. This function is called automatically when the program exits.
void close_trace_file();

/// \brief Check if the trace file is open.
bool is_trace_file_open();

/// \brief Trace event used for events with defined name, starting point and duration.
struct trace_event {
  const char* name;
  trace_point start_tp;

  trace_event(const char* name_, trace_point start_tp_) : name(name_), start_tp(start_tp_) {}
};

/// \brief Trace event with defined name, starting point but no duration.
struct instant_trace_event {
  enum class cpu_scope { global, process, thread };

  const char* name;
  trace_point tp;
  cpu_scope   scope;

  instant_trace_event(const char* name_, trace_point tp_, cpu_scope scope_) : name(name_), tp(tp_), scope(scope_) {}
};

/// \brief Tracer that does not write any events. The compiler should eliminate all calls to this tracer when
/// optimizations are enabled.
class null_event_tracer
{
public:
  trace_point now() const { return {}; }

  void operator<<(const trace_event& event) const {}

  void operator<<(const instant_trace_event& event) const {}
};

template <bool Enabled = true>
class file_event_tracer
{
public:
  trace_point now() const { return trace_clock::now(); }

  void operator<<(const trace_event& event) const;

  void operator<<(const instant_trace_event& event) const;
};

template <>
class file_event_tracer<false> : public null_event_tracer
{
};

template <bool Enabled = true>
class logger_event_tracer
{
public:
  explicit logger_event_tracer(srslog::log_channel& log_ch_) : log_ch(log_ch_) {}

  trace_point now() const { return trace_clock::now(); }

  void operator<<(const trace_event& event) const;

  void operator<<(const instant_trace_event& event) const;

private:
  srslog::log_channel& log_ch;
};

template <>
class logger_event_tracer<false> : public null_event_tracer
{
};

class test_event_tracer
{
public:
  trace_point now() const { return trace_clock::now(); }

  void operator<<(const trace_event& event);

  void operator<<(const instant_trace_event& event);

  const std::string& get_last_event() const { return last_event; }

private:
  std::string last_event;
};

} // namespace srsran