/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/support/executors/task_executor.h"
#include "srsran/support/tracing/event_tracing.h"

namespace srsran {

/// \brief A task executor that traces the latencies of the task enqueuing/dequeuing and task invocation.
template <typename Exec, typename Tracer>
class executor_tracer final : public task_executor
{
public:
  template <typename ExecType>
  executor_tracer(ExecType&& exec_, Tracer& tracer_, const std::string& name_) :
    exec(std::forward<ExecType>(exec_)),
    tracer(tracer_),
    enqueue_event_name(fmt::format("{}_enqueue", name_)),
    run_event_name(fmt::format("{}_run", name_))
  {
  }

  [[nodiscard]] bool execute(unique_task task) override
  {
    auto enqueue_tp = trace_clock::now();
    return get(exec).execute([this, task = std::move(task), enqueue_tp]() mutable {
      tracer << trace_event(enqueue_event_name.c_str(), enqueue_tp);
      auto process_tp = trace_clock::now();
      task();
      tracer << trace_event(run_event_name.c_str(), process_tp);
    });
  }

  [[nodiscard]] bool defer(unique_task task) override
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
  U& get(std::unique_ptr<U>& u)
  {
    return *u;
  }

  template <typename U>
  U& get(U& u)
  {
    return u;
  }

  Exec        exec;
  Tracer&     tracer;
  std::string enqueue_event_name;
  std::string run_event_name;
};

/// \brief Specialization for null event tracer. It should not add any overhead compared to the original executor.
template <typename Exec>
class executor_tracer<Exec, detail::null_event_tracer> final : public task_executor
{
public:
  executor_tracer(Exec exec_, detail::null_event_tracer& /**/, const std::string& /**/) : exec(std::move(exec_)) {}

  [[nodiscard]] bool execute(unique_task task) override { return get(exec).execute(std::move(task)); }

  [[nodiscard]] bool defer(unique_task task) override { return get(exec).defer(std::move(task)); }

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
executor_tracer<Exec, Tracer> make_trace_executor(const std::string& name, Exec&& exec, Tracer& tracer)
{
  return executor_tracer<Exec, Tracer>(std::forward<Exec>(exec), tracer, name);
}

template <typename Exec, typename Tracer>
std::unique_ptr<task_executor> make_trace_executor_ptr(const std::string& name, Exec&& exec, Tracer& tracer)
{
  return std::make_unique<executor_tracer<Exec, Tracer>>(std::forward<Exec>(exec), tracer, name);
}

} // namespace srsran
