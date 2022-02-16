
#ifndef SRSGNB_TASK_EXECUTOR_H
#define SRSGNB_TASK_EXECUTOR_H

#include "srsgnb/adt/unique_function.h"

namespace srsgnb {

class task_executor
{
public:
  virtual ~task_executor()               = default;
  virtual void execute(unique_task task) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_TASK_EXECUTOR_H
