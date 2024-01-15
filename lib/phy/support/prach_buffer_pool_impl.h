/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
