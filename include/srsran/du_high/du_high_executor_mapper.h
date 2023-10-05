
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
  /// Method to signal the detection of a new UE and potentially change its executors based on its
  /// parameters (e.g. PCell).
  /// \param ue_index Index of the UE.
  /// \param pcell_index Primary Cell of the new UE.
  /// \return task executor of this UE.
  virtual void rebind_executors(du_ue_index_t ue_index, du_cell_index_t pcell_index) = 0;

  /// \brief Returns the executor for a given UE that handles control tasks.
  ///
  /// This executor should be used for tasks that affect the UE state and that we are not too frequent.
  /// \param ue_index Index of the UE.
  /// \return task executor of the UE with given UE Index.
  virtual task_executor& ctrl_executor(du_ue_index_t ue_index) = 0;

  /// \brief Returns the executor currently registered for F1-U Rx PDU handling for a given UE.
  /// \param ue_index Index of the UE.
  /// \return task executor of the UE with given UE Index.
  virtual task_executor& f1u_dl_pdu_executor(du_ue_index_t ue_index) = 0;

  /// Returns the executor currently registered for MAC Rx PDU handling for a given UE.
  /// \param ue_index Index of the UE.
  /// \return task executor of the UE with given UE Index.
  virtual task_executor& mac_ul_pdu_executor(du_ue_index_t ue_index) = 0;

  /// Method to return the default executor with no associated UE index.
  /// \param ue_index Index of the UE.
  /// \return task executor.
  task_executor& executor() { return ctrl_executor(INVALID_DU_UE_INDEX); }
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
