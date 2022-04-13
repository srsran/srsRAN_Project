
#ifndef SRSGNB_DU_L2_DL_EXECUTOR_MAPPER_H
#define SRSGNB_DU_L2_DL_EXECUTOR_MAPPER_H

#include "srsgnb/ran/du_types.h"
#include "srsgnb/support/executors/task_executor.h"

namespace srsgnb {

/// This interface is used to allow the DU to choose between different DL task executors.
class du_l2_dl_executor_mapper
{
public:
  virtual ~du_l2_dl_executor_mapper() = default;

  virtual task_executor& executor(du_cell_index_t cell_index) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_DU_L2_DL_EXECUTOR_MAPPER_H
