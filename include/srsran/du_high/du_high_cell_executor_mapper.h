
#pragma once

#include "srsran/ran/du_types.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

/// This interface is used to allow the DU to choose between different DL task executors.
class du_high_cell_executor_mapper
{
public:
  virtual ~du_high_cell_executor_mapper() = default;

  virtual task_executor& executor(du_cell_index_t cell_index) = 0;

  virtual task_executor& slot_ind_executor(du_cell_index_t cell_index) = 0;
};

} // namespace srsran
