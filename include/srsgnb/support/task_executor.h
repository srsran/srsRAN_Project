
#ifndef SRSGNB_TASK_EXECUTOR_H
#define SRSGNB_TASK_EXECUTOR_H

#include "srsgnb/adt/move_callback.h"

namespace srsgnb {

class task_executor
{
public:
  virtual ~task_executor()               = default;
  virtual void execute(move_task_t task) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_TASK_EXECUTOR_H
