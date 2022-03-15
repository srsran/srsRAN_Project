
#ifndef SRSGNB_DU_L2_EXECUTOR_MAPPER_H
#define SRSGNB_DU_L2_EXECUTOR_MAPPER_H

#include "du_types.h"
#include "rnti.h"
#include "srsgnb/support/task_executor.h"

namespace srsgnb {

class du_l2_executor_mapper
{
public:
  virtual ~du_l2_executor_mapper()                                                 = default;
  virtual task_executor& rebind_executor(rnti_t rnti, du_cell_index_t pcell_index) = 0;
  virtual task_executor& executor(rnti_t rnti)                                     = 0;
};

} // namespace srsgnb

#endif // SRSGNB_DU_L2_EXECUTOR_MAPPER_H
