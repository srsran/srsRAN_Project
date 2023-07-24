/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include <arpa/inet.h>
#include <cstdint>
#include <cstring>

namespace srsran {
namespace ofh {

/// This class serializes input data into the configured memory buffer in network order (big-endian).
class network_order_binary_serializer
{
public:
  explicit network_order_binary_serializer(uint8_t* ptr_) : ptr(ptr_)
  {
    srsran_assert(ptr, "Invalid pointer to buffer");
  }

  /// Serializes the given value and advances the position by sizeof(x) bytes.
  void write(uint16_t x)
  {
    // Convert to network order.
    uint16_t net_x = htons(x);
    std::memcpy(ptr, &net_x, sizeof(net_x));
    advance(sizeof(net_x));
  }

  /// Serializes the given value and advances the position by sizeof(x) bytes.
  void write(int16_t x)
  {
    // Convert to network order.
    uint16_t net_x = htons(x);
    std::memcpy(ptr, &net_x, sizeof(net_x));
    advance(sizeof(net_x));
  }

  /// Serializes the given value and advances the position by sizeof(x) bytes.
  void write(uint8_t x)
  {
    std::memcpy(ptr, &x, sizeof(x));
    advance(sizeof(x));
  }

  /// Serializes the given span and advances the position by sizeof(x) bytes for each element.
  template <typename T>
  void write(span<T> s)
  {
    for (const auto elem : s) {
      write(elem);
    }
  }

  /// Serializes the given span of bytes and advances the position by the span size.
  void write(span<const uint8_t> s)
  {
    std::memcpy(ptr, s.data(), s.size() * sizeof(uint8_t));
    advance(s.size() * sizeof(uint8_t));
  }

  /// Serializes the given span of bytes and advances the position by the span size.
  void write(span<uint8_t> s) { write(span<const uint8_t>(s)); }

  /// Advances the offset by the given amount.
  void advance(unsigned x)
  {
    ptr += x;
    offset += x;
  }

  /// Returns the current offset.
  unsigned get_offset() const { return offset; }

private:
  uint8_t* ptr;
  unsigned offset = 0;
};

} // namespace ofh
} // namespace srsran
