
#ifndef SRSRAN_THREAD_POOL_H
#define SRSRAN_THREAD_POOL_H

#include "srsgnb/adt/circular_buffer.h"
#include "srsgnb/adt/unique_function.h"
#include "srsgnb/srslog/srslog.h"
#include "srsgnb/support/unique_thread.h"
#include "task_executor.h"
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <stack>
#include <string>
#include <vector>

namespace srsgnb {

/// Class used to create a single worker with an input task queue with a single reader
class task_worker
{
  using task_t = unique_task;

public:
  task_worker(std::string thread_name_,
              uint32_t    queue_size,
              bool        start_postponed = false,
              int         prio_           = -1,
              int         mask_           = -1);
  task_worker(const task_worker&) = delete;
  task_worker(task_worker&&)      = delete;
  task_worker& operator=(const task_worker&) = delete;
  task_worker& operator=(task_worker&&) = delete;
  ~task_worker();

  void stop();
  void start(int prio_ = -1, int mask_ = -1);

  void push_task(task_t&& task)
  {
    auto ret = pending_tasks.try_push(std::move(task));
    if (ret.is_error()) {
      logger.error("Cannot push anymore tasks into the worker queue. maximum size is {}",
                   uint32_t(pending_tasks.max_size()));
      return;
    }
  }

  uint32_t nof_pending_tasks() const { return pending_tasks.size(); }

  std::thread::id get_id() const { return t_handle.get_id(); }

private:
  // args
  int                   prio = -1;
  int                   mask = -1;
  srslog::basic_logger& logger;

  unique_thread t_handle;

  srsgnb::dyn_blocking_queue<task_t> pending_tasks;
};

/// Executor for single-thread task worker.
class task_worker_executor : public task_executor
{
public:
  task_worker_executor() = default;
  explicit task_worker_executor(task_worker& worker_) : worker(&worker_) {}
  void execute(unique_task task) override
  {
    if (worker->get_id() == std::this_thread::get_id()) {
      // Same thread. Run task right away.
      task();
    } else {
      worker->push_task(std::move(task));
    }
  }
  void defer(unique_task task) override { worker->push_task(std::move(task)); }

private:
  task_worker* worker = nullptr;
};

inline std::unique_ptr<task_executor> make_task_executor(task_worker& w)
{
  return std::make_unique<task_worker_executor>(task_worker_executor{w});
}

} // namespace srsgnb

#endif // SRSRAN_THREAD_POOL_H
