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

#include "srsran/adt/expected.h"
#include "srsran/adt/span.h"
#include "srsran/ran/du_types.h"
#include "srsran/scheduler/harq_id.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/shared_transport_block.h"

namespace srsran {

/// Handle to a DL HARQ buffer managed inside the pool that allows moving the ownership of the buffer to a shared
/// transport block.
class dl_harq_buffer_handle
{
  span<uint8_t>          buffer;
  std::atomic<unsigned>* ref_cnt;

public:
  dl_harq_buffer_handle(span<uint8_t> buffer_, std::atomic<unsigned>* ref_cnt_) : buffer(buffer_), ref_cnt(ref_cnt_)
  {
    srsran_assert(ref_cnt, "Invalid reference counter");
    ref_cnt->fetch_add(1, std::memory_order::memory_order_relaxed);
  }

  ~dl_harq_buffer_handle()
  {
    if (ref_cnt) {
      ref_cnt->fetch_sub(1, std::memory_order::memory_order_acq_rel);
    }
  }

  dl_harq_buffer_handle(const dl_harq_buffer_handle& other)            = delete;
  dl_harq_buffer_handle& operator=(const dl_harq_buffer_handle& other) = delete;

  dl_harq_buffer_handle(dl_harq_buffer_handle&& other) noexcept :
    buffer(std::exchange(other.buffer, {})), ref_cnt(std::exchange(other.ref_cnt, nullptr))
  {
  }
  dl_harq_buffer_handle& operator=(dl_harq_buffer_handle&& other) = delete;

  /// Returns the buffer.
  span<uint8_t>       get_buffer() { return buffer; }
  span<const uint8_t> get_buffer() const { return buffer; }

  /// \brief Transfers the ownership of the underlying buffer to a \c shared_transport_block.
  /// \note This method can be only called once. An assertion is triggered is the buffer is already transferred.
  shared_transport_block transfer_to_buffer_view(size_t nof_bytes)
  {
    srsran_assert(ref_cnt, "Cannot transfer an empty buffer");

    auto out_buffer = buffer.first(nof_bytes);
    buffer          = {};
    // As we transfer ownership, do not increment the reference counter inside the view.
    return shared_transport_block(out_buffer, std::exchange(ref_cnt, nullptr), false);
  }
};

/// Class that manages the allocation, deallocation and fetching of DL HARQ buffers for a given cell.
class cell_dl_harq_buffer_pool
{
  /// Type representing a DL HARQ buffer.
  struct dl_harq_buffer_storage {
    dl_harq_buffer_storage() = default;
    explicit dl_harq_buffer_storage(size_t initial_size) : buffer(initial_size) {}
    explicit dl_harq_buffer_storage(std::vector<uint8_t>&& buffer_) : buffer(std::move(buffer_)) {}

    /// Reference counter, values greater than 0 mean the buffer is being used, otherwise it is free to be reused.
    std::atomic<unsigned> ref_cnt{0};
    /// HARQ buffer storage.
    std::vector<uint8_t> buffer;
  };

  /// Type representing the list of DL HARQ buffers allocated for a given UE in a given cell.
  using ue_dl_harq_buffer_list = static_vector<dl_harq_buffer_storage*, MAX_NOF_HARQS>;

public:
  /// \brief Construction of a DL HARQ buffer pool for a given cell.
  ///
  /// \param cell_nof_prbs Number of PRBs of the cell.
  /// \param nof_ports Number of ports of the cell.
  /// \param ctrl_exec Executor to which DL HARQ buffer allocation tasks is dispatched.
  cell_dl_harq_buffer_pool(unsigned cell_nof_prbs, unsigned nof_ports, task_executor& ctrl_exec);
  ~cell_dl_harq_buffer_pool();

  /// Called on cell deactivation to clear all available buffers.
  void clear();

  /// Allocate DL HARQ buffers for a newly created UE.
  void allocate_ue_buffers(du_ue_index_t ue_index, unsigned nof_harqs);

  /// Deallocate DL HARQ buffers for a removed UE.
  void deallocate_ue_buffers(du_ue_index_t ue_index);

  /// Allocates a DL HARQ buffer for a given UE.
  expected<dl_harq_buffer_handle> allocate_dl_harq_buffer(du_ue_index_t ue_index, harq_id_t h_id)
  {
    srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid UE index");
    srsran_assert(cell_buffers[ue_index].size() > h_id, "Invalid HARQ ID={}", fmt::underlying(h_id));

    auto* harq_buffer = cell_buffers[ue_index][h_id];
    if (harq_buffer->ref_cnt.load(std::memory_order_acquire) != 0) {
      return make_unexpected(default_error_t{});
    }

    return dl_harq_buffer_handle(harq_buffer->buffer, &harq_buffer->ref_cnt);
  }

private:
  /// Returns true if the pool has been depleted.
  bool is_pool_depleted() const { return pool_elem_index == 0; }

  /// This function dispatches a task to grow the cache of DL HARQ buffers, using the \c ctrl_exec, in case the cache
  /// size decreased below a specific threshold.
  void grow_cache_in_background();

  /// Returns a pointer to an unused element in the pool or nullptr if the pool is depleted.
  dl_harq_buffer_storage* allocate_from_pool();

  /// Takes an unused element from the buffer cache (reference count is 0) and returns its pointer, otherwise returns
  /// nullptr if none is found.
  dl_harq_buffer_storage* allocate_from_cache();

  /// Maximum MAC PDU length, derived based on the cell properties.
  const unsigned max_pdu_len;
  /// Executor to which DL HARQ buffer allocation tasks is dispatched in the background.
  task_executor& ctrl_exec;
  /// Logger.
  srslog::basic_logger& logger;
  /// List of DL HARQ buffers currently allocated to UEs in the cell.
  std::vector<ue_dl_harq_buffer_list> cell_buffers;
  /// DL HARQ buffers that are not associated with any UE and can be allocated to newly created UEs.
  std::vector<dl_harq_buffer_storage*> buffer_cache;
  /// Stores the DL HARQ buffers.
  std::unique_ptr<std::array<dl_harq_buffer_storage, MAX_NOF_DU_UES * MAX_NOF_HARQS>> pool;
  /// Index to the available buffer storage in the pool.
  size_t pool_elem_index;
  /// Flag used to cancel scheduled tasks that grow the cache of DL HARQ buffers.
  std::shared_ptr<bool> pool_growth_cancelled = std::make_shared<bool>(false);
};

} // namespace srsran
