#ifndef SRSGNB_ASYNC_TASK_LOOP_H
#define SRSGNB_ASYNC_TASK_LOOP_H

#include "async_queue.h"
#include "async_task.h"
#include "lazy_task.h"
#include "srsgnb/adt/unique_function.h"

namespace srsgnb {
/// Asynchronous task that sequentially runs other enqueued asynchronous tasks
class async_task_sequencer
{
public:
  async_task_sequencer(size_t queue_size) : queue(queue_size) { run(); }
  async_task_sequencer(const async_task_sequencer&) = delete;
  async_task_sequencer& operator=(const async_task_sequencer&) = delete;

  template <typename R>
  bool schedule(lazy_task<R>&& t)
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

  async_task<void> request_stop()
  {
    // Enqueue task in case main loop is waiting for new procedure
    running = false;
    queue.try_push(launch_async([](coro_context<lazy_task<void> >& ctx) {
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
    loop_task = launch_async([this](coro_context<async_task<void> >& ctx) {
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
  async_queue<lazy_task<void> > queue;
  async_task<void>              loop_task;
  lazy_task<void>               next_task;
};

} // namespace srsgnb

#endif // SRSGNB_ASYNC_TASK_LOOP_H
