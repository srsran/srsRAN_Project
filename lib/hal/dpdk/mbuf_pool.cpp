/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/hal/dpdk/mbuf_pool.h"
#include "dpdk.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace dpdk;

mbuf_pool::mbuf_pool(rte_mempool* pool_) : pool(*pool_)
{
  srsran_assert(pool_, "Invalid mbuf pool.");
}

mbuf_pool::~mbuf_pool()
{
  // Free the memory buffer pool.
  ::free_mem_pool(pool);
}
