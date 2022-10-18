/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ring_allocator.h"

namespace srsgnb {

class ring_buffer_pool
{
public:
  explicit ring_buffer_pool(size_t pool_sz) : mem_res(pool_sz), alloc(mem_res.memory_block()) {}

  span<uint8_t> allocate_buffer(size_t sz)
  {
    span<uint8_t> buffer{static_cast<uint8_t*>(alloc.allocate(sz)), sz};
    return buffer;
  }

private:
  heap_memory_resource mem_res;
  ring_allocator       alloc;
};

} // namespace srsgnb