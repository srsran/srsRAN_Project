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

#include "srsran/support/event_tracing.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/format_utils.h"
#include "srsran/support/unique_thread.h"
#include <sched.h>

using namespace srsran;
using namespace std::chrono;

namespace {

trace_point run_epoch = trace_clock::now();

/// Helper class to write trace events to a file.
class event_trace_writer
{
public:
  explicit event_trace_writer(const char* trace_file) :
    fptr(fopen(trace_file, "w")), trace_worker("tracer_worker", 2048, std::chrono::microseconds{200})
  {
    if (fptr == nullptr) {
      report_fatal_error("ERROR: Failed to open trace file {}", trace_file);
    }
    fmt::print(fptr, "[");
  }
  event_trace_writer(event_trace_writer&& other) noexcept                 = delete;
  event_trace_writer(const event_trace_writer& other) noexcept            = delete;
  event_trace_writer& operator=(event_trace_writer&& other) noexcept      = delete;
  event_trace_writer& operator=(const event_trace_writer& other) noexcept = delete;

  ~event_trace_writer()
  {
    trace_worker.wait_pending_tasks();
    trace_worker.stop();
    fmt::print(fptr, "\n]");
    fclose(fptr);
  }

  template <typename EventType>
  void write_trace(const EventType& ev)
  {
    if (not trace_worker.push_task([this, ev]() {
          if (SRSRAN_LIKELY(not first_entry)) {
            fmt::print(fptr, ",\n{}", ev);
          } else {
            fmt::print(fptr, "\n{}", ev);
            first_entry = false;
          }
        })) {
      if (not warn_logged.exchange(true, std::memory_order_relaxed)) {
        file_event_tracer<true> warn_tracer;
        warn_tracer << instant_trace_event{"trace_overflow", instant_trace_event::cpu_scope::global};
        srslog::fetch_basic_logger("ALL").warning("Tracing thread cannot keep up with the number of events.");
      }
    }
  }

private:
  FILE* fptr;

  // Task worker to process events.
  general_task_worker<concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::sleep> trace_worker;

  bool              first_entry = true;
  std::atomic<bool> warn_logged{false};
};

/// Unique event trace file writer.
std::unique_ptr<event_trace_writer> trace_file_writer;

struct trace_event_extended : public trace_event {
  unsigned       cpu;
  const char*    thread_name;
  trace_duration duration;

  trace_event_extended(const trace_event& event, trace_duration duration_) :
    trace_event(event), cpu(sched_getcpu()), thread_name(this_thread_name()), duration(duration_)
  {
  }
};

struct instant_trace_event_extended : public instant_trace_event {
  unsigned    cpu;
  const char* thread_name;
  trace_point tp;

  instant_trace_event_extended(const instant_trace_event& event) :
    instant_trace_event(event), cpu(sched_getcpu()), thread_name(this_thread_name()), tp(trace_point::clock::now())
  {
  }
};

struct timestamp_data {
  hours        h;
  minutes      min;
  seconds      sec;
  microseconds usec;
};

timestamp_data get_timestamp(trace_point tp)
{
  using days = duration<int, std::ratio_multiply<hours::period, std::ratio<24>>::type>;

  timestamp_data ret;

  system_clock::duration start_dur = tp.time_since_epoch();
  start_dur -= duration_cast<days>(start_dur);
  ret.h = duration_cast<hours>(start_dur);
  start_dur -= ret.h;
  ret.min = duration_cast<minutes>(start_dur);
  start_dur -= ret.min;
  ret.sec = duration_cast<seconds>(start_dur);
  start_dur -= ret.sec;
  ret.usec = duration_cast<microseconds>(start_dur);

  return ret;
}

} // namespace

void srsran::open_trace_file(const std::string& trace_file_name)
{
  if (trace_file_writer != nullptr) {
    report_fatal_error("Trace file already open");
  }
  trace_file_writer = std::make_unique<event_trace_writer>(trace_file_name.c_str());
}

void srsran::close_trace_file()
{
  trace_file_writer = nullptr;
}

bool srsran::is_trace_file_open()
{
  return trace_file_writer != nullptr;
}

namespace fmt {

template <>
struct formatter<trace_event_extended> : public basic_fmt_parser {
  template <typename FormatContext>
  auto format(const trace_event_extended& event, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    auto ts = duration_cast<microseconds>(event.start_tp - run_epoch).count();

    return format_to(ctx.out(),
                     "{{\"args\": {{}}, \"pid\": {}, \"tid\": \"{}\","
                     "\"dur\": {}, \"ts\": {}, \"cat\": \"process\", \"ph\": \"X\","
                     "\"name\": \"{}\"}}",
                     event.cpu,
                     event.thread_name,
                     event.duration.count(),
                     ts,
                     event.name);
  }
};

template <>
struct formatter<instant_trace_event_extended> : public basic_fmt_parser {
  template <typename FormatContext>
  auto format(const instant_trace_event_extended& event, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    static const char* scope_str[] = {"g", "p", "t"};

    auto ts = duration_cast<microseconds>(event.tp - run_epoch).count();

    const timestamp_data timestamp = get_timestamp(event.tp);

    return format_to(ctx.out(),
                     "{{\"args\": {{\"tstamp\": \"{}:{}:{}.{}\"}}, \"pid\": {}, \"tid\": \"{}\","
                     "\"ts\": {}, \"cat\": \"process\", \"ph\": \"i\", \"s\": \"{}\","
                     "\"name\": \"{}\"}}",
                     timestamp.h.count(),
                     timestamp.min.count(),
                     timestamp.sec.count(),
                     timestamp.usec.count(),
                     event.cpu,
                     event.thread_name,
                     ts,
                     scope_str[(unsigned)event.scope],
                     event.name);
  }
};

} // namespace fmt

template <>
void file_event_tracer<true>::operator<<(const trace_event& event) const
{
  if (not is_trace_file_open()) {
    return;
  }
  const auto dur = std::chrono::duration_cast<trace_duration>(now() - event.start_tp);
  trace_file_writer->write_trace(trace_event_extended{event, dur});
}

template <>
void file_event_tracer<true>::operator<<(const trace_thres_event& event) const
{
  if (not is_trace_file_open()) {
    return;
  }
  const auto dur = std::chrono::duration_cast<trace_duration>(now() - event.start_tp);
  if (dur >= event.thres) {
    trace_file_writer->write_trace(trace_event_extended{trace_event{event.name, event.start_tp}, dur});
  }
}

template <>
void file_event_tracer<true>::operator<<(const instant_trace_event& event) const
{
  if (not is_trace_file_open()) {
    return;
  }
  trace_file_writer->write_trace(instant_trace_event_extended{event});
}

template <>
void logger_event_tracer<true>::operator<<(const trace_event& event) const
{
  log_ch("{}", trace_event_extended{event, std::chrono::duration_cast<trace_duration>(now() - event.start_tp)});
}

template <>
void logger_event_tracer<true>::operator<<(const trace_thres_event& event) const
{
  const trace_duration dur = std::chrono::duration_cast<trace_duration>(now() - event.start_tp);
  if (dur >= event.thres) {
    log_ch("{}", trace_event_extended{trace_event{event.name, event.start_tp}, dur});
  }
}

template <>
void logger_event_tracer<true>::operator<<(const instant_trace_event& event) const
{
  log_ch("{}", instant_trace_event_extended{event});
}

void test_event_tracer::operator<<(const trace_event& event)
{
  const auto end_tp = now();
  last_events.push_back(fmt::format(
      "{}", trace_event_extended{event, std::chrono::duration_cast<trace_duration>(end_tp - event.start_tp)}));
}

void test_event_tracer::operator<<(const trace_thres_event& event)
{
  const trace_duration dur = std::chrono::duration_cast<trace_duration>(now() - event.start_tp);
  if (dur >= event.thres) {
    last_events.push_back(fmt::format("{}", trace_event_extended{trace_event{event.name, event.start_tp}, dur}));
  }
}

void test_event_tracer::operator<<(const instant_trace_event& event)
{
  last_events.push_back(fmt::format("{}", instant_trace_event_extended{event}));
}