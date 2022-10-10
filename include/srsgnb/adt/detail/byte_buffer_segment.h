/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/span.h"
#include "srsgnb/support/srsgnb_assert.h"
#include <array>
#include <cstdint>

namespace srsgnb {

/// \brief Memory segment of fixed size specified by SEGMENT_SIZE.
/// Each segment buffer is divided into three parts [ HEADROOM | PAYLOAD | TAILROOM ]
/// A segment also contains a header region that is used to create an intrusive linked list.
/// Bytes can be added in the HEADROOM region via prepend() or in the TAILROOM via append()
class byte_buffer_segment
{
public:
  constexpr static size_t SEGMENT_SIZE     = 256;
  constexpr static size_t DEFAULT_HEADROOM = 16;

  using value_type     = uint8_t;
  using iterator       = uint8_t*;
  using const_iterator = const uint8_t*;

  /// Segment header where metadata gets stored.
  struct metadata_storage {
    std::shared_ptr<byte_buffer_segment> next = nullptr;
  };

  byte_buffer_segment(size_t headroom = DEFAULT_HEADROOM)
  {
    // Members initialized here instead of the initialization list to avoid
    // GCC 12.1.0 to warn about uninitialized use of buffer [-Werror=uninitialized]
    payload_data_     = buffer.data() + headroom;
    payload_data_end_ = buffer.data() + headroom;
  }
  byte_buffer_segment(const byte_buffer_segment& other) noexcept
  {
    // Members initialized here instead of the initialization list to avoid
    // GCC 12.1.0 to warn about uninitialized use of buffer [-Werror=uninitialized]
    payload_data_     = buffer.data() + other.headroom();
    payload_data_end_ = buffer.data() + other.tailroom_start();
    std::copy(other.begin(), other.end(), begin());
  }
  byte_buffer_segment(byte_buffer_segment&& other) noexcept
  {
    // Members initialized here instead of the initialization list to avoid
    // GCC 12.1.0 to warn about uninitialized use of buffer [-Werror=uninitialized]
    payload_data_     = buffer.data() + other.headroom();
    payload_data_end_ = buffer.data() + other.tailroom_start();
    std::copy(other.begin(), other.end(), begin());
  }
  byte_buffer_segment& operator=(const byte_buffer_segment& other) noexcept
  {
    if (this != &other) {
      payload_data_     = buffer.data() + other.headroom();
      payload_data_end_ = buffer.data() + other.tailroom_start();
      std::copy(other.begin(), other.end(), begin());
    }
    return *this;
  }
  byte_buffer_segment& operator=(byte_buffer_segment&& other) noexcept
  {
    payload_data_     = buffer.data() + other.headroom();
    payload_data_end_ = buffer.data() + other.tailroom_start();
    std::copy(other.begin(), other.end(), begin());
    return *this;
  }

  static size_t capacity() { return SEGMENT_SIZE; }

  /// Checks whether segment has no payload.
  bool empty() const { return begin() == end(); }

  /// Returns how much space in bytes there is at the head of the segment.
  size_t headroom() const { return data() - buffer.data(); }

  /// Returns size in bytes of segment.
  size_t length() const { return end() - begin(); }

  /// Returns how much space in bytes there is at the tail of the segment.
  size_t tailroom() const { return buffer.end() - end(); }

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
    srsgnb_sanity_check((size_t)(it_end - it_begin) <= tailroom(), "There is not enough tailroom for append.");
    payload_data_end_ = std::copy(it_begin, it_end, end());
  }

  /// Appends single byte at the tail of the segment.
  void append(uint8_t byte)
  {
    srsgnb_assert(tailroom() >= 1, "There is not enough tailroom space.");
    buffer[tailroom_start()] = byte;
    payload_data_end_++;
  }

  /// Prepends segment with provided span of bytes.
  void prepend(span<const uint8_t> bytes)
  {
    srsgnb_assert(headroom() >= bytes.size(), "There is not enough headroom space.");
    payload_data_ -= bytes.size();
    std::copy(bytes.begin(), bytes.end(), begin());
  }

  /// Reserves headroom space in segment.
  /// \param nof_bytes Number of bytes to reserve.
  void reserve_prepend(size_t nof_bytes)
  {
    srsgnb_assert(headroom() >= nof_bytes, "There is not enough headroom space.");
    payload_data_ -= nof_bytes;
  }

  /// Removes "nof_bytes" from the head of the segment.
  void trim_head(size_t nof_bytes)
  {
    srsgnb_assert(nof_bytes <= length(), "There is not enough headroom space.");
    payload_data_ += nof_bytes;
  }

  /// Removes "nof_bytes" from the tail of the segment.
  void trim_tail(size_t nof_bytes)
  {
    srsgnb_assert(nof_bytes <= length(), "There is not enough headroom space.");
    payload_data_end_ -= nof_bytes;
  }

  /// Resizes payload of segment.
  void resize(size_t nof_bytes)
  {
    srsgnb_assert(nof_bytes <= capacity() - headroom(), "There is not enough space for provided size");
    payload_data_end_ = payload_data_ + nof_bytes;
  }

  uint8_t& operator[](size_t idx)
  {
    srsgnb_sanity_check(idx < length(), "Out-of-bound access");
    return *(begin() + idx);
  }
  const uint8_t& operator[](size_t idx) const
  {
    srsgnb_sanity_check(idx < length(), "Out-of-bound access");
    return *(begin() + idx);
  }

  const uint8_t* data() const { return begin(); }
  uint8_t*       data() { return begin(); }

  uint8_t& back()
  {
    srsgnb_sanity_check(not empty(), "back() called for empty segment.");
    return *(payload_data_end_ - 1);
  }
  const uint8_t& back() const
  {
    srsgnb_sanity_check(not empty(), "back() called for empty segment.");
    return *(payload_data_end_ - 1);
  }

  iterator       begin() { return payload_data_; }
  iterator       end() { return payload_data_end_; }
  const_iterator begin() const { return payload_data_; }
  const_iterator end() const { return payload_data_end_; }

  metadata_storage&       metadata() { return metadata_; }
  const metadata_storage& metadata() const { return metadata_; }

  byte_buffer_segment*       next() { return metadata().next.get(); }
  const byte_buffer_segment* next() const { return metadata().next.get(); }

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
  size_t tailroom_start() const { return headroom() + length(); }

  metadata_storage                  metadata_;
  std::array<uint8_t, SEGMENT_SIZE> buffer;
  uint8_t*                          payload_data_;
  uint8_t*                          payload_data_end_;
};

namespace detail {

/// This class represents an iterator of a linked list of byte_buffer_segments.
template <typename SegmentType>
class byte_buffer_segment_list_iterator_impl
{
  using byte_type = std::conditional_t<std::is_const<SegmentType>::value, const uint8_t, uint8_t>;

public:
  using iterator_type     = byte_buffer_segment_list_iterator_impl<SegmentType>;
  using value_type        = span<byte_type>;
  using reference         = span<byte_type>;
  using difference_type   = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;
  struct pointer {
    pointer(span<byte_type> ar_) : ar(ar_) {}
    span<byte_type>*       operator->() { return &ar; }
    const span<byte_type>* operator->() const { return &ar; }

  private:
    span<byte_type> ar;
  };

  byte_buffer_segment_list_iterator_impl(SegmentType* seg, unsigned offset_, unsigned size_) :
    current_segment(seg), offset(offset_), rem_bytes(size_)
  {
  }

  reference operator*()
  {
    return reference{current_segment->data() + offset, std::min(rem_bytes, (unsigned)current_segment->length())};
  }
  reference operator*() const
  {
    return reference{current_segment->data() + offset, std::min(rem_bytes, (unsigned)current_segment->length())};
  }
  pointer operator->() { return pointer{this->operator*()}; }
  pointer operator->() const { return pointer{this->operator*()}; }

  iterator_type& operator++()
  {
    srsgnb_assert(current_segment != nullptr, "operator++ called after end()");
    offset = 0;
    rem_bytes -= std::min(rem_bytes, (unsigned)current_segment->length());
    if (rem_bytes == 0) {
      current_segment = nullptr;
    } else {
      current_segment = current_segment->next();
    }
    return *this;
  }
  iterator_type operator++(int)
  {
    iterator_type tmp(*this);
    ++(*this);
    return tmp;
  }

  bool operator==(const iterator_type& other) const { return current_segment == other.current_segment; }
  bool operator!=(const iterator_type& other) const { return !(*this == other); }

private:
  SegmentType* current_segment = nullptr;
  unsigned     offset          = 0;
  unsigned     rem_bytes       = 0;
};

template <typename SegmentType>
class byte_buffer_segment_range_impl
{
public:
  using iterator = byte_buffer_segment_list_iterator_impl<SegmentType>;

  byte_buffer_segment_range_impl(SegmentType* seg, unsigned offset, unsigned size) : begin_(seg, offset, size) {}

  byte_buffer_segment_list_iterator_impl<SegmentType>       begin() { return begin_; }
  byte_buffer_segment_list_iterator_impl<const SegmentType> begin() const { return begin_; }
  byte_buffer_segment_list_iterator_impl<SegmentType>       end()
  {
    return byte_buffer_segment_list_iterator_impl<SegmentType>(nullptr, 0, 0);
  }
  byte_buffer_segment_list_iterator_impl<const SegmentType> end() const
  {
    return byte_buffer_segment_list_iterator_impl<const SegmentType>(nullptr, 0, 0);
  }

  bool empty() const { return begin() == end(); }

private:
  byte_buffer_segment_list_iterator_impl<SegmentType> begin_;
};

} // namespace detail

/// Range of byte_buffer_segments.
using byte_buffer_segment_range       = detail::byte_buffer_segment_range_impl<byte_buffer_segment>;
using const_byte_buffer_segment_range = detail::byte_buffer_segment_range_impl<const byte_buffer_segment>;

} // namespace srsgnb