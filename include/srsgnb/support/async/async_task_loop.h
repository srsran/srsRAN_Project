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

#include "async_queue.h"
#include "async_task.h"
#include "eager_async_task.h"
#include "srsgnb/adt/unique_function.h"

namespace srsran {
/// Asynchronous task that sequentially runs other enqueued asynchronous tasks
class async_task_sequencer
{
public:
  async_task_sequencer(size_t queue_size) : queue(queue_size) { run(); }
  async_task_sequencer(const async_task_sequencer&)            = delete;
  async_task_sequencer& operator=(const async_task_sequencer&) = delete;

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

  eager_async_task<void> request_stop()
  {
    // Enqueue task in case main loop is waiting for new procedure
    running = false;
    queue.try_push(launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    }));
    return std::move(loop_task);
  }

  size_t nof_pending_tasks() const { return queue.size(); }

  bool empty() const { return queue.size() == 0; }

private:
  void run()
  {
    loop_task = launch_async([this](coro_context<eager_async_task<void>>& ctx) {
      CORO_BEGIN(ctx);

      // runs until requested to stop.
      while (running) {
        // Wait for new procedure to be enqueued.
        CORO_AWAIT_VALUE(next_task, queue);

        // Await for popped task to complete
        CORO_AWAIT(next_task);
      }

      CORO_RETURN();
    });
  }

  bool                          running = true;
  async_queue<async_task<void>> queue;
  eager_async_task<void>        loop_task;
  async_task<void>              next_task;
};

} // namespace srsran
