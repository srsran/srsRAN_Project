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

#include "srsran/support/memory_pool/memory_pool_utils.h"
#include "srsran/support/srsran_assert.h"
#include <cstdint>

namespace srsran {

/// \brief Linear allocator for a contiguous memory block.
///
/// This type of allocator doesn't provide a deallocation mechanism.
class linear_memory_allocator
{
public:
  linear_memory_allocator(void* memory_resource, std::size_t memory_resource_size) :
    mem_start(static_cast<uint8_t*>(memory_resource)), mem_res_size(memory_resource_size)
  {
    srsran_sanity_check(mem_start != nullptr and mem_res_size != 0, "Invalid memory resource");
  }

  std::size_t size() const { return mem_res_size; }
  std::size_t nof_bytes_left() const { return mem_res_size - mem_offset; }
  std::size_t nof_bytes_allocated() const { return mem_offset; }

  void* memory_resource_start() const { return mem_start; }

  void* allocate(std::size_t sz, std::size_t al) noexcept
  {
    void* p    = align_next(mem_start + mem_offset, al);
    mem_offset = (static_cast<uint8_t*>(p) - mem_start) + sz;
    srsran_sanity_check(mem_offset <= mem_res_size, "Out of memory");
    return p;
  }

private:
  uint8_t*    mem_start;
  std::size_t mem_res_size;
  std::size_t mem_offset = 0;
};

} // namespace srsran
