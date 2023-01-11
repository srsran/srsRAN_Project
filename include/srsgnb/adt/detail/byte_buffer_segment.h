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
#include "srsgnb/support/memory_pool/fixed_size_memory_block_pool.h"
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
  constexpr static size_t SEGMENT_SIZE     = 1024;
  constexpr static size_t DEFAULT_HEADROOM = 16;

  using value_type     = uint8_t;
  using iterator       = uint8_t*;
  using const_iterator = const uint8_t*;

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
    srsgnb_assert(idx < length(), "Out-of-bound access");
    return *(begin() + idx);
  }
  const uint8_t& operator[](size_t idx) const
  {
    srsgnb_assert(idx < length(), "Out-of-bound access");
    return *(begin() + idx);
  }

  const uint8_t* data() const { return begin(); }
  uint8_t*       data() { return begin(); }

  uint8_t& back()
  {
    srsgnb_assert(not empty(), "back() called for empty segment.");
    return *(payload_data_end_ - 1);
  }
  const uint8_t& back() const
  {
    srsgnb_assert(not empty(), "back() called for empty segment.");
    return *(payload_data_end_ - 1);
  }

  iterator       begin() { return payload_data_; }
  iterator       end() { return payload_data_end_; }
  const_iterator begin() const { return payload_data_; }
  const_iterator end() const { return payload_data_end_; }

  std::unique_ptr<byte_buffer_segment>&       next() { return next_segment; }
  const std::unique_ptr<byte_buffer_segment>& next() const { return next_segment; }

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

  /// Byte_buffer_segments are allocated using a pre-allocated segment memory pool.
  void* operator new(size_t sz, const std::nothrow_t& nothrow_value) noexcept;

  /// Byte_buffer_segments are deallocated back to the global segment memory pool.
  void operator delete(void* p);

private:
  size_t tailroom_start() const { return headroom() + length(); }

  uint8_t*                             payload_data_;
  uint8_t*                             payload_data_end_;
  std::unique_ptr<byte_buffer_segment> next_segment;
  std::array<uint8_t, SEGMENT_SIZE>    buffer;
};

/// Pool of byte buffer segments.
struct byte_buffer_pool_tag {};
using byte_buffer_segment_pool = fixed_size_memory_block_pool<byte_buffer_pool_tag>;

/// Initialize pool of byte buffer segments with specific number of segments. Assert if the pool was already initialized
/// with a different number of segments.
inline void init_byte_buffer_segment_pool(std::size_t nof_segments)
{
  auto& pool = byte_buffer_segment_pool::get_instance(nof_segments, sizeof(byte_buffer_segment));
  srsgnb_assert(nof_segments == pool.nof_memory_blocks(),
                "The pool was already initialized with a different number of segments ({} != {})",
                nof_segments,
                pool.nof_memory_blocks());
}

inline void* byte_buffer_segment::operator new(size_t sz, const std::nothrow_t& /**/) noexcept
{
  // Initialize byte buffer segment pool, if not yet initialized.
  constexpr static size_t default_byte_buffer_segment_pool_size = 8192;
  static auto&            pool =
      byte_buffer_segment_pool::get_instance(default_byte_buffer_segment_pool_size, sizeof(byte_buffer_segment));
  return pool.allocate_node(sz);
}

inline void byte_buffer_segment::operator delete(void* p)
{
  static auto& pool = byte_buffer_segment_pool::get_instance();
  pool.deallocate_node(p);
}

namespace detail {

/// This class represents a byte iterator for a linked list of byte_buffer_segments.
template <typename T>
class byte_buffer_iterator_impl
{
public:
  using iterator_type     = byte_buffer_iterator_impl<T>;
  using value_type        = std::remove_const_t<T>;
  using reference         = T&;
  using pointer           = T*;
  using difference_type   = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;

  explicit byte_buffer_iterator_impl(byte_buffer_segment* start_segment = nullptr, size_t offset_ = 0) :
    current_segment(start_segment), offset(offset_)
  {
  }

  /// Conversion from iterators of T to const T.
  template <typename U, std::enable_if_t<not std::is_same<U, T>::value, bool> = true>
  byte_buffer_iterator_impl(const byte_buffer_iterator_impl<U>& other) :
    current_segment(other.current_segment), offset(other.offset)
  {
  }

  reference operator*() { return *(current_segment->data() + offset); }
  reference operator*() const { return *(current_segment->data() + offset); }
  pointer   operator->() { return (current_segment->data() + offset); }
  pointer   operator->() const { return (current_segment->data() + offset); }

  byte_buffer_iterator_impl& operator++()
  {
    offset++;
    if (offset >= current_segment->length()) {
      offset          = 0;
      current_segment = current_segment->next().get();
    }
    return *this;
  }
  byte_buffer_iterator_impl operator++(int)
  {
    byte_buffer_iterator_impl tmp(*this);
    ++(*this);
    return tmp;
  }
  byte_buffer_iterator_impl operator+(unsigned n) const
  {
    byte_buffer_iterator_impl tmp(*this);
    tmp += n;
    return tmp;
  }
  byte_buffer_iterator_impl& operator+=(unsigned n)
  {
    offset += n;
    while (current_segment != nullptr and offset >= current_segment->length()) {
      offset -= current_segment->length();
      current_segment = current_segment->next().get();
    }
    srsgnb_assert(current_segment != nullptr or offset == 0, "Out-of-bounds Access");
    return *this;
  }

  /// Distance between two iterators. The iterators must point at the same list of segments.
  difference_type operator-(const byte_buffer_iterator_impl<T>& other) const
  {
    difference_type      diff = 0;
    byte_buffer_segment* seg  = other.current_segment;
    for (; seg != current_segment; seg = seg->next().get()) {
      diff += seg->length();
    }
    diff += offset - other.offset;
    return diff;
  }

  bool operator==(const byte_buffer_iterator_impl<T>& other) const
  {
    return current_segment == other.current_segment and offset == other.offset;
  }
  bool operator!=(const byte_buffer_iterator_impl<T>& other) const { return !(*this == other); }

private:
  template <typename OtherT>
  friend class byte_buffer_iterator_impl;
  template <typename U>
  friend class byte_buffer_segment_list_iterator_impl;

  byte_buffer_segment* current_segment = nullptr;
  size_t               offset          = 0;
};

/// This class represents a byte_buffer_segment iterator for a linked list of byte_buffer_segments.
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

  byte_buffer_segment_list_iterator_impl(SegmentType* seg, size_t offset_, size_t size_) :
    current_segment(seg), offset(offset_), rem_bytes(size_)
  {
    srsgnb_assert(current_segment != nullptr or (offset == 0 and rem_bytes == 0),
                  "Positive offset or length for empty segment");
    srsgnb_assert(current_segment == nullptr or offset < current_segment->length(), "Invalid offset");
  }
  template <typename U>
  byte_buffer_segment_list_iterator_impl(const byte_buffer_iterator_impl<U>& it, size_t size_) :
    byte_buffer_segment_list_iterator_impl(it.current_segment, it.offset, size_)
  {
  }
  template <typename OtherT>
  byte_buffer_segment_list_iterator_impl(const byte_buffer_segment_list_iterator_impl<OtherT>& other) :
    byte_buffer_segment_list_iterator_impl(other.current_segment, other.offset, other.rem_bytes)
  {
  }

  reference operator*()
  {
    return reference{current_segment->data() + offset,
                     std::min(rem_bytes, (unsigned)current_segment->length() - offset)};
  }
  reference operator*() const
  {
    return reference{current_segment->data() + offset,
                     std::min(rem_bytes, (unsigned)current_segment->length() - offset)};
  }
  pointer operator->() { return pointer{this->operator*()}; }
  pointer operator->() const { return pointer{this->operator*()}; }

  iterator_type& operator++()
  {
    srsgnb_assert(current_segment != nullptr, "operator++ called after end()");
    rem_bytes -= std::min(rem_bytes, (unsigned)current_segment->length() - offset);
    offset = 0;
    if (rem_bytes == 0) {
      current_segment = nullptr;
    } else {
      current_segment = current_segment->next().get();
    }
    return *this;
  }
  iterator_type operator++(int)
  {
    iterator_type tmp(*this);
    ++(*this);
    return tmp;
  }

  bool operator==(const iterator_type& other) const
  {
    return current_segment == other.current_segment and offset == other.offset;
  }
  bool operator!=(const iterator_type& other) const { return !(*this == other); }

private:
  template <typename OtherT>
  friend class byte_buffer_segment_list_iterator_impl;

  SegmentType* current_segment = nullptr;
  unsigned     offset          = 0;
  unsigned     rem_bytes       = 0;
};

/// This class represents a range (a begin and end pair) of byte_buffer_segment_list iterators.
template <typename SegmentType>
class byte_buffer_segment_range_impl
{
public:
  using iterator = byte_buffer_segment_list_iterator_impl<SegmentType>;

  byte_buffer_segment_range_impl(SegmentType* seg, size_t offset, size_t size) : begin_(seg, offset, size) {}
  template <typename U>
  byte_buffer_segment_range_impl(const byte_buffer_iterator_impl<U>& it, size_t size) : begin_(it, size)
  {
  }

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

/// \brief Checks whether a type represents a range of byte_buffer_iterators (e.g. byte_buffer, byte_buffer_slice,
/// byte_buffer_view).
template <typename ByteBufferType>
using is_byte_buffer_range =
    std::is_same<typename ByteBufferType::iterator, detail::byte_buffer_iterator_impl<uint8_t>>;

namespace detail {

/// \brief Compare byte buffer range and another container that is not a span or a byte_buffer type.
/// The comparison is done byte by byte.
template <
    typename ByteBufferType,
    typename U,
    std::enable_if_t<not std::is_convertible<U, span<const uint8_t>>::value and not is_byte_buffer_range<U>::value,
                     int> = 0>
bool compare_byte_buffer_range(const ByteBufferType& t, const U& u)
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

  const_byte_buffer_segment_range segments = buffer.segments();
  auto                            other_it = container.begin();
  for (span<const uint8_t> seg : segments) {
    if (not std::equal(seg.begin(), seg.end(), other_it, other_it + seg.size())) {
      return false;
    }
    other_it += seg.size();
  }
  srsgnb_sanity_check(other_it == container.end(), "segment list is in invalid state");
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
  const_byte_buffer_segment_range segments1 = lhs.segments();
  const_byte_buffer_segment_range segments2 = rhs.segments();
  auto                            seg_it1   = segments1.begin();
  auto                            seg_it2   = segments2.begin();

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
  srsgnb_sanity_check(seg_it1 == segments1.end() and seg_it2 == segments2.end(), "byte buffers are in invalid state");
  return true;
}

struct byte_buffer_segment_pool_tag {};

} // namespace detail

} // namespace srsgnb
