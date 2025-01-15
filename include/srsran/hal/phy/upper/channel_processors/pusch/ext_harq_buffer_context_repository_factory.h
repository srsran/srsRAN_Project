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

#include "srsran/hal/phy/upper/channel_processors/pusch/ext_harq_buffer_context_repository.h"
#include <memory>

namespace srsran {
namespace hal {

/// Returns a ext_harq_buffer_context_repository instance on success, otherwise returns nullptr.
std::shared_ptr<ext_harq_buffer_context_repository>
create_ext_harq_buffer_context_repository(unsigned nof_codeblocks,
                                          uint64_t ext_harq_buff_size,
                                          bool     debug_mode = false);

} // namespace hal
} // namespace srsran
