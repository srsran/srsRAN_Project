/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/support/prach_buffer.h"
#include "srsran/phy/support/prach_buffer_pool.h"
#include <memory>

namespace srsran {

/// \brief Implementation of the PRACH buffer pool.
class prach_buffer_pool_impl : public prach_buffer_pool
{
  std::vector<std::unique_ptr<prach_buffer>> pool;
  unsigned                                   current_index = 0U;
  size_t                                     size          = 0;

public:
  /// \brief Constructs a PRACH buffer pool with the given PRACH buffers.
  explicit prach_buffer_pool_impl(std::vector<std::unique_ptr<prach_buffer>>&& elements);

  // See interface for documentation.
  prach_buffer& get_prach_buffer() override;
};

} // namespace srsran
