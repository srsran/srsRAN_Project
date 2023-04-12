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

#include "srsran/support/event_tracing.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

template <typename Exec, typename Tracer>
class trace_executor final : public task_executor
{
public:
  trace_executor(Exec exec_, Tracer tracer_, const char* name_) :
    exec(std::forward<Exec>(exec_)),
    tracer(tracer_),
    enqueue_event_name(fmt::format("{}_enqueue", name_)),
    run_event_name(fmt::format("{}_run", name_))
  {
  }

  bool execute(unique_task task) override
  {
    auto enqueue_tp = trace_clock::now();
    return get(exec).execute([this, task = std::move(task), enqueue_tp]() mutable {
      tracer << trace_event(enqueue_event_name.c_str(), enqueue_tp);
      auto process_tp = trace_clock::now();
      task();
      tracer << trace_event(run_event_name.c_str(), process_tp);
    });
  }

  bool defer(unique_task task) override
  {
    auto enqueue_tp = trace_clock::now();
    return get(exec).defer([this, task = std::move(task), enqueue_tp]() mutable {
      tracer << trace_event(enqueue_event_name.c_str(), enqueue_tp);
      auto process_tp = trace_clock::now();
      task();
      tracer << trace_event(run_event_name.c_str(), process_tp);
    });
  }

private:
  template <typename U>
  U& get(U* u)
  {
    return *u;
  }

  template <typename U>
  U& get(std::unique_ptr<U> u)
  {
    return *u;
  }

  template <typename U>
  U& get(U& u)
  {
    return u;
  }

  Exec        exec;
  Tracer      tracer;
  std::string enqueue_event_name;
  std::string run_event_name;
};

/// \brief Specialization for null event tracer.
template <typename Exec>
class trace_executor<Exec, null_event_tracer> final : public task_executor
{
public:
  trace_executor(Exec exec_, null_event_tracer& /**/, const char* /**/) : exec(std::move(exec_)) {}

  bool execute(unique_task task) override { return get(exec).execute(std::move(task)); }

  bool defer(unique_task task) override { return get(exec).defer(std::move(task)); }

private:
  template <typename U>
  U& get(U* u)
  {
    return *u;
  }

  template <typename U>
  U& get(std::unique_ptr<U> u)
  {
    return *u;
  }

  template <typename U>
  U& get(U& u)
  {
    return u;
  }

  Exec exec;
};

template <typename Exec, typename Tracer>
trace_executor<Exec, Tracer> make_trace_executor(const char* name, Exec&& exec, Tracer&& tracer)
{
  return trace_executor<Exec, Tracer>(std::forward<Exec>(exec), std::forward<Tracer>(tracer), name);
}

} // namespace srsran