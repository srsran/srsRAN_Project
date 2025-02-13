/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/support/error_handling.h"

namespace srsran {

/// \brief Buffer that holds SIBs in packed form.
///
/// Different versions of the same SIB can be stored in the buffer in the form of segments. This is useful for SIBs
/// which have its content split across different segments that are broadcast in sequence, such as SIBs 7 and 8.
template <typename T = byte_buffer>
class segmented_sib_buffer
{
public:
  /// Default constructor.
  segmented_sib_buffer() noexcept : buffers(0), current_segment(0) {}

  /// Constructor that takes ownership of a buffer holding a single SIB segment.
  explicit segmented_sib_buffer(T&& buffer) noexcept(std::is_nothrow_move_constructible<T>::value) : current_segment(0)
  {
    buffers.emplace_back(std::move(buffer));
  }

  /// Constructor that takes ownership of a vector holding the SIB segments.
  segmented_sib_buffer(const std::vector<T>&& segments) noexcept(std::is_nothrow_move_constructible<T>::value) :
    buffers(std::move(segments)), current_segment(0)
  {
  }

  /// Explicit copy constructor.
  explicit segmented_sib_buffer(const segmented_sib_buffer<T>& other) noexcept(
      std::is_nothrow_copy_constructible<T>::value) = default;

  /// Move constructor.
  segmented_sib_buffer(segmented_sib_buffer<T>&& other) noexcept(std::is_nothrow_move_constructible<T>::value) =
      default;

  /// \brief Gets the contents of the SIB.
  /// \return A reference to the SIB buffer holding the current segment.
  const T& get() const
  {
    srsran_assert(current_segment < buffers.size(), "SIB segment reading index overflow.");
    return buffers[current_segment];
  }

  /// \brief Gets the contents of an SIB segment.
  /// \param[in] i_segment Segment index.
  /// \return A read-only reference to the SIB segment buffer.
  /// \remark An assertion is thrown if the SIB is not segmented, or if the segment index exceeds the available
  /// segments.
  const T& get_segment(unsigned i_segment) const
  {
    srsran_assert(is_segmented(), "SIB message is not segmented.");
    srsran_assert(i_segment < buffers.size(), "SIB segment reading index overflow.");
    return buffers[i_segment];
  }

  /// \brief Gets the contents of an SIB segment.
  /// \param[in] i_segment Segment index.
  /// \return A reference to the SIB segment buffer.
  /// \remark An assertion is thrown if the SIB is not segmented, or if the segment index exceeds the available
  /// segments.
  T& get_segment(unsigned i_segment)
  {
    srsran_assert(is_segmented(), "SIB message is not segmented.");
    srsran_assert(i_segment < buffers.size(), "SIB segment reading index overflow.");
    return buffers[i_segment];
  }

  /// \brief Advances the current segment, available by calling \ref get().
  /// \remark An assertion is thrown if the SIB is not segmented.
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
  /// \remark An assertion is thrown if the SIB is not segmented, or if the segment index exceeds the available
  /// segments.
  size_t get_segment_length(unsigned i_segment) const
  {
    srsran_assert(is_segmented(), "SIB message is not segmented.");
    srsran_assert(i_segment < buffers.size(), "SIB segment reading index overflow.");
    return buffers[i_segment].length();
  }

  /// \brief Appends a new SIB segment to the buffer.
  /// \param[in] segment A buffer holding the SIB segment. Ownership is transferred to the \c segmented_sib_buffer.
  void append_segment(T&& segment) { buffers.emplace_back(std::move(segment)); }

  /// Returns the number of currently stored SIB segments.
  unsigned get_nof_segments() const { return buffers.size(); }

  /// Performs a shallow copy.
  segmented_sib_buffer<T> copy() const { return segmented_sib_buffer<T>{*this}; }

  /// Copy assignment operator. Performs a shallow copy.
  segmented_sib_buffer<T>& operator=(const segmented_sib_buffer<T>& other)
  {
    if (this != &other) {
      *this = other.copy();
    }
    return *this;
  }

  /// Move assignment operator.
  segmented_sib_buffer<T>& operator=(segmented_sib_buffer<T>&& other) = default;

  /// Equality operator
  bool operator==(const segmented_sib_buffer<T>& other) const
  {
    return (current_segment == other.current_segment) && (buffers == other.buffers);
  }

  bool operator!=(const segmented_sib_buffer<T>& other) const { return !(operator==(other)); }

private:
  /// Returns \c true if the SIB has more than one segment, \c false otherwise.
  bool is_segmented() const { return buffers.size() > 1; }

  /// SIB buffers. Each buffer stores an SIB segment.
  std::vector<T> buffers;

  /// Segment reading index.
  unsigned current_segment;
};

} // namespace srsran
