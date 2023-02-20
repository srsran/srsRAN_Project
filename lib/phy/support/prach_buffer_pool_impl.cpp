/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "prach_buffer_pool_impl.h"

using namespace srsran;

prach_buffer_pool_impl::prach_buffer_pool_impl(std::vector<std::unique_ptr<prach_buffer>>&& elements) :
  pool(std::move(elements)), current_index(0), size(pool.size())
{
}

prach_buffer& prach_buffer_pool_impl::get_prach_buffer()
{
  unsigned index = current_index % size;
  ++current_index;
  return *pool[index];
}
