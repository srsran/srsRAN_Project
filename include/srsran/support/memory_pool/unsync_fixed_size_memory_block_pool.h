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

#include "memory_block_list.h"

namespace srsran {

/// \brief Unsynchronized (not thread-safe) memory pool of memory blocks of equal size.
class unsync_fixed_size_memory_block_pool
{
public:
  /// Ctor of the memory pool.
  explicit unsync_fixed_size_memory_block_pool(size_t nof_blocks_, size_t memory_block_size_) :
    mblock_size(align_next(memory_block_size_, alignof(std::max_align_t))), nof_blocks(nof_blocks_)
  {
    srsran_assert(mblock_size > free_memory_block_list::min_memory_block_align(),
                  "Segment size is too small ({} <= {})",
                  mblock_size,
                  free_memory_block_list::min_memory_block_align());

    // Allocate the required memory for the given number of segments and segment size.
    size_t total_mem = mblock_size * nof_blocks;
    allocated_memory.resize(total_mem);

    // Push all segments to the central cache.
    for (unsigned i = 0; i != nof_blocks; ++i) {
      block_free_list.push(static_cast<void*>(allocated_memory.data() + (mblock_size * i)));
    }
  }
  unsync_fixed_size_memory_block_pool(const unsync_fixed_size_memory_block_pool&)            = delete;
  unsync_fixed_size_memory_block_pool(unsync_fixed_size_memory_block_pool&&)                 = delete;
  unsync_fixed_size_memory_block_pool& operator=(const unsync_fixed_size_memory_block_pool&) = delete;
  unsync_fixed_size_memory_block_pool& operator=(unsync_fixed_size_memory_block_pool&&)      = delete;

  /// Memory block size in bytes.
  size_t memory_block_size() const { return mblock_size; }

  /// Number of memory blocks contained in this memory pool.
  size_t nof_memory_blocks() const { return nof_blocks; }

  /// Number of blocks currently available to be allocated.
  size_t nof_blocks_available() const { return block_free_list.size(); }

  /// Whether the pool is empty.
  size_t empty() const { return block_free_list.empty(); }

  /// Allocate a node from the memory pool with the provided size.
  void* allocate_node(size_t sz) noexcept
  {
    srsran_assert(sz <= mblock_size, "Allocated node size={} exceeds max object size={}", sz, mblock_size);

    // pop from free memory block list. If the free list is empty, null is returned.
    void* node = block_free_list.try_pop();
    return node;
  }

  /// Deallocate node by returning it back to the memory pool.
  void deallocate_node(void* p)
  {
    srsran_assert(p != nullptr, "Deallocated nodes must have valid address");

    // push to free memory block list.
    block_free_list.push(p);
  }

private:
  const size_t mblock_size;
  const size_t nof_blocks;

  std::vector<uint8_t>   allocated_memory;
  free_memory_block_list block_free_list;
};

} // namespace srsran
