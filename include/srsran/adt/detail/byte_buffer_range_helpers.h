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

#include "srsran/adt/detail/byte_buffer_segment_list.h"

namespace srsran {
namespace detail {

/// \brief Byte-wise comparison between a byte buffer-like range and another container that is not a span or a
/// byte buffer-like type.
template <typename ByteBufferType,
          typename Container,
          std::enable_if_t<not std::is_convertible<Container, span<const uint8_t>>::value and
                               not is_byte_buffer_range<Container>::value,
                           int> = 0>
bool compare_byte_buffer_range(const ByteBufferType& t, const Container& u)
{
  static_assert(is_byte_buffer_range<ByteBufferType>::value, "Invalid byte buffer type passed as argument");
  return std::equal(t.begin(), t.end(), u.begin(), u.end());
}

/// \brief Comparison between a byte buffer type and a span of bytes.
/// For optimization purposes, the comparison is done segment by segment rather than byte by byte.
template <typename ByteBufferType,
          typename U,
          std::enable_if_t<std::is_convertible<U, span<const uint8_t>>::value, int> = 0>
bool compare_byte_buffer_range(const ByteBufferType& buffer, const U& container)
{
  static_assert(is_byte_buffer_range<ByteBufferType>::value, "Invalid byte buffer type passed as argument");

  if (buffer.length() != container.size()) {
    return false;
  }

  const_byte_buffer_segment_span_range segments = buffer.segments();
  auto                                 other_it = container.begin();
  for (span<const uint8_t> seg : segments) {
    if (not std::equal(seg.begin(), seg.end(), other_it, other_it + seg.size())) {
      return false;
    }
    other_it += seg.size();
  }
  srsran_sanity_check(other_it == container.end(), "segment list is in invalid state");
  return true;
}

/// \brief Comparison between two byte buffer types.
/// For optimization purposes, the comparison is done segment by segment rather than byte by byte.
template <typename ByteBufferType1,
          typename ByteBufferType2,
          std::enable_if_t<is_byte_buffer_range<ByteBufferType2>::value, int> = 0>
bool compare_byte_buffer_range(const ByteBufferType1& lhs, const ByteBufferType2& rhs)
{
  static_assert(is_byte_buffer_range<ByteBufferType1>::value, "Invalid byte buffer type passed as argument");
  if (lhs.length() != rhs.length()) {
    return false;
  }

  const_byte_buffer_segment_span_range segments1 = lhs.segments();
  const_byte_buffer_segment_span_range segments2 = rhs.segments();
  auto                                 seg_it1   = segments1.begin();
  auto                                 seg_it2   = segments2.begin();

  if (seg_it1->begin() == seg_it2->begin() and seg_it1->end() == seg_it2->end()) {
    // shortcut in case the two byte buffers point to the same underlying memory.
    return true;
  }
  unsigned offset1 = 0;
  unsigned offset2 = 0;
  while (seg_it1 != segments1.end() and seg_it2 != segments2.end()) {
    unsigned to_cmp = std::min(seg_it1->size() - offset1, seg_it2->size() - offset2);
    if (not std::equal(seg_it1->begin() + offset1, seg_it1->begin() + offset1 + to_cmp, seg_it2->begin() + offset2)) {
      return false;
    }
    offset1 += to_cmp;
    offset2 += to_cmp;
    if (offset1 == seg_it1->size()) {
      ++seg_it1;
      offset1 = 0;
    }
    if (offset2 == seg_it2->size()) {
      ++seg_it2;
      offset2 = 0;
    }
  }
  srsran_sanity_check(seg_it1 == segments1.end() and seg_it2 == segments2.end(), "byte buffers are in invalid state");
  return true;
}

} // namespace detail
} // namespace srsran
