/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
