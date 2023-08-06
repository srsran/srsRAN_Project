
#pragma once

#include "srsran/ran/du_types.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

/// This interface is used to allow the DU to choose between different cell-specific task executors.
class du_high_cell_executor_mapper
{
public:
  virtual ~du_high_cell_executor_mapper() = default;

  virtual task_executor& executor(du_cell_index_t cell_index) = 0;

  virtual task_executor& slot_ind_executor(du_cell_index_t cell_index) = 0;
};

/// This interface is used to allow the DU to choose between different UE-specific task executors.
class du_high_ue_executor_mapper
{
public:
  virtual ~du_high_ue_executor_mapper() = default;
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
  task_executor& executor() { return executor(INVALID_DU_UE_INDEX); }
};

/// \brief Interface used to access different executors used in the DU-High.
class du_high_executor_mapper
{
public:
  virtual ~du_high_executor_mapper() = default;

  /// \brief Retrieve DU cell executor mapper object.
  virtual du_high_cell_executor_mapper& cell_mapper() = 0;

  /// \brief Retrieve DU UE executor mapper object.
  virtual du_high_ue_executor_mapper& ue_mapper() = 0;

  /// \brief Retrieve DU control executor.
  virtual task_executor& du_control_executor() = 0;

  /// \brief Retrieve DU timer tick dispatching executor.
  virtual task_executor& du_timer_executor() = 0;

  /// \brief Retrieve E2 control executor.
  virtual task_executor& du_e2_executor() = 0;
};

} // namespace srsran
