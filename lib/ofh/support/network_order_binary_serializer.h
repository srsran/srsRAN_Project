/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

  /// Returns a view over the given number of bytes and advances the offset by this amount.
  span<uint8_t> get_view_and_advance(unsigned x)
  {
    span<uint8_t> view(ptr, x);
    advance(x);
    return view;
  }

  /// Returns the current offset.
  unsigned get_offset() const { return offset; }

private:
  uint8_t* ptr;
  unsigned offset = 0;
};

} // namespace ofh
} // namespace srsran
