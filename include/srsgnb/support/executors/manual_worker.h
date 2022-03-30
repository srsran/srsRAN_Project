
#ifndef SRSGNB_MANUAL_WORKER_H
#define SRSGNB_MANUAL_WORKER_H

#include "srsgnb/adt/circular_buffer.h"
#include "task_executor.h"

namespace srsgnb {

/// \brief Task worker that implements the executor interface and requires manual calls to run pending deferred tasks.
/// Useful for unit testing.
class manual_worker : public task_executor
{
public:
  manual_worker(size_t q_size) : pending_tasks(q_size) {}

  void execute(unique_task task) override
  {
    if (std::this_thread::get_id() == t_id) {
      task();
    } else {
      defer(std::move(task));
    }
  }

  void defer(unique_task task) override { pending_tasks.push_blocking(std::move(task)); }

  bool has_pending_tasks() const { return not pending_tasks.empty(); }

  bool is_stopped() const { return pending_tasks.is_stopped(); }

  void stop()
  {
    if (not is_stopped()) {
      pending_tasks.stop();
    }
  }

  void request_stop()
  {
    defer([this]() { stop(); });
  }

  /// Run next pending task if it is enqueued.
  bool run_next()
  {
    if (not has_thread_id()) {
      // if not initialized.
      t_id = std::this_thread::get_id();
    }
    bool        success;
    unique_task t = pending_tasks.pop_blocking(&success);
    if (not success) {
      return false;
    }
    t();
    return true;
  }

private:
  bool has_thread_id() const { return t_id != std::thread::id{}; }

  std::thread::id                 t_id;
  dyn_blocking_queue<unique_task> pending_tasks;
};

} // namespace srsgnb

#endif // SRSGNB_MANUAL_WORKER_H
