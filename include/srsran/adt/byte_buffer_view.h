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

#include "srsran/adt/detail/byte_buffer_range_helpers.h"

namespace srsran {

/// \brief Non-owning view to a byte sequence.
///
/// The underlying byte sequence is not contiguous in memory. Instead, it is represented as an intrusive linked list of
/// byte buffer segments, where each segment contains a span of bytes.
class byte_buffer_view
{
  /// Checks whether type is a byte_buffer/byte_buffer_slice.
  template <typename ByteBufferType>
  using is_owning_byte_buffer_type = std::conditional_t<std::is_same_v<std::decay_t<ByteBufferType>, byte_buffer_view>,
                                                        std::false_type,
                                                        is_byte_buffer_range<ByteBufferType>>;

public:
  using value_type     = uint8_t;
  using iterator       = detail::byte_buffer_segment_list_byte_iterator;
  using const_iterator = detail::byte_buffer_segment_list_byte_const_iterator;

  byte_buffer_view() = default;

  /// Conversion from a pair of iterators.
  byte_buffer_view(iterator it_begin_, iterator it_end_) : it(it_begin_), it_end(it_end_) {}

  /// Conversion from byte_buffer-like type (e.g. byte_buffer, byte_buffer_slice) to byte_buffer_view.
  template <typename ByteBufferType, std::enable_if_t<is_owning_byte_buffer_type<ByteBufferType>::value, int> = 0>
  byte_buffer_view(const ByteBufferType& buffer) : it(buffer.begin()), it_end(buffer.end())
  {
  }
  template <typename ByteBufferType, std::enable_if_t<is_owning_byte_buffer_type<ByteBufferType>::value, int> = 0>
  byte_buffer_view(const ByteBufferType& buffer, size_t start, size_t sz) : it(buffer.begin() + start), it_end(it + sz)
  {
  }

  /// Get iterator pointing at the first byte of the view.
  iterator       begin() { return it; }
  const_iterator begin() const { return it; }

  /// Get iterator pointing at the end of the view.
  iterator       end() { return it_end; }
  const_iterator end() const { return it_end; }

  /// Checks whether the view is empty.
  bool empty() const { return it == it_end; }

  /// Checks the length in bytes of the view.
  size_t length() const { return it_end - it; }

  /// Index-based random access to bytes of the view.
  const uint8_t& operator[](size_t i) const { return *(it + i); }
  uint8_t&       operator[](size_t i) { return *(it + i); }

  /// Returns another sub-view with dimensions specified in arguments.
  byte_buffer_view view(size_t offset, size_t size) const
  {
    srsran_assert(offset + size <= length(), "Invalid view dimensions.");
    return {it + offset, it + offset + size};
  }

  /// Split byte buffer view into two contiguous views, with break point defined by "offset".
  /// \param offset index at which view is split into two contiguous views.
  /// \return pair of contiguous views.
  std::pair<byte_buffer_view, byte_buffer_view> split(size_t offset)
  {
    auto it_split = begin() + offset;
    return {{begin(), it_split}, {it_split, end()}};
  }

  /// Returns a non-owning list of segments that compose the byte_buffer.
  const_byte_buffer_segment_span_range segments() const { return {it, length()}; }

  /// Returns a non-owning list of segments that compose the byte_buffer.
  /// The segments are not const, so that the callee can modify the bytes, but not layout of the buffer.
  byte_buffer_segment_span_range modifiable_segments() { return {it, length()}; }

  /// \brief Equality comparison between byte buffer view and another range.
  template <typename T>
  friend bool operator==(const byte_buffer_view& lhs, const T& r)
  {
    return detail::compare_byte_buffer_range(lhs, r);
  }
  template <typename T, std::enable_if_t<not is_byte_buffer_range<T>::value, int> = 0>
  friend bool operator==(const T& r, const byte_buffer_view& rhs)
  {
    return detail::compare_byte_buffer_range(rhs, r);
  }
  template <typename T>
  friend bool operator!=(const byte_buffer_view& lhs, const T& r)
  {
    return not(lhs == r);
  }
  template <typename T, std::enable_if_t<not is_byte_buffer_range<T>::value, int> = 0>
  friend bool operator!=(const T& r, const byte_buffer_view& rhs)
  {
    return not(rhs == r);
  }

protected:
  iterator it;
  iterator it_end;
};

} // namespace srsran

namespace fmt {

/// \brief Custom formatter for byte_buffer_view.
template <>
struct formatter<srsran::byte_buffer_view> {
  enum { hexadecimal, binary } mode = hexadecimal;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    auto it = ctx.begin();
    while (it != ctx.end() and *it != '}') {
      if (*it == 'b') {
        mode = binary;
      }
      ++it;
    }
    return it;
  }

  template <typename FormatContext>
  auto format(const srsran::byte_buffer_view& buf, FormatContext& ctx) const
  {
    if (mode == hexadecimal) {
      return format_to(ctx.out(), "{:0>2x}", fmt::join(buf.begin(), buf.end(), " "));
    }
    return format_to(ctx.out(), "{:0>8b}", fmt::join(buf.begin(), buf.end(), " "));
  }
};

} // namespace fmt
