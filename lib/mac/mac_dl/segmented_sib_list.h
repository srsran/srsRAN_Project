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

#include "srsran/support/srsran_assert.h"
#include <vector>

namespace srsran {

/// \brief Class that holds multiple SIB messages in packed form.
///
/// Different versions of the same SIB can be stored in the form of segments. This is useful for SIBs  which have its
/// content split across different segments that are broadcast in sequence, such as SIBs 7 and 8.
template <typename T>
class segmented_sib_list
{
public:
  /// Default constructor.
  segmented_sib_list() noexcept : buffers(0), current_segment(0) {}

  /// Constructor that takes ownership of a buffer holding a single SIB segment.
  explicit segmented_sib_list(T&& buffer) noexcept(std::is_nothrow_move_constructible_v<T>) : current_segment(0)
  {
    buffers.emplace_back(std::move(buffer));
  }

  /// Explicit copy constructor.
  explicit segmented_sib_list(const segmented_sib_list<T>& other) noexcept(
      std::is_nothrow_copy_constructible<T>::value) = default;

  /// Move constructor.
  segmented_sib_list(segmented_sib_list<T>&& other) noexcept(std::is_nothrow_move_constructible<T>::value) = default;

  /// \brief Gets the SIB message holding the current SI information segment.
  /// \return A reference to the SIB buffer holding the current segment.
  const T& get_current_segment() const
  {
    srsran_assert(current_segment < buffers.size(), "SIB segment reading index overflow.");
    return buffers[current_segment];
  }

  /// \brief Gets the contents of an SIB segment.
  /// \param[in] i_segment Segment index.
  /// \return A read-only reference to the SIB segment buffer.
  /// \remark An assertion is thrown if the segment index exceeds the available segments.
  const T& get_segment(unsigned i_segment) const
  {
    srsran_assert(i_segment < buffers.size(), "SIB segment reading index overflow.");
    return buffers[i_segment];
  }

  /// \brief Gets the contents of an SIB segment.
  /// \param[in] i_segment Segment index.
  /// \return A reference to the SIB segment buffer.
  /// \remark An assertion is thrown if the segment index exceeds the available segments.
  T& get_segment(unsigned i_segment)
  {
    srsran_assert(i_segment < buffers.size(), "SIB segment reading index overflow.");
    return buffers[i_segment];
  }

  /// \brief Advances the current segment, available by calling \ref get_current_segment().
  /// \remark An assertion is thrown if the list does not hold multiple segments.
  void advance_current_segment()
  {
    srsran_assert(is_segmented(), "SIB message is not segmented.");

    ++current_segment;
    if (current_segment == get_nof_segments()) {
      current_segment = 0;
    }
  }

  /// \brief Gets the SIB segment length.
  /// \param[in] i_segment Segment index.
  /// \return The SIB segment length
  /// \remark An assertion is thrown if the segment index exceeds the available segments.
  size_t get_segment_length(unsigned i_segment) const
  {
    srsran_assert(i_segment < buffers.size(), "SIB segment reading index overflow.");
    return buffers[i_segment].length();
  }

  /// \brief Appends a new SIB segment to the buffer.
  /// \param[in] segment A buffer holding the SIB segment. Ownership is transferred to the \c segmented_sib_buffer.
  void append_segment(T&& segment) { buffers.emplace_back(std::move(segment)); }

  /// Returns the number of currently stored SIB segments.
  unsigned get_nof_segments() const { return buffers.size(); }

  /// Copy assignment operator. Performs a shallow copy.
  segmented_sib_list<T>& operator=(const segmented_sib_list<T>& other) = default;

  /// Move assignment operator.
  segmented_sib_list<T>& operator=(segmented_sib_list<T>&& other) = default;

  /// Equality operator
  bool operator==(const segmented_sib_list<T>& other) const
  {
    return (current_segment == other.current_segment) && (buffers == other.buffers);
  }

  bool operator!=(const segmented_sib_list<T>& other) const { return !(operator==(other)); }

private:
  /// Returns \c true if the SIB has more than one segment, \c false otherwise.
  bool is_segmented() const { return buffers.size() > 1; }

  /// SIB buffers. Each buffer stores an SIB segment.
  std::vector<T> buffers;

  /// Segment reading index.
  unsigned current_segment;
};

} // namespace srsran
