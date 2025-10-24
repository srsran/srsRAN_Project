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

#include "srsran/adt/detail/byte_buffer_segment_list.h"
#include "srsran/support/has_method.h"

namespace srsran {
namespace detail {

/// \brief Comparison between a byte buffer type and another container.
///
/// Depending on the type of containers passed, different equal algorithms are used.
template <typename LhsContainer, typename RhsContainer>
bool compare_byte_buffer_range(const LhsContainer& lhs, const RhsContainer& rhs)
{
  static_assert(is_byte_buffer_range<LhsContainer>::value or is_byte_buffer_range<RhsContainer>::value,
                "Invalid byte buffer type passed as argument");

  if constexpr (not is_byte_buffer_range<LhsContainer>::value and is_byte_buffer_range<RhsContainer>::value) {
    // In case only rhs is a byte buffer-like type, we reverse the arguments.
    return compare_byte_buffer_range(rhs, lhs);
  }
  // From this point onwards, lhs must be a byte_buffer-like type.

  if constexpr (std::is_convertible_v<RhsContainer, span<const uint8_t>>) {
    // If rhs is a span-like container. Do segment-wise comparison.

    if (lhs.length() != rhs.size()) {
      return false;
    }

    const_byte_buffer_segment_span_range segments = lhs.segments();
    auto                                 other_it = rhs.begin();
    for (span<const uint8_t> seg : segments) {
      if (not std::equal(seg.begin(), seg.end(), other_it, other_it + seg.size())) {
        return false;
      }
      other_it += seg.size();
    }
    srsran_sanity_check(other_it == rhs.end(), "segment list is in invalid state");
    return true;
  }

  if constexpr (is_byte_buffer_range<RhsContainer>::value) {
    // rhs is a byte buffer-like type. We can leverage segment-wise comparison.

    if (lhs.length() != rhs.length()) {
      return false;
    }

    const_byte_buffer_segment_span_range segments1 = lhs.segments();
    const_byte_buffer_segment_span_range segments2 = rhs.segments();
    auto                                 seg_it1   = segments1.begin();
    auto                                 seg_it2   = segments2.begin();

    if (seg_it1->begin() == seg_it2->begin() and seg_it1->end() == seg_it2->end()) {
      // Shortcut in case the two byte buffers point to the same underlying memory.
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

  if constexpr (has_member(RhsContainer, size())) {
    // For containers with a size() method.

    if (lhs.length() != rhs.size()) {
      return false;
    }
  }

  // Fallback comparison with any type of container.
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

} // namespace detail
} // namespace srsran
