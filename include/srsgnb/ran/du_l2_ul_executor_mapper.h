
#ifndef SRSGNB_DU_L2_UL_EXECUTOR_MAPPER_H
#define SRSGNB_DU_L2_UL_EXECUTOR_MAPPER_H

#include "du_types.h"
#include "rnti.h"
#include "srsgnb/support/executors/task_executor.h"

namespace srsgnb {

/// This interface is used to allow the DU to choose between different UL task scheduling strategies.
class du_l2_ul_executor_mapper
{
public:
  virtual ~du_l2_ul_executor_mapper() = default;
  /// Method to signal the detection of a new UE and potentially change its executor based on its parameters.
  /// \param rnti RNTI of new UE.
  /// \param pcell_index Primary Cell of the new UE.
  /// \return task executor of this UE.
  virtual task_executor& rebind_executor(rnti_t rnti, du_cell_index_t pcell_index) = 0;

  /// Method to return the executor to which a UE is currently binded.
  /// \param rnti RNTI of the UE
  /// \return task executor of the UE with given RNTI.
  virtual task_executor& executor(rnti_t rnti) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_DU_L2_UL_EXECUTOR_MAPPER_H
