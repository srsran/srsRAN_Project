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

#include "srsran/adt/span.h"
#include "srsran/support/srsran_assert.h"
#include <memory>

namespace srsran {

class heap_memory_resource
{
public:
  heap_memory_resource(size_t sz) : block(std::make_unique<uint8_t[]>(sz)), block_view(block.get(), sz)
  {
    srsran_assert(block != nullptr, "Failed to allocate memory pool");
  }

  span<uint8_t> memory_block() const { return block_view; }

  size_t size() const { return block_view.size(); }

private:
  std::unique_ptr<uint8_t[]> block;
  span<uint8_t>              block_view;
};

} // namespace srsran