
#ifndef SRSGNB_TASK_EXECUTOR_H
#define SRSGNB_TASK_EXECUTOR_H

#include "srsgnb/adt/unique_function.h"

namespace srsgnb {

class task_executor
{
public:
  virtual ~task_executor() = default;

  /// Dispatches a task to be executed. According to this policy, executor may invoke task inplace if it deemed safe
  /// to do so.
  virtual void execute(unique_task task) = 0;

  /// Dispatches a task to be later executed. The task always gets enqueued for later invocation.
  virtual void defer(unique_task task) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_TASK_EXECUTOR_H
