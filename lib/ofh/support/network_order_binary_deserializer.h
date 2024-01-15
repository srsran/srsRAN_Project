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
#include <arpa/inet.h>
#include <cstdint>

namespace srsran {
namespace ofh {

/// This class deserializes data from the configured memory buffer in network order and returns it in host endianness.
class network_order_binary_deserializer
{
public:
  explicit network_order_binary_deserializer(span<const uint8_t> buffer) : ptr(buffer.data()), size(buffer.size())
  {
    srsran_assert(ptr, "Invalid pointer to buffer");
  }

  /// \brief Deserializes a T from the buffer and advances the position by sizeof(T) bytes.
  ///
  /// \note This function only accepts POD types.
  template <typename T>
  T read()
  {
    static_assert(std::is_pod<T>::value, "Unsupported data type");

    switch (sizeof(T)) {
      case 1:
        return read_one_byte();
      case 2:
        return read_two_bytes();
      case 4:
        return read_four_bytes();
      default:
        srsran_assert(0, "Deserializer does not support this data");
    }
    return T();
  }

  /// Deserializes a fixed amount of elements given by the size of the input span and advances the position by sizeof(T)
  /// bytes for each element.
  template <typename T>
  void read(span<T> s)
  {
    for (auto& element : s) {
      element = read<T>();
    }
  }

  /// Deserializes a fixed amount of bytes given by the size of the input span and advances the position by the span
  /// size.
  void read(span<uint8_t> s)
  {
    std::memcpy(s.data(), ptr, s.size() * sizeof(uint8_t));
    advance(s.size() * sizeof(uint8_t));
  }

  /// Advances the offset by the given amount.
  void advance(unsigned x)
  {
    ptr += x;
    offset += x;
  }

  /// Returns the current offset.
  unsigned get_offset() const { return offset; }

  /// Returns the number of unconsumed bytes of the buffer.
  unsigned remaining_bytes() const { return size - offset; }

private:
  /// Deserializes one byte and advances the position by one byte.
  uint8_t read_one_byte()
  {
    uint8_t value;
    std::memcpy(&value, ptr, sizeof(value));
    advance(sizeof(value));

    return value;
  }

  /// Deserializes two bytes and advances the position by two bytes.
  uint16_t read_two_bytes()
  {
    uint16_t value;
    std::memcpy(&value, ptr, sizeof(value));
    advance(sizeof(value));

    return ntohs(value);
  }

  /// Deserializes four bytes and advances the position by four bytes.
  uint32_t read_four_bytes()
  {
    uint32_t value;
    std::memcpy(&value, ptr, sizeof(value));
    advance(sizeof(value));

    return ntohl(value);
  }

private:
  const uint8_t* ptr;
  const unsigned size;
  unsigned       offset = 0;
};

} // namespace ofh
} // namespace srsran
