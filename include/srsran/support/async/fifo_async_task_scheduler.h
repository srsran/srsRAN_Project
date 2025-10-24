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

#include "srsran/support/async/async_queue.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/eager_async_task.h"
#include "srsran/support/async/event_sender_receiver.h"

namespace srsran {

/// Asynchronous task that sequentially runs other enqueued asynchronous tasks.
class fifo_async_task_scheduler
{
public:
  explicit fifo_async_task_scheduler(size_t queue_size) : queue(queue_size) { run(); }

  fifo_async_task_scheduler(const fifo_async_task_scheduler&)            = delete;
  fifo_async_task_scheduler& operator=(const fifo_async_task_scheduler&) = delete;

  template <typename R>
  bool schedule(async_task<R>&& t)
  {
    return queue.try_push(std::move(t));
  }

  template <typename AsyncTask, typename... Args>
  bool schedule(Args&&... args)
  {
    return queue.try_push(launch_async<AsyncTask>(std::forward<Args>(args)...));
  }

  template <typename AsyncFunc>
  bool schedule(AsyncFunc&& async_func)
  {
    return queue.try_push(launch_async(std::forward<AsyncFunc>(async_func)));
  }

  void clear_pending_tasks() { queue.clear(); }

  eager_async_task<void> request_stop()
  {
    // Enqueue task in case main loop is waiting for new procedure.
    running = false;
    queue.clear();
    queue.try_push(launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    }));
    return std::move(loop_task);
  }

  size_t nof_pending_tasks() const { return queue.size(); }

  bool empty() const { return queue.size() == 0; }

  bool is_stopped() const { return loop_task.empty(); }

private:
  void run()
  {
    loop_task = launch_async([this](coro_context<eager_async_task<void>>& ctx) {
      CORO_BEGIN(ctx);

      // Runs until requested to stop.
      while (running) {
        // Wait for new procedure to be enqueued.
        CORO_AWAIT_VALUE(next_task, queue);

        // Await for popped task to complete.
        CORO_AWAIT(std::move(next_task));
      }

      CORO_RETURN();
    });
  }

  bool                          running = true;
  async_queue<async_task<void>> queue;
  eager_async_task<void>        loop_task;
  async_task<void>              next_task;
};

/// \brief Launches an asynchronous task on the given task sequencer and returns an async task that is only complete
/// when the former is complete.
///
/// This function is useful to synchronize two procedures running in separate task schedulers.
template <typename Callback,
          typename ReturnType = detail::function_return_t<decltype(&std::decay_t<Callback>::operator())>,
          std::enable_if_t<std::is_same_v<ReturnType, void>, int> = 0>
[[nodiscard]] async_task<bool> when_completed_on_task_sched(fifo_async_task_scheduler& task_sched,
                                                            Callback&&                 task_to_run)
{
  struct task_offloader {
    task_offloader(fifo_async_task_scheduler& task_sched_, Callback&& callback_) :
      task_sched(task_sched_), callback(std::forward<Callback>(callback_))
    {
    }

    void operator()(coro_context<async_task<bool>>& ctx)
    {
      CORO_BEGIN(ctx);

      task_sched.schedule(dispatched_task());

      CORO_AWAIT_VALUE(const bool result, rx);

      CORO_RETURN(result);
    }

  private:
    async_task<void> dispatched_task()
    {
      return launch_async([this, tx = rx.get_sender()](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        callback();

        tx.set();

        CORO_RETURN();
      });
    }

    fifo_async_task_scheduler& task_sched;
    Callback                   callback;
    event_receiver<void>       rx;
  };

  return launch_async<task_offloader>(task_sched, std::forward<Callback>(task_to_run));
}

template <typename Callback,
          typename ReturnType = detail::function_return_t<decltype(&std::decay_t<Callback>::operator())>,
          std::enable_if_t<not std::is_same_v<ReturnType, void>, int> = 0>
[[nodiscard]] async_task<std::optional<ReturnType>> when_completed_on_task_sched(fifo_async_task_scheduler& task_sched,
                                                                                 Callback&&                 task_to_run)
{
  struct task_offloader {
    task_offloader(fifo_async_task_scheduler& task_sched_, Callback&& callback_) :
      task_sched(task_sched_), callback(std::forward<Callback>(callback_))
    {
    }

    void operator()(coro_context<async_task<std::optional<ReturnType>>>& ctx)
    {
      CORO_BEGIN(ctx);

      task_sched.schedule(dispatched_task());

      CORO_AWAIT_VALUE(std::optional<ReturnType> result, rx);

      CORO_RETURN(result);
    }

  private:
    async_task<void> dispatched_task()
    {
      return launch_async([this, tx = rx.get_sender()](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        tx.set(callback());

        CORO_RETURN();
      });
    }

    fifo_async_task_scheduler& task_sched;
    Callback                   callback;
    event_receiver<ReturnType> rx;
  };

  return launch_async<task_offloader>(task_sched, std::forward<Callback>(task_to_run));
}

/// \brief Launches a coroutine on the given task sequencer and returns an async task that is only complete when the
/// former is complete.
///
/// This function is useful to synchronize two procedures running in separate task schedulers.
/// \tparam Routine       Type of the routine to run in the provided task scheduler.
/// \tparam ReturnType    Return type of coroutine's operator. Must be void.
/// \param task_sched     Task scheduler to which switch the execution too.
/// \param routine_to_run Routine to run in the task scheduler.
/// \return               Returns and async_task<bool> that can be awaited on.
template <typename Routine,
          typename ReturnType = detail::function_return_t<decltype(&std::decay_t<Routine>::operator())>,
          std::enable_if_t<std::is_same_v<ReturnType, void>, int> = 0>
[[nodiscard]] async_task<bool> when_coroutine_completed_on_task_sched(fifo_async_task_scheduler& task_sched,
                                                                      Routine&&                  routine_to_run)
{
  struct task_offloader {
    task_offloader(fifo_async_task_scheduler& task_sched_, Routine&& routine_) :
      task_sched(task_sched_), routine(std::forward<Routine>(routine_))
    {
    }

    void operator()(coro_context<async_task<bool>>& ctx)
    {
      CORO_BEGIN(ctx);

      task_sched.schedule(dispatched_task());

      CORO_AWAIT_VALUE(const bool result, rx);

      CORO_RETURN(result);
    }

  private:
    async_task<void> dispatched_task()
    {
      return launch_async([this, tx = rx.get_sender()](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        CORO_AWAIT(launch_routine());

        tx.set();

        CORO_RETURN();
      });
    }

    async_task<void> launch_routine() { return launch_async<Routine>(std::forward<Routine>(routine)); }

    fifo_async_task_scheduler& task_sched;
    Routine                    routine;
    event_receiver<void>       rx;
  };

  return launch_async<task_offloader>(task_sched, std::forward<Routine>(routine_to_run));
}

} // namespace srsran
