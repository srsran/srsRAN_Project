/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/gtpu/gtpu_teid_pool_factory.h"
#include "gtpu_teid_pool_impl.h"

using namespace srsran;

std::unique_ptr<gtpu_teid_pool> srsran::create_gtpu_allocator(const gtpu_allocator_creation_request& msg)
{
  return std::make_unique<gtpu_teid_pool_impl>(msg.max_nof_teids);
}
