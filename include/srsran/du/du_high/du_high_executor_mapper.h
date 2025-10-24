/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/ran/du_types.h"
#include "srsran/support/executors/task_executor.h"
#include <chrono>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

namespace srsran {

class executor_metrics_channel_registry;

namespace srs_du {

/// \brief Mapper of task executors used by the MAC DL, RLC DL and MAC scheduler for low-latency tasks. The task
/// executors can be mapped based on cell index or type of task (slot indication vs others).
class du_high_cell_executor_mapper
{
public:
  virtual ~du_high_cell_executor_mapper() = default;

  /// \brief Default executor to handle MAC events for a given cell.
  virtual task_executor& mac_cell_executor(du_cell_index_t cell_index) = 0;

  /// \brief Executor to handle RLC-lower events for a given cell.
  virtual task_executor& rlc_lower_executor(du_cell_index_t cell_index) = 0;

  /// \brief Executor to handle slot_indication events for a given cell. These events are of high-priority.
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

  /// \brief Retrieve E2 control executor.
  virtual task_executor& du_e2_executor() = 0;

  /// \brief Retrieve executor used for reading messages from the F1-C SCTP socket.
  virtual task_executor& f1c_rx_executor() = 0;

  /// \brief Retrieve executor used for reading messages from the E2 SCTP socket.
  virtual task_executor& e2_rx_executor() = 0;

  /// \brief Retrieve executor used for reading messages from the F1-U UDP sockets.
  virtual task_executor& f1u_rx_executor() = 0;
};

/// Configuration of DU-high executor mapper.
struct du_high_executor_config {
  /// Dedicated serialized worker for a given cell.
  struct dedicated_cell_worker {
    /// Serialized executor used to pass slot indications to a cell in the MAC.
    task_executor* high_prio_executor;
    /// Serialized executor used to pass other tasks to a cell in the MAC.
    task_executor* low_prio_executor;
  };
  using dedicated_cell_worker_list = std::vector<dedicated_cell_worker>;
  /// Worker pool which will be used for the DU-high cell real-time tasks.
  struct strand_based_worker_pool {
    /// Number of cells of the DU-high.
    unsigned nof_cells;
    /// Default queue size for tasks other than slot indications.
    unsigned default_task_queue_size;
    /// Worker pool executors.
    std::vector<task_executor*> pool_executors;
  };
  /// \brief Configuration of the DU-high cell executors. Two options:
  /// - list of dedicated workers, one per cell, and indexed by cell index.
  /// - worker pool, to which different strands, one per cell, will be assigned.
  using cell_executor_config = std::variant<dedicated_cell_worker_list, strand_based_worker_pool>;
  /// \brief Configuration of the DU-high UE executors.
  ///
  /// Parallelism is achieved by distributing UEs across different strands.
  /// These executors are used for:
  /// - handle UL PDUs in MAC, RLC, and F1
  /// - handle DL PDUs in F1 and RLC-high
  struct ue_executor_config {
    /// Policy used to distribute UEs across strands.
    enum class map_policy { per_cell, round_robin };
    map_policy policy;
    /// Maximum number of strands to instantiate. There is a trade-off parallelization vs memory.
    unsigned max_nof_strands;
    /// Size of the queue used for control-plane tasks associated with a UE.
    unsigned ctrl_queue_size;
    /// Size of the queue used for PDU handling tasks associated with a UE.
    unsigned pdu_queue_size;
    /// Executor of a thread pool to which strands will point to.
    task_executor* pool_executor;
    /// Executor used to read the F1-U UDP socket.
    task_executor* f1u_reader_executor;
  };
  /// \brief Configuration of strand for control-plane tasks of the DU-high.
  struct control_executor_config {
    unsigned       task_queue_size;
    task_executor* pool_executor;
  };

  /// DU-high cell executors.
  cell_executor_config cell_executors;
  /// DU-high UE executors.
  ue_executor_config ue_executors;
  /// Executor used for control-plane tasks of the DU-high.
  control_executor_config ctrl_executors;
  /// Whether the DU-high is operating in a real-time or simulation environment
  bool is_rt_mode_enabled = true;
  /// Whether to trace executed tasks.
  bool trace_exec_tasks = false;
  /// \brief Optional executor metrics channel registry.
  ///
  /// If it is initialized, the executor mapper wraps the executors with metric decorators.
  executor_metrics_channel_registry* exec_metrics_channel_registry = nullptr;
};

/// \brief Creates an executor mapper for the DU-high.
std::unique_ptr<du_high_executor_mapper> create_du_high_executor_mapper(const du_high_executor_config& config);

} // namespace srs_du
} // namespace srsran
