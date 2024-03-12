/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/adt/span.h"
#include "srsran/ran/du_types.h"
#include "srsran/scheduler/harq_id.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

/// Class that manages the allocation, deallocation and fetching of DL HARQ buffers for a given cell.
class cell_dl_harq_buffer_pool
{
  /// Type representing a DL HARQ buffer.
  using dl_harq_buffer_storage = std::vector<uint8_t>;

  /// Type representing the list of DL HARQ buffers allocated for a given UE in a given cell.
  using ue_dl_harq_buffer_list = static_vector<dl_harq_buffer_storage, MAX_NOF_HARQS>;

public:
  /// \brief Construction of a DL HARQ buffer pool for a given cell.
  /// \param cell_nof_prbs Number of PRBs of the cell.
  /// \param nof_ports Number of ports of the cell.
  /// \param ctrl_exec Executor to which DL HARQ buffer allocation tasks is dispatched.
  explicit cell_dl_harq_buffer_pool(unsigned cell_nof_prbs, unsigned nof_ports, task_executor& ctrl_exec);

  /// \brief Allocate DL HARQ buffers for a newly created UE.
  void allocate_ue_buffers(du_ue_index_t ue_index, unsigned nof_harqs);

  /// \brief Deallocate DL HARQ buffers for a removed UE.
  void deallocate_ue_buffers(du_ue_index_t ue_index);

  /// Get DL HARQ buffer for a given UE.
  span<uint8_t> dl_harq_buffer(du_ue_index_t ue_index, harq_id_t h_id)
  {
    srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid UE index");
    srsran_assert(cell_buffers[ue_index].size() >= h_id, "Invalid HARQ ID={}", h_id);
    return cell_buffers[ue_index][h_id];
  }

private:
  // Maximum MAC PDU length, derived based on the cell properties.
  const unsigned max_pdu_len;
  // Executor to which DL HARQ buffer allocation tasks is dispatched in the background.
  task_executor&        ctrl_exec;
  srslog::basic_logger& logger;

  // This function dispatches a task to grow the pool of DL HARQ buffers, using the \c ctrl_exec, in case the pool
  // size decreased below a specific threshold.
  void grow_pool_in_background();

  // List of DL HARQ buffers currently allocated to UEs in the cell.
  std::vector<ue_dl_harq_buffer_list> cell_buffers;

  // DL HARQ buffers that are not associated with any UE and can be allocated to newly created UEs.
  std::vector<std::vector<uint8_t>> buffer_cache;
};

} // namespace srsran
