#ifndef SRSGNB_ASYNC_TASK_LOOP_H
#define SRSGNB_ASYNC_TASK_LOOP_H

#include "async_queue.h"
#include "async_task.h"
#include "srsgnb/adt/unique_function.h"

namespace srsgnb {
/// Asynchronous task that sequentially runs other enqueued asynchronous tasks
class async_task_sequencer
{
public:
  async_task_sequencer(size_t queue_size) : queue(queue_size) { run(); }
  async_task_sequencer(const async_task_sequencer&) = delete;
  async_task_sequencer& operator=(const async_task_sequencer&) = delete;

  template <typename AsyncTask, typename... Args>
  bool schedule(Args&&... args)
  {
    // TODO: right now I dont have lazy_tasks, so I have to wrap the coroutine in a lambda.
    // TODO: forward args
    return queue.try_push([&]() { return launch_async<AsyncTask>(args...); });
  }

  template <typename AsyncFunc>
  bool schedule(AsyncFunc&& async_func)
  {
    return queue.try_push([async_func = std::forward<AsyncFunc>(async_func)]() mutable {
      return launch_async(std::forward<AsyncFunc>(async_func));
    });
  }

  async_task<void> request_stop()
  {
    // Enqueue task in case main loop is waiting for new procedure
    running = false;
    queue.try_push([]() {
      return launch_async([](coro_context<async_task<void> >& ctx) {
        CORO_BEGIN(ctx);
        CORO_RETURN();
      });
    });
    return std::move(loop_task);
  }

private:
  void run()
  {
    loop_task = launch_async([this](coro_context<async_task<void> >& ctx) {
      CORO_BEGIN(ctx);

      // infinite task
      while (running) {
        // Wait for new procedure to be enqueued
        CORO_AWAIT_VALUE(next_task, queue);

        // Await for popped task to complete
        CORO_AWAIT(next_task());
      }

      CORO_RETURN();
    });
  }

  bool                                              running = true;
  async_queue<unique_function<async_task<void>()> > queue;
  async_task<void>                                  loop_task;
  unique_function<async_task<void>()>               next_task;
};

} // namespace srsgnb

#endif // SRSGNB_ASYNC_TASK_LOOP_H
