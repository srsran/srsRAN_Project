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

#include "srsran/support/tracing/event_tracing.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/executors/unique_thread.h"
#include "srsran/support/format/custom_formattable.h"
#include "srsran/support/format/fmt_basic_parser.h"
#include "srsran/support/format/fmt_to_c_str.h"
#include "srsran/support/tracing/rusage_trace_recorder.h"
#include "fmt/chrono.h"
#include <sched.h>

using namespace srsran;
using namespace std::chrono;

namespace {

struct instant_trace_event_extended;

/// Helper class to write trace events to a file.
class event_trace_writer
{
public:
  /// \brief Create a event tracer writer backend.
  /// \param trace_fname     Tracing filename.
  /// \param split_after_n_  Number of lines for splitting the file. Set to zero for no split.
  /// \param event_trigger_n Number of events to store for event triggered tracing. Set to zero for writing all events.
  explicit event_trace_writer(std::string_view trace_fname, unsigned split_after_n_, unsigned event_trigger_n) :
    split_after_n(split_after_n_), trace_worker("tracer_worker", 4096, microseconds{200}), event_ring(event_trigger_n)
  {
    size_t pos_ext = trace_fname.find_last_of(".");
    report_fatal_error_if_not(pos_ext != std::string::npos, "Unable to derive extension of filename {}", trace_fname);
    base_fname = trace_fname.substr(0, pos_ext);
    ext_fname  = trace_fname.substr(pos_ext);
    create_new_file();
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
    ::fclose(fptr);
  }

  template <typename EventType>
  void write_trace(const EventType& ev)
  {
    if (not trace_worker.push_task([this, ev]() { backend_enqueue_event(ev); })) {
      if (not warn_logged.exchange(true, std::memory_order_relaxed)) {
        file_event_tracer<true> warn_tracer;
        warn_tracer << instant_trace_event{"trace_overflow", instant_trace_event::cpu_scope::global};
        srslog::fetch_basic_logger("ALL").warning("Tracing thread cannot keep up with the number of events.");
      }
    }
  }

private:
  template <typename EventType>
  void backend_write_event(const EventType& ev)
  {
    if (SRSRAN_LIKELY(nof_lines > 0)) {
      fmt::print(fptr, ",\n{}", ev);
    } else {
      fmt::print(fptr, "\n{}", ev);
    }
    ++nof_lines;
    if (split_after_n > 0 and nof_lines >= split_after_n) {
      // In case splitting is enabled and we reached the end of the current file, we open a new one.
      create_new_file();
    }
  }

  template <typename EventType>
  void backend_enqueue_event(const EventType& ev)
  {
    // Skip enqueueing the event in the ring buffer if event trigger is not configured.
    if (event_ring.max_size() == 0) {
      backend_write_event(ev);
      return;
    }

    // Pop oldest event if the event ring is full.
    if (event_ring.full()) {
      event_ring.pop();
    }

    // Push task for writing the event.
    event_ring.push([this, ev]() { backend_write_event(ev); });

    // Execute all events in the ring buffer if the event is instant with a severe criticality.
    if constexpr (std::is_same_v<EventType, instant_trace_event_extended>) {
      if (ev.criticality >= event_criticality_threshold) {
        while (!event_ring.empty()) {
          event_ring.top()();
          event_ring.pop();
        }
      }
    }
  }

  void create_new_file()
  {
    if (fptr != nullptr) {
      // If another file is already opened, close it.
      fmt::print(fptr, "\n]");
      ::fclose(fptr);
      fptr = nullptr;
    }

    fmt::memory_buffer fmtbuf;
    if (split_after_n > 0) {
      fmt::format_to(std::back_inserter(fmtbuf), "{}{}{}", base_fname, next_file, ext_fname);
      ++next_file;
    } else {
      fmt::format_to(std::back_inserter(fmtbuf), "{}{}", base_fname, ext_fname);
    }
    fptr = ::fopen(to_c_str(fmtbuf), "w");
    report_fatal_error_if_not(fptr != nullptr, "ERROR: Failed to open trace file {}", to_c_str(fmtbuf));
    nof_lines = 0;
    fmt::print(fptr, "[");
  }

  /// Number of events, after which a new trace file is created.
  const unsigned split_after_n;

  /// Represents the filename before the extension (e.g. '.json').
  std::string_view base_fname;

  /// File extension (e.g. '.json').
  std::string_view ext_fname;

  /// Number of the next file, in case splitting is enabled.
  unsigned next_file = 0;

  /// Number of events processed for the current file.
  long unsigned nof_lines = 0;

  /// Currently opened filed to which events are being written into.
  FILE* fptr = nullptr;

  /// Task worker to process events.
  general_task_worker<concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::sleep> trace_worker;

  /// Event tracing criticality threshold for writing the file.
  static constexpr instant_trace_event::event_criticality event_criticality_threshold =
      instant_trace_event::event_criticality::severe;

  /// Ring of events to process when an instant event with severe criticality is received.
  ring_buffer<unique_function<void(), default_unique_function_buffer_size, false>> event_ring;

  std::atomic<bool> warn_logged{false};
};

struct trace_event_extended : public trace_event {
  unsigned       cpu;
  const char*    thread_name;
  trace_duration duration;

  trace_event_extended(const trace_event& event, trace_duration duration_) :
    trace_event(event), cpu(::sched_getcpu()), thread_name(this_thread_name()), duration(duration_)
  {
  }
};

struct instant_trace_event_extended : public instant_trace_event {
  unsigned    cpu;
  const char* thread_name;
  trace_point tp;

  instant_trace_event_extended(const instant_trace_event& event) :
    instant_trace_event(event), cpu(::sched_getcpu()), thread_name(this_thread_name()), tp(trace_point::clock::now())
  {
  }
};

struct rusage_trace_event_extended : public trace_event_extended {
  resource_usage::diff rusage_diff;

  rusage_trace_event_extended(const trace_event& event, trace_duration duration_, resource_usage::diff rusage_diff_) :
    trace_event_extended(event, duration_), rusage_diff(rusage_diff_)
  {
  }
};

} // namespace

/// Epoch used to compute relative timestamps for trace events.
static const trace_point run_epoch = trace_clock::now();
/// Unique event trace file writer.
static std::unique_ptr<event_trace_writer> trace_file_writer;

void srsran::open_trace_file(std::string_view trace_file_name, unsigned split_after_n, unsigned event_trigger_n)
{
  if (trace_file_writer != nullptr) {
    report_fatal_error("Trace file '{}' already open", trace_file_name);
  }
  trace_file_writer = std::make_unique<event_trace_writer>(trace_file_name, split_after_n, event_trigger_n);
}

void srsran::close_trace_file()
{
  trace_file_writer = nullptr;
}

bool srsran::is_trace_file_open()
{
  return trace_file_writer != nullptr;
}

/// Helper to get an approximation of the system clock timestamp.
static auto formatted_date(trace_point start_tp)
{
  /// Caching and mapping of system_clock with trace points for %H:%M:%S formatting.
  static system_clock::time_point cached_sys_tp   = system_clock::now();
  static trace_point              cached_trace_tp = trace_clock::now();

  if (start_tp - cached_trace_tp > seconds{1}) {
    // Recompute the mapping of system clock to trace points to compensate for drifts.
    cached_sys_tp   = system_clock::now();
    cached_trace_tp = trace_clock::now();
  }

  return make_formattable([start_tp](auto& ctx) {
    // Retrieve system clock approximation
    auto    systp        = cached_sys_tp + (start_tp - cached_trace_tp);
    std::tm current_time = fmt::gmtime(high_resolution_clock::to_time_t(systp));
    auto    us_fraction  = std::chrono::duration_cast<microseconds>(systp.time_since_epoch()).count() % 1000000u;
    return fmt::format_to(ctx.out(), "{:%H:%M:%S}.{:06}", current_time, us_fraction);
  });
}

namespace fmt {

/// Common fmt parser to all events.
struct trace_format_parser {
  bool is_log;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    auto it = ctx.begin();
    is_log  = false;
    while (it != ctx.end() and *it != '}') {
      if (*it == 'l') {
        is_log = true;
      }
      ++it;
    }
    return it;
  }
};

template <>
struct formatter<trace_event_extended> : public trace_format_parser {
  template <typename FormatContext>
  auto format(const trace_event_extended& event, FormatContext& ctx) const
  {
    auto ts = duration_cast<microseconds>(event.start_tp - run_epoch).count();

    if (is_log) {
      return format_to(ctx.out(),
                       "event=\"{}\": cpu={} tid=\"{}\" tstamp={} ts={}_usec dur={}_usec",
                       event.name,
                       event.cpu,
                       event.thread_name,
                       formatted_date(event.start_tp),
                       ts,
                       event.duration.count());
    }

    return format_to(ctx.out(),
                     "{{\"args\": {{}}, \"pid\": {{}}, \"cpu\": {}, \"tid\": \"{}\", "
                     "\"dur\": {}, \"ts\": {}, \"cat\": \"process\", \"ph\": \"X\", "
                     "\"name\": \"{}\"}}",
                     event.cpu,
                     event.thread_name,
                     event.duration.count(),
                     ts,
                     event.name);
  }
};

template <>
struct formatter<instant_trace_event_extended> : public trace_format_parser {
  template <typename FormatContext>
  auto format(const instant_trace_event_extended& event, FormatContext& ctx) const
  {
    static const char* scope_str[] = {"g", "p", "t"};

    auto ts = duration_cast<microseconds>(event.tp - run_epoch).count();

    if (is_log) {
      return format_to(ctx.out(),
                       "instant_event=\"{}\": cpu={} tid=\"{}\" tstamp={} ts={}_usec",
                       event.name,
                       event.cpu,
                       event.thread_name,
                       formatted_date(event.tp),
                       ts);
    }

    return format_to(ctx.out(),
                     "{{\"args\": {{\"tstamp\": \"{}\"}}, \"pid\": {{}},  \"cpu\": {}, \"tid\": \"{}\", "
                     "\"ts\": {}, \"cat\": \"process\", \"ph\": \"i\", \"s\": \"{}\", "
                     "\"name\": \"{}\"}}",
                     formatted_date(event.tp),
                     event.cpu,
                     event.thread_name,
                     ts,
                     scope_str[(unsigned)event.scope],
                     event.name);
  }
};

template <>
struct formatter<rusage_trace_event_extended> : public trace_format_parser {
  template <typename FormatContext>
  auto format(const rusage_trace_event_extended& event, FormatContext& ctx) const
  {
    auto ts = duration_cast<microseconds>(event.start_tp - run_epoch).count();

    if (is_log) {
      return format_to(ctx.out(),
                       "rusage_event=\"{}\": cpu={} tid=\"{}\" start_tstamp={} ts={}_usec dur={}_usec "
                       "vol_ctxt_switch={} invol_ctxt_switch={}",
                       event.name,
                       event.cpu,
                       event.thread_name,
                       formatted_date(event.start_tp),
                       ts,
                       event.duration.count(),
                       event.rusage_diff.vol_ctxt_switch_count,
                       event.rusage_diff.invol_ctxt_switch_count);
    }

    return format_to(ctx.out(),
                     "{{\"args\": {{\"start_tstamp\": \"{}\", \"vol_ctxt_switch\": {}, \"invol_ctxt_switch\": {}}}, "
                     "\"pid\": {}, \"tid\": \"{}\", \"dur\": {}, "
                     "\"ts\": {}, \"cat\": \"process\", \"ph\": \"X\", "
                     "\"name\": \"{}\"}}",
                     formatted_date(event.start_tp),
                     event.rusage_diff.vol_ctxt_switch_count,
                     event.rusage_diff.invol_ctxt_switch_count,
                     event.cpu,
                     event.thread_name,
                     event.duration.count(),
                     ts,
                     event.name);
  }
};

} // namespace fmt

template <>
bool file_event_tracer<true>::is_enabled() const
{
  return is_trace_file_open();
}

template <>
void file_event_tracer<true>::operator<<(const trace_event& event) const
{
  if (not is_enabled()) {
    return;
  }
  const auto dur = std::chrono::duration_cast<trace_duration>(now() - event.start_tp);
  trace_file_writer->write_trace(trace_event_extended{event, dur});
}

template <>
void file_event_tracer<true>::operator<<(const trace_thres_event& event) const
{
  if (not is_enabled()) {
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
  if (not is_enabled()) {
    return;
  }
  trace_file_writer->write_trace(instant_trace_event_extended{event});
}

template <>
void file_event_tracer<true>::operator<<(const rusage_trace_event& event) const
{
  if (not is_enabled()) {
    return;
  }
  const auto dur = std::chrono::duration_cast<trace_duration>(now() - event.start_tp);
  trace_file_writer->write_trace(rusage_trace_event_extended{
      trace_event{event.name, event.start_tp}, dur, resource_usage::now().value() - event.rusg_capture});
}

template <>
void file_event_tracer<true>::operator<<(const rusage_thres_trace_event& event) const
{
  if (not is_enabled()) {
    return;
  }
  const auto dur = std::chrono::duration_cast<trace_duration>(now() - event.start_tp);
  if (dur >= event.thres) {
    trace_file_writer->write_trace(rusage_trace_event_extended{
        trace_event{event.name, event.start_tp}, dur, resource_usage::now().value() - event.rusg_capture});
  }
}

template <>
void file_event_tracer<true>::operator<<(span<const rusage_trace_event> events) const
{
  if (not is_enabled()) {
    return;
  }
  // Log total
  trace_file_writer->write_trace(rusage_trace_event_extended{
      trace_event{events.front().name, events.front().start_tp},
      std::chrono::duration_cast<trace_duration>(events.back().start_tp - events.front().start_tp),
      events.back().rusg_capture - events.front().rusg_capture});
  if (events.size() > 2) {
    auto prev_it = events.begin();
    for (auto it = prev_it + 1; it != events.end(); prev_it = it++) {
      trace_file_writer->write_trace(
          rusage_trace_event_extended{trace_event{it->name, prev_it->start_tp},
                                      std::chrono::duration_cast<trace_duration>(it->start_tp - prev_it->start_tp),
                                      it->rusg_capture - prev_it->rusg_capture});
    }
  }
}

template <>
void logger_event_tracer<true>::push(const trace_event& event) const
{
  (*log_ch)("{:l}", trace_event_extended{event, std::chrono::duration_cast<trace_duration>(now() - event.start_tp)});
}

template <>
void logger_event_tracer<true>::push(const trace_thres_event& event) const
{
  const trace_duration dur = std::chrono::duration_cast<trace_duration>(now() - event.start_tp);
  if (dur >= event.thres) {
    (*log_ch)("{:l}", trace_event_extended{trace_event{event.name, event.start_tp}, dur});
  }
}

template <>
void logger_event_tracer<true>::push(const instant_trace_event& event) const
{
  (*log_ch)("{:l}", instant_trace_event_extended{event});
}

template <>
void logger_event_tracer<true>::push(const rusage_trace_event& event) const
{
  const auto dur = std::chrono::duration_cast<trace_duration>(now() - event.start_tp);
  (*log_ch)(
      "{:l}",
      rusage_trace_event_extended{trace_event{event.name, event.start_tp},
                                  dur,
                                  resource_usage::now().value_or(resource_usage::snapshot{}) - event.rusg_capture});
}

template <>
void logger_event_tracer<true>::push(const rusage_thres_trace_event& event) const
{
  const auto dur = std::chrono::duration_cast<trace_duration>(now() - event.start_tp);
  if (dur >= event.thres) {
    (*log_ch)(
        "{:l}",
        rusage_trace_event_extended{trace_event{event.name, event.start_tp},
                                    dur,
                                    resource_usage::now().value_or(resource_usage::snapshot{}) - event.rusg_capture});
  }
}

template <>
void logger_event_tracer<true>::push(span<const rusage_trace_event> events) const
{
  // Log total
  (*log_ch)("{:l}",
            rusage_trace_event_extended{
                trace_event{events.front().name, events.front().start_tp},
                std::chrono::duration_cast<trace_duration>(events.back().start_tp - events.front().start_tp),
                events.back().rusg_capture - events.front().rusg_capture});
  if (events.size() > 2) {
    auto prev_it = events.begin();
    for (auto it = prev_it + 1; it != events.end(); prev_it = it++) {
      (*log_ch)(
          "{:l}",
          rusage_trace_event_extended{trace_event{it->name, prev_it->start_tp},
                                      std::chrono::duration_cast<trace_duration>(it->start_tp - prev_it->start_tp),
                                      it->rusg_capture - prev_it->rusg_capture});
    }
  }
}

void test_event_tracer::operator<<(const trace_event& event)
{
  const auto end_tp = now();
  last_events.push_back(
      fmt::format(is_log_stype ? "{:l}" : "{}",
                  trace_event_extended{event, std::chrono::duration_cast<trace_duration>(end_tp - event.start_tp)}));
}

void test_event_tracer::operator<<(const trace_thres_event& event)
{
  const trace_duration dur = std::chrono::duration_cast<trace_duration>(now() - event.start_tp);
  if (dur >= event.thres) {
    last_events.push_back(
        fmt::format(is_log_stype ? "{:l}" : "{}", trace_event_extended{trace_event{event.name, event.start_tp}, dur}));
  }
}

void test_event_tracer::operator<<(const instant_trace_event& event)
{
  last_events.push_back(fmt::format(is_log_stype ? "{:l}" : "{}", instant_trace_event_extended{event}));
}

void test_event_tracer::operator<<(const rusage_trace_event& event)
{
  const auto dur = std::chrono::duration_cast<trace_duration>(now() - event.start_tp);
  last_events.push_back(fmt::format(is_log_stype ? "{:l}" : "{}",
                                    rusage_trace_event_extended{trace_event{event.name, event.start_tp},
                                                                dur,
                                                                resource_usage::now().value() - event.rusg_capture}));
}

void test_event_tracer::operator<<(const rusage_thres_trace_event& event)
{
  const auto dur = std::chrono::duration_cast<trace_duration>(now() - event.start_tp);
  if (dur >= event.thres) {
    last_events.push_back(fmt::format(is_log_stype ? "{:l}" : "{}",
                                      rusage_trace_event_extended{trace_event{event.name, event.start_tp},
                                                                  dur,
                                                                  resource_usage::now().value() - event.rusg_capture}));
  }
}

void test_event_tracer::operator<<(span<const rusage_trace_event> events)
{
  // Log total
  last_events.push_back(
      fmt::format(is_log_stype ? "{:l}" : "{}",
                  rusage_trace_event_extended{
                      trace_event{events.front().name, events.front().start_tp},
                      std::chrono::duration_cast<trace_duration>(events.back().start_tp - events.front().start_tp),
                      events.back().rusg_capture - events.front().rusg_capture}));
  if (events.size() > 2) {
    auto prev_it = events.begin();
    for (auto it = prev_it + 1; it != events.end(); prev_it = it++) {
      last_events.push_back(fmt::format(
          is_log_stype ? "{:l}" : "{}",
          rusage_trace_event_extended{trace_event{it->name, prev_it->start_tp},
                                      std::chrono::duration_cast<trace_duration>(it->start_tp - prev_it->start_tp),
                                      it->rusg_capture - prev_it->rusg_capture}));
    }
  }
}
