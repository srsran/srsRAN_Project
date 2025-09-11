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

#include "cell_dl_harq_buffer_pool.h"
#include "srsran/ran/pdsch/pdsch_constants.h"

using namespace srsran;

/// Derive maximum TB/MAC PDU length given a cell parameters.
static units::bytes derive_max_pdu_length(unsigned cell_nof_prbs, unsigned nof_ports)
{
  srsran_assert(nof_ports >= 1 and nof_ports <= pdsch_constants::CODEWORD_MAX_NOF_LAYERS, "Invalid number of ports");
  units::bits cw_max_size{pdsch_constants::MAX_NRE_PER_RB * cell_nof_prbs * nof_ports *
                          pdsch_constants::MAX_MODULATION_ORDER};
  return cw_max_size.round_up_to_bytes();
}

// (Implementation-defined) Batch of DL HARQ buffers to allocate every time the cache is getting depleted.
static constexpr unsigned DL_HARQ_ALLOC_BATCH = MAX_NOF_HARQS * 2;

// (Implementation-defined) Number of DL HARQ buffers to allocate in the control executor in each executor call. This
// value should be small to avoid blocking the control executor for too long.
static constexpr unsigned DL_HARQ_ALLOC_MINIBATCH = 2;

cell_dl_harq_buffer_pool::cell_dl_harq_buffer_pool(unsigned       cell_nof_prbs,
                                                   unsigned       nof_ports,
                                                   task_executor& ctrl_exec_) :
  max_pdu_len(derive_max_pdu_length(cell_nof_prbs, nof_ports).value()),
  ctrl_exec(ctrl_exec_),
  logger(srslog::fetch_basic_logger("MAC")),
  cell_buffers(MAX_NOF_DU_UES),
  pool(std::make_unique<std::array<dl_harq_buffer_storage, MAX_NOF_DU_UES * MAX_NOF_HARQS>>()),
  pool_elem_index(pool->size())
{
  buffer_cache.reserve(MAX_NOF_DU_UES * MAX_NOF_HARQS);

  // Preallocate DL HARQ buffers for any UEs that may be added.
  for (unsigned i = 0; i != DL_HARQ_ALLOC_BATCH; ++i) {
    auto* buffer = allocate_from_pool();
    if (not buffer) {
      break;
    }
    buffer->buffer.resize(max_pdu_len);
    buffer_cache.emplace_back(buffer);
  }
}

cell_dl_harq_buffer_pool::~cell_dl_harq_buffer_pool()
{
  // Cancel any pending background task to grow the pool.
  *pool_growth_cancelled = true;
}

void cell_dl_harq_buffer_pool::clear()
{
  *pool_growth_cancelled = true;
  pool_growth_cancelled  = std::make_shared<bool>(false);

  for (unsigned i = 0; i != cell_buffers.size(); ++i) {
    deallocate_ue_buffers(to_du_ue_index(i));
  }
}

void cell_dl_harq_buffer_pool::allocate_ue_buffers(du_ue_index_t ue_index, unsigned nof_harqs)
{
  srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid UE index");
  srsran_assert(nof_harqs <= MAX_NOF_HARQS, "Invalid maximum number of HARQs");

  ue_dl_harq_buffer_list& ue_harqs = cell_buffers[ue_index];

  if (not ue_harqs.empty()) {
    logger.error("ue={}: HARQ buffers already allocated for new UE", fmt::underlying(ue_index));
    return;
  }

  // Grow the list of HARQ buffers associated with this UE.
  while (ue_harqs.size() < nof_harqs) {
    if (buffer_cache.empty()) {
      // Allocate a new HARQ buffer as there are not enough buffers in the cache.
      // In general, we should avoid allocating a DL HARQ at this point, to avoid delaying the UE creation.
      auto* buffer = allocate_from_pool();
      if (not buffer) {
        return;
      }
      buffer->buffer.resize(max_pdu_len);
      ue_harqs.emplace_back(buffer);
      continue;
    }

    // Reuse a HARQ buffer from the cache.
    auto* buffer = allocate_from_cache();
    if (not buffer) {
      logger.warning("ue={}: No HARQ buffers available for new UE", fmt::underlying(ue_index));
      return;
    }
    ue_harqs.emplace_back(buffer);
  }

  // Defer the growth of the DL HARQ buffer cache.
  // We do not want to perform this operation at this point to avoid affecting the UE creation latency.
  grow_cache_in_background();
}

void cell_dl_harq_buffer_pool::deallocate_ue_buffers(du_ue_index_t ue_idx)
{
  srsran_assert(is_du_ue_index_valid(ue_idx), "Invalid UE index");
  ue_dl_harq_buffer_list& ue_harqs = cell_buffers[ue_idx];

  // Move allocated HARQs for this UE into the cache.
  for (auto* harq_buffer : ue_harqs) {
    buffer_cache.emplace_back(harq_buffer);
  }
  ue_harqs.clear();
}

void cell_dl_harq_buffer_pool::grow_cache_in_background()
{
  if (is_pool_depleted()) {
    return;
  }
  if (buffer_cache.size() >= DL_HARQ_ALLOC_BATCH) {
    // Stop growing the cache if it has enough DL HARQ buffers in cache.
    return;
  }

  if (not ctrl_exec.defer([this, cancel_flag_cpy = pool_growth_cancelled]() {
        if (cancel_flag_cpy) {
          // Task cancelled.
          return;
        }
        // Allocate minibatch of DL HARQ buffers and save them in cache.
        for (unsigned i = 0; i != DL_HARQ_ALLOC_MINIBATCH; ++i) {
          if (auto* buffer = allocate_from_pool()) {
            buffer->buffer.resize(max_pdu_len);
            buffer_cache.emplace_back(buffer);
            continue;
          }
          return;
        }

        // Dispatch new task to grow the cache if it hasn't yet achieved the desired size.
        grow_cache_in_background();
      })) {
    logger.warning("Failed to dispatch task to allocate DL HARQ buffers");
  }
}

cell_dl_harq_buffer_pool::dl_harq_buffer_storage* cell_dl_harq_buffer_pool::allocate_from_pool()
{
  if (is_pool_depleted()) {
    logger.warning("DL HARQ buffer pool depleted, no more buffers available");
    return nullptr;
  }

  return &(*pool)[--pool_elem_index];
}

cell_dl_harq_buffer_pool::dl_harq_buffer_storage* cell_dl_harq_buffer_pool::allocate_from_cache()
{
  // Some buffers may be still in flight after user removal.
  auto it = std::find_if(buffer_cache.rbegin(), buffer_cache.rend(), [](const dl_harq_buffer_storage* buffer) {
    return buffer->ref_cnt.load(std::memory_order_relaxed) == 0;
  });
  if (it == buffer_cache.rend()) {
    return nullptr;
  }

  auto* tmp = *it;
  buffer_cache.erase(std::next(it).base());
  return tmp;
}
