/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/support/math/math_utils.h"
#include "srsran/support/srsran_assert.h"
#include "srsran/support/units.h"
#include <vector>

namespace srsran {

/// \brief Represents a pool of payload buffers.
///
/// The pool comprises a byte array, and this class provides functionality to assign a specified number of bytes from
/// the pool.
class rx_payload_buffer_pool
{
  /// Minimum block size. It ensures that the payload offsets are selected using multiples of blocks.
  static constexpr std::size_t min_block_size = 64;

  /// Rounds the block size to a multiple of min_block_size.
  static std::size_t round_to_block_size(std::size_t size)
  {
    return divide_ceil(size, min_block_size) * min_block_size;
  }

public:
  /// Creates the receive transport block buffer pool from the maximum number of PRB and layers.
  rx_payload_buffer_pool(unsigned max_nof_prb, unsigned max_nof_layers) :
    pool(round_to_block_size(units::bits(max_nof_prb * 156 * 8 * max_nof_layers).truncate_to_bytes().value()))
  {
    srsran_assert(max_nof_prb != 0, "Invalid number of PRB.");
    srsran_assert(max_nof_layers != 0, "Invalid number of layers.");
  }

  /// Resets the payload buffer pool.
  void reset() { available = pool; }

  /// Returns the next available portion of the pool.
  span<uint8_t> acquire_payload_buffer(units::bytes size)
  {
    // Round the number of consumed bytes to the next block.
    size_t count = round_to_block_size(size.value());

    // Return an invalid buffer if there is no sufficient space in the buffer.
    if (count > available.size()) {
      return {};
    }

    // Select the first bytes as the payload for the transmission.
    span<uint8_t> payload = available.first(size.value());

    // Advance available bytes.
    available = available.last(available.size() - count);
    return payload;
  }

private:
  /// Actual data.
  std::vector<uint8_t> pool;
  /// Span that points to the unused portion of the pool.
  span<uint8_t> available;
};

} // namespace srsran
