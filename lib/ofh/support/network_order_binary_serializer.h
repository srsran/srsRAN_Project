/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
