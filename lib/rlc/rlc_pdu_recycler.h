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

#include "rlc_bearer_logger.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/spsc_queue.h"
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
  rlc_pdu_recycler(size_t recycler_size, rlc_bearer_logger& logger_) : logger(logger_)
  {
    recycle_bin = std::make_unique<concurrent_queue<byte_buffer,
                                                    concurrent_queue_policy::lockfree_spsc,
                                                    concurrent_queue_wait_policy::non_blocking>>(recycler_size);
  }

  /// \brief Adds a discarded PDU to the recycler which shall be deleted later by another executor
  /// \param pdu The PDU that shall be deleted later
  /// \return True if the PDU was queued for later deletion; False if the PDU was deleted on the spot due to full queue.
  bool add_discarded_pdu(byte_buffer pdu) { return recycle_bin->try_push(std::move(pdu)); }

  /// \brief Delegates the deletion of all stored PDUs to the given task_executor.
  void clear_by_executor(task_executor& executor)
  {
    // Redirect recycling of unused byte_buffers to ue_executor
    if (not executor.execute([this]() mutable { clear(); })) {
      logger.log_error("Failed to delegate deletion of PDUs to given executor.");
    }
  }

  /// \brief Get the pdu recycler queue.
  /// Should only be used for testing
  concurrent_queue<byte_buffer, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::non_blocking>&
  get_recycle_queue()
  {
    return *recycle_bin;
  }

private:
  /// \brief Deletes the stored PDUs. This function shall be called from the executor in \c clear_by_executor.
  void clear()
  {
    trace_point clear_tp = up_tracer.now();
    byte_buffer discard;
    while (recycle_bin->try_pop(discard)) {
    }

    up_tracer << trace_event{"rlc_clear_pdus", clear_tp};
  }

  rlc_bearer_logger& logger;

  std::unique_ptr<
      concurrent_queue<byte_buffer, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::non_blocking>>
      recycle_bin;
};

} // namespace srsran
