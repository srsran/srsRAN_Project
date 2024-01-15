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

#include "heap_memory_resource.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

/// \brief This memory allocator allocates chunks of a contiguous memory region in a circular fashion.
class ring_memory_allocator
{
public:
  explicit ring_memory_allocator(span<uint8_t> mem_block_) : mem_block(mem_block_) {}

  void* allocate(size_t sz) noexcept
  {
    srsran_assert(sz <= mem_block.size(), "Invalid allocation size ({} > {})", sz, mem_block.size());

    if (next_offset + sz > mem_block.size()) {
      // Wrap-around.
      next_offset = sz;
      return mem_block.data();
    } else {
      uint8_t* ptr = mem_block.data() + next_offset;
      next_offset += sz;
      return ptr;
    }
  }

  void deallocate(void* p) noexcept
  {
    srsran_assert(static_cast<uint8_t*>(p) - mem_block.data() < (ssize_t)mem_block.size(),
                  "Deallocating invalid memory pointer");
  }

private:
  size_t        next_offset = 0;
  span<uint8_t> mem_block;
};

} // namespace srsran