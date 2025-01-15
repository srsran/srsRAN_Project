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

#include "srsran/adt/byte_buffer.h"

namespace srsran {

/// Buffer pool application configuration.
struct buffer_pool_appconfig {
  std::size_t nof_segments = 1048576;
  std::size_t segment_size = byte_buffer_segment_pool_default_segment_size();
};

} // namespace srsran
