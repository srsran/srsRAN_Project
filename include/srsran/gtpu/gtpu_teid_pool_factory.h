/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/gtpu/gtpu_teid_pool.h"
#include <memory>

namespace srsran {

struct gtpu_allocator_creation_request {
  uint32_t max_nof_teids;
};

std::unique_ptr<gtpu_teid_pool> create_gtpu_allocator(const gtpu_allocator_creation_request& msg);

} // namespace srsran
