
#ifndef SRSGNB_RAN_DU_L2_UL_EXECUTOR_MAPPER_H
#define SRSGNB_RAN_DU_L2_UL_EXECUTOR_MAPPER_H

#include "du_types.h"
#include "srsgnb/support/executors/task_executor.h"

namespace srsgnb {

/// This interface is used to allow the DU to choose between different UL task scheduling strategies.
class du_l2_ul_executor_mapper
{
public:
  virtual ~du_l2_ul_executor_mapper() = default;
  /// Method to signal the detection of a new UE and potentially change its executor based on its
  /// parameters (e.g. PCell).
  /// \param ue_index Index of the UE.
  /// \param pcell_index Primary Cell of the new UE.
  /// \return task executor of this UE.
  virtual task_executor& rebind_executor(du_ue_index_t ue_index, du_cell_index_t pcell_index) = 0;

  /// Method to return the executor to which a UE is currently binded.
  /// \param ue_index Index of the UE.
  /// \return task executor of the UE with given UE Index.
  virtual task_executor& executor(du_ue_index_t ue_index) = 0;

  /// Method to return the default executor with no associated UE index.
  /// \param ue_index Index of the UE.
  /// \return task executor.
  task_executor& executor() { return executor(MAX_NOF_DU_UES); }
};

} // namespace srsgnb

#endif // SRSGNB_RAN_DU_L2_UL_EXECUTOR_MAPPER_H
