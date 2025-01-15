/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "apps/services/buffer_pool/buffer_pool_appconfig.h"

namespace srsran {
namespace app_services {

/// Buffer pool manager for the applications.
class buffer_pool_manager
{
public:
  explicit buffer_pool_manager(const buffer_pool_appconfig& config)
  {
    init_byte_buffer_segment_pool(config.nof_segments, config.segment_size);
  }
};

} // namespace app_services
} // namespace srsran
