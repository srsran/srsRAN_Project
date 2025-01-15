/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/hal/phy/upper/channel_processors/pusch/ext_harq_buffer_context_repository_factory.h"

using namespace srsran;
using namespace hal;

std::shared_ptr<ext_harq_buffer_context_repository>
srsran::hal::create_ext_harq_buffer_context_repository(unsigned nof_codeblocks,
                                                       uint64_t ext_harq_buff_size,
                                                       bool     debug_mode)
{
  return std::make_shared<ext_harq_buffer_context_repository>(nof_codeblocks, ext_harq_buff_size, debug_mode);
}
