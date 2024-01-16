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

#include "rlc_bearer_logger.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/instrumentation/traces/up_traces.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

/// \brief Recycling bin for discarded PDUs that shall be deleted by a different executor off a real-time critical path.
/// This class is intended to offload the time-consumping deletion of thousands byte_buffer objects of ACK'ed PDUs from
/// tx_window upon reception of the RLC AM status report, which is handled by the pcell_executor in a real-time critical
/// path.
class rlc_pdu_recycler
{
public:
  rlc_pdu_recycler(size_t rlc_window_size, rlc_bearer_logger& logger_) : logger(logger_)
  {
    for (std::vector<byte_buffer>& recycle_bin : recycle_bins) {
      recycle_bin.reserve(rlc_window_size);
    }
  }

  /// \brief Adds a discarded PDU to the recycler which shall be deleted later by another executor
  /// \param pdu The PDU that shall be deleted later
  /// \return True if the PDU was queued for later deletion; False if the PDU was deleted on the spot due to full queue.
  bool add_discarded_pdu(byte_buffer pdu)
  {
    if (recycle_bin_to_fill->size() >= recycle_bin_to_fill->capacity()) {
      // recycle bin is full; delete the PDUs upon return
      return false;
    }
    recycle_bin_to_fill->push_back(std::move(pdu));
    return true;
  }

  /// \brief Delegates the deletion of all stored PDUs to the given task_executor.
  void clear_by_executor(task_executor& executor)
  {
    // Swap recycle bins under a lock
    {
      std::lock_guard<std::mutex> recycle_bin_swap_lock(recycle_bin_swap_mutex);
      std::swap(recycle_bin_to_fill, recycle_bin_to_swap);
    }
    // Redirect recycling of unused byte_buffers to ue_executor
    auto handle_func = [this]() mutable { clear(); };
    if (not executor.execute(std::move(handle_func))) {
      logger.log_error("Failed to delegate deletion of PDUs to given executor.");
    }
  }

  /// \brief For unit-testing only: Expose inner recycle bins
  /// \return Reference to inner recycle bins for testing purposes
  std::array<std::vector<byte_buffer>, 3>& get_recycle_bins() { return recycle_bins; }

private:
  /// \brief Deletes the stored PDUs. This function shall be called from the executor in \c clear_by_executor.
  void clear()
  { // swap recycle bins under a lock
    trace_point clear_tp = up_tracer.now();
    {
      std::lock_guard<std::mutex> lock(recycle_bin_swap_mutex);
      std::swap(recycle_bin_to_swap, recycle_bin_to_dump);
    }
    // delete all PDUs to return their memory segments to the pool
    recycle_bin_to_dump->clear();

    up_tracer << trace_event{"rlc_clear_pdus", clear_tp};
  }

  rlc_bearer_logger& logger;

  /// Recycle bins for discarded PDUs
  std::array<std::vector<byte_buffer>, 3> recycle_bins;

  /// Pointer to an empty recycle bin that shall be filled with discarded PDUs by \c add_discarded_pdu.
  std::vector<byte_buffer>* recycle_bin_to_fill = &recycle_bins[0];
  /// Pointer to a recycle bin that can be swapped with either the recycle_bin_to_fill or the recycle_bin_to_dump.
  std::vector<byte_buffer>* recycle_bin_to_swap = &recycle_bins[1];
  /// Pointer to a recycle bin filled with PDUs that can be deleted (to return their memory segments to the pool).
  std::vector<byte_buffer>* recycle_bin_to_dump = &recycle_bins[2];
  /// Mutex for swapping the recycle bins.
  std::mutex recycle_bin_swap_mutex;
};

} // namespace srsran
