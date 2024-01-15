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
#include <array>
#include <cstdint>

namespace srsran {
namespace detail {

/// \brief Span of bytes (non owning) divided into three parts [ HEADROOM | PAYLOAD | TAILROOM ].
/// Bytes can be added in the HEADROOM region via prepend() or in the TAILROOM via append().
class byte_buffer_segment
{
public:
  using value_type     = uint8_t;
  using iterator       = uint8_t*;
  using const_iterator = const uint8_t*;

  /// \brief Initialize a byte_buffer_segment with no headroom, tailroom and payload.
  byte_buffer_segment() = default;

  /// \brief Initialize byte_buffer_segment with a given buffer and headroom size. The payload size is set to 0 and the
  /// tailroom size will be equal to the given buffer size minus the headroom size.
  ///
  /// \param buffer_ Buffer where bytes will be stored.
  /// \param headroom_ Initial size of the headroom.
  explicit byte_buffer_segment(span<uint8_t> buffer_, size_t headroom_) :
    buffer(buffer_), payload(buffer_.subspan(headroom_, 0))
  {
  }

  /// \brief Returns the total number of bytes that the segment can store, i.e. the sum of the payload, headroom and
  /// tailroom sizes.
  size_t capacity() const { return buffer.size(); }

  /// Checks whether segment has no payload.
  bool empty() const { return begin() == end(); }

  /// Returns how much space in bytes there is at the head of the segment.
  size_t headroom() const { return begin() - buffer.begin(); }

  /// Returns size in bytes of segment.
  size_t length() const { return payload.size(); }

  /// Returns how much space in bytes there is at the tail of the segment.
  size_t tailroom() const { return buffer.end() - end(); }

  /// \brief Get the last byte of the byte_buffer_segment payload.
  uint8_t& back()
  {
    srsran_assert(not empty(), "back() called for empty segment.");
    return payload.back();
  }
  const uint8_t& back() const
  {
    srsran_assert(not empty(), "back() called for empty segment.");
    return payload.back();
  }

  /// \brief Get a pointer to the byte_buffer_segment payload.
  const uint8_t* data() const { return begin(); }
  uint8_t*       data() { return begin(); }

  /// Index-based access to the payload of the byte_buffer_segment.
  uint8_t& operator[](size_t idx)
  {
    srsran_assert(idx < length(), "Out-of-bound access");
    return *(begin() + idx);
  }
  const uint8_t& operator[](size_t idx) const
  {
    srsran_assert(idx < length(), "Out-of-bound access");
    return *(begin() + idx);
  }

  /// Resizes payload of segment. If the new size is larger than the old size, the new bytes are uninitialized.
  void resize(size_t nof_bytes)
  {
    srsran_assert(nof_bytes <= capacity() - headroom(), "There is not enough space for provided size");
    payload = span<uint8_t>{payload.begin(), payload.begin() + nof_bytes};
  }

  /// Appends a span of bytes at the tail of the segment.
  void append(span<const uint8_t> bytes) { append(bytes.begin(), bytes.end()); }

  /// Appends a iterator range of bytes at the tail of the segment.
  template <typename It>
  void append(It it_begin, It it_end)
  {
    static_assert(std::is_same<std::decay_t<decltype(*it_begin)>, uint8_t>::value, "Invalid value_type");
    static_assert(
        std::is_same<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>::value,
        "Only random access iterators allowed.");
    srsran_assert((size_t)(it_end - it_begin) <= tailroom(), "There is not enough tailroom for append.");
    iterator new_end = std::copy(it_begin, it_end, end());
    payload          = span<uint8_t>{payload.begin(), new_end};
  }

  /// Appends single byte at the tail of the segment.
  void append(uint8_t byte)
  {
    srsran_assert(tailroom() >= 1, "There is not enough tailroom space.");
    payload        = span<uint8_t>{payload.begin(), payload.end() + 1};
    payload.back() = byte;
  }

  /// Prepends segment with provided span of bytes.
  void prepend(span<const uint8_t> bytes)
  {
    srsran_assert(headroom() >= bytes.size(), "There is not enough headroom space.");
    payload = span<uint8_t>{payload.begin() - bytes.size(), payload.end()};
    std::copy(bytes.begin(), bytes.end(), begin());
  }

  /// Reserves headroom space in segment.
  /// \param nof_bytes Number of bytes to reserve.
  void reserve_prepend(size_t nof_bytes)
  {
    srsran_assert(headroom() >= nof_bytes, "There is not enough headroom space.");
    payload = span<uint8_t>{payload.begin() - nof_bytes, payload.end()};
  }

  /// Removes "nof_bytes" from the head of the segment.
  void trim_head(size_t nof_bytes)
  {
    srsran_assert(nof_bytes <= length(), "There is not enough headroom space.");
    payload = span<uint8_t>{payload.begin() + nof_bytes, payload.end()};
  }

  /// Removes "nof_bytes" from the tail of the segment.
  void trim_tail(size_t nof_bytes)
  {
    srsran_assert(nof_bytes <= length(), "There is not enough headroom space.");
    payload = span<uint8_t>{payload.begin(), payload.end() - nof_bytes};
  }

  /// \brief Returns iterator pointing to the start of the payload.
  iterator       begin() { return payload.begin(); }
  const_iterator begin() const { return payload.begin(); }

  /// \brief Returns iterator pointing to the end of the payload.
  iterator       end() { return payload.end(); }
  const_iterator end() const { return payload.end(); }

  /// \brief Checks if the payload of a segment is equal to the content of another range.
  template <typename Container>
  bool operator==(const Container& other) const
  {
    static_assert(std::is_same<std::decay_t<decltype(*other.begin())>, uint8_t>::value, "Invalid value_type");
    return std::equal(begin(), end(), other.begin(), other.end());
  }
  template <typename Container>
  bool operator!=(const Container& other) const
  {
    return !(*this == other);
  }

private:
  // Range of bytes spanning the beginning of the headroom and end of the tailroom.
  span<const uint8_t> buffer;

  // Range of bytes where the payload is stored.
  span<uint8_t> payload;
};

} // namespace detail
} // namespace srsran