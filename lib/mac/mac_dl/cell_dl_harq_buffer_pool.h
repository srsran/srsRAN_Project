/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  using ue_dl_harq_buffer_list = static_vector<std::vector<uint8_t>, MAX_NOF_HARQS>;

public:
  explicit cell_dl_harq_buffer_pool(unsigned cell_nof_prbs, unsigned nof_ports, task_executor& ctrl_exec);

  void allocate_ue_buffers(du_ue_index_t ue_index, unsigned nof_harqs);

  void deallocate_ue_buffers(du_ue_index_t ue_index);

  /// Get DL HARQ buffer for a given UE.
  span<uint8_t> dl_harq_buffer(du_ue_index_t ue_index, harq_id_t h_id)
  {
    srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid UE index");
    srsran_assert(cell_buffers[ue_index].size() >= h_id, "Invalid HARQ ID={}", h_id);
    return cell_buffers[ue_index][h_id];
  }

private:
  const unsigned        max_pdu_len;
  task_executor&        ctrl_exec;
  srslog::basic_logger& logger;

  void grow_pool();

  // Pool of DL HARQ buffers for a given cell.
  std::vector<ue_dl_harq_buffer_list> cell_buffers;

  // Buffers that can be reused for the creation of new UE HARQ buffers.
  std::vector<std::vector<uint8_t>> buffer_cache;
};

} // namespace srsran