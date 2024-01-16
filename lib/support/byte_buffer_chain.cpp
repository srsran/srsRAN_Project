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

#include "srsran/adt/byte_buffer_chain.h"
#include "srsran/adt/detail/byte_buffer_segment_pool.h"
#include "srsran/support/memory_pool/memory_pool_utils.h"

using namespace srsran;

byte_buffer_chain::byte_buffer_chain() :
  // Allocate memory block from pool for the array of slices.
  mem_block(detail::get_default_byte_buffer_segment_pool().allocate_node()),
  // Number of slices that fit in the memory block.
  max_slices(detail::get_default_byte_buffer_segment_pool().memory_block_size() / sizeof(buffer_storage_type))
{
  if (mem_block == nullptr) {
    srslog::fetch_basic_logger("ALL").warning("POOL: Failed to allocate memory block for byte_buffer_chain");
    return;
  }

  // Initialize slice array in the allocated memory block.
  slices_ptr = static_cast<byte_buffer_slice*>(align_next(mem_block.get(), alignof(buffer_storage_type)));
}

void byte_buffer_chain::block_deleter::operator()(void* p)
{
  if (p != nullptr) {
    detail::byte_buffer_segment_pool::get_instance().deallocate_node(p);
  }
}
