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
#include "srsgnb/srslog/bundled/fmt/format.h"
#include "srsgnb/support/srsgnb_assert.h"
#include <array>
#include <cstdint>
#include <vector>

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

class byte_buffer_view;
class byte_buffer_slice;

/// \brief Byte sequence, which represents its data in memory via an intrusive linked list of memory chunks.
/// This container is not contiguous in memory.
/// Default copy ctor and assignment is disabled in this container. The user should instead std::move to transfer
/// ownership, .copy() for shallow copies with shared ownership and .deep_copy() for byte-wise copies.
class byte_buffer
{
  template <typename T>
  struct iterator_impl {
    using iterator_type     = iterator_impl<T>;
    using value_type        = std::remove_const_t<T>;
    using reference         = T&;
    using pointer           = T*;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    explicit iterator_impl(byte_buffer_segment* start_segment = nullptr, size_t offset_ = 0) :
      current_segment(start_segment), offset(offset_)
    {
    }
    template <typename U, std::enable_if_t<not std::is_same<U, T>::value, bool> = true>
    iterator_impl(const iterator_impl<U>& other) : current_segment(other.current_segment), offset(other.offset)
    {
    }

    reference operator*() { return *(current_segment->data() + offset); }
    reference operator*() const { return *(current_segment->data() + offset); }
    pointer   operator->() { return (current_segment->data() + offset); }
    pointer   operator->() const { return (current_segment->data() + offset); }

    iterator_impl& operator++()
    {
      offset++;
      if (offset >= current_segment->length()) {
        offset          = 0;
        current_segment = current_segment->next();
      }
      return *this;
    }
    iterator_impl operator++(int)
    {
      iterator_impl tmp(*this);
      ++(*this);
      return tmp;
    }
    iterator_impl operator+(unsigned n) const
    {
      iterator_impl tmp(*this);
      tmp += n;
      return tmp;
    }
    iterator_impl& operator+=(unsigned n)
    {
      offset += n;
      while (current_segment != nullptr and offset >= current_segment->length()) {
        offset -= current_segment->length();
        current_segment = current_segment->next();
      }
      srsgnb_sanity_check(current_segment != nullptr or offset == 0, "Out-of-bounds Access");
      return *this;
    }

    difference_type operator-(const iterator_impl<T>& other) const
    {
      difference_type      diff = 0;
      byte_buffer_segment* seg  = other.current_segment;
      for (; seg != current_segment; seg = seg->next()) {
        diff += seg->length();
      }
      diff += offset - other.offset;
      return diff;
    }

    bool operator==(const iterator_impl<T>& other) const
    {
      return current_segment == other.current_segment and offset == other.offset;
    }
    bool operator!=(const iterator_impl<T>& other) const { return !(*this == other); }

  private:
    friend class byte_buffer_view;
    template <typename OtherT>
    friend struct iterator_impl;

    byte_buffer_segment* current_segment;
    size_t               offset;
  };

public:
  using value_type     = uint8_t;
  using iterator       = iterator_impl<uint8_t>;
  using const_iterator = iterator_impl<const uint8_t>;

  byte_buffer() = default;
  /// Explicit copy ctor. User should use copy() method for copy assignments.
  explicit byte_buffer(const byte_buffer&) noexcept = default;
  byte_buffer(span<const uint8_t> bytes) { append(bytes); }
  byte_buffer(std::initializer_list<uint8_t> lst) : byte_buffer(span<const uint8_t>{lst.begin(), lst.size()}) {}
  template <typename It>
  byte_buffer(It other_begin, It other_end)
  {
    // TODO: optimize
    for (; other_begin != other_end; ++other_begin) {
      append(*other_begin);
    }
  }
  byte_buffer(byte_buffer&& other) noexcept : head(std::move(other.head)), tail(other.tail), len(other.len)
  {
    other.tail = nullptr;
    other.len  = 0;
  }

  /// copy assignment is disabled. Use std::move, .copy() or .deep_copy() instead.
  byte_buffer& operator=(const byte_buffer&) noexcept = delete;

  /// Move assignment of byte_buffer. It avoids unnecessary reference counting increment.
  byte_buffer& operator=(byte_buffer&& other) noexcept
  {
    head       = std::move(other.head);
    tail       = other.tail;
    len        = other.len;
    other.tail = nullptr;
    other.len  = 0;
    return *this;
  }

  /// Assignment of span of bytes.
  byte_buffer& operator=(span<const uint8_t> bytes) noexcept
  {
    clear();
    append(bytes);
    return *this;
  }

  /// Performs a deep copy (byte by bytes) of this byte_buffer.
  byte_buffer deep_copy() const
  {
    byte_buffer buf;
    for (byte_buffer_segment* seg = head.get(); seg != nullptr; seg = seg->next()) {
      buf.append(span<uint8_t>{seg->data(), seg->length()});
    }
    return buf;
  }

  /// Performs a shallow copy. Head segment reference counter is incremented.
  byte_buffer copy() const { return byte_buffer{*this}; }

  /// Appends bytes to the byte buffer. This function may retrieve new segments from a memory pool.
  void append(span<const uint8_t> bytes)
  {
    if (empty() and not bytes.empty()) {
      append_segment();
    }
    for (size_t count = 0; count < bytes.size();) {
      if (tail->tailroom() == 0) {
        append_segment();
      }
      size_t              to_write = std::min(tail->tailroom(), bytes.size() - count);
      span<const uint8_t> subspan  = bytes.subspan(count, to_write);
      tail->append(subspan);
      count += to_write;
      len += to_write;
    }
  }

  /// Appends an initializer list of bytes.
  void append(const std::initializer_list<uint8_t>& bytes) { append(span<const uint8_t>{bytes.begin(), bytes.size()}); }

  /// Appends bytes from another byte_buffer. This function may allocate new segments.
  void append(const byte_buffer& other)
  {
    if (empty() and not other.empty()) {
      append_segment();
    }
    for (byte_buffer_segment* seg = other.head.get(); seg != nullptr; seg = seg->next()) {
      auto other_it = seg->begin();
      while (other_it != seg->end()) {
        if (tail->tailroom() == 0) {
          append_segment();
        }
        auto to_append = std::min(seg->end() - other_it, (iterator::difference_type)tail->tailroom());
        tail->append(other_it, other_it + to_append);
        other_it += to_append;
      }
      len += seg->length();
    }
  }

  /// Appends bytes to the byte buffer. This function may allocate new segments.
  void append(uint8_t byte)
  {
    if (empty() or tail->tailroom() == 0) {
      append_segment();
    }
    tail->append(byte);
    len++;
  }

  /// Appends a view of bytes into current byte buffer.
  void append(const byte_buffer_view& view);

  /// Appends an owning view of bytes into current byte buffer.
  void append(const byte_buffer_slice& view);

  /// Prepends bytes to byte_buffer. This function may allocate new segments.
  void prepend(span<const uint8_t> bytes)
  {
    if (empty()) {
      prepend_segment();
    }
    for (size_t count = 0; count < bytes.size();) {
      if (head->headroom() == 0) {
        prepend_segment();
      }
      size_t              to_write = std::min(head->headroom(), bytes.size() - count);
      span<const uint8_t> subspan  = bytes.subspan(bytes.size() - to_write - count, to_write);
      head->prepend(subspan);
      count += to_write;
    }
    len += bytes.size();
  }

  /// Prepends space in byte_buffer. This function may allocate new segments.
  /// \param nof_bytes Number of bytes to reserve at header.
  /// \return range of bytes that were reserved.
  byte_buffer_view reserve_prepend(size_t nof_bytes);

  /// Chains two byte buffers. The after byte buffer becomes empty.
  byte_buffer& chain_after(byte_buffer after)
  {
    if (empty()) {
      *this = std::move(after);
    } else {
      tail->metadata().next = std::move(after.head);
      tail                  = after.tail;
      len += after.len;
      after.len  = 0;
      after.tail = nullptr;
    }
    return *this;
  }

  /// Chains two byte buffers. The before byte buffer becomes empty.
  byte_buffer& chain_before(byte_buffer before)
  {
    if (empty()) {
      *this = std::move(before);
    } else {
      before.tail->metadata().next = std::move(head);
      head                         = std::move(before.head);
      len += before.len;
      before.len  = 0;
      before.tail = nullptr;
    }
    return *this;
  }

  /// Clear byte buffer.
  void clear()
  {
    head = nullptr;
    tail = nullptr;
    len  = 0;
  }

  /// Removes "nof_bytes" from the head of the byte_buffer.
  void trim_head(size_t nof_bytes)
  {
    srsgnb_sanity_check(length() >= nof_bytes, "Trying to trim more bytes than those available");
    for (size_t trimmed = 0; trimmed != nof_bytes;) {
      size_t to_trim = std::min(nof_bytes - trimmed, head->length());
      head->trim_head(to_trim);
      trimmed += to_trim;
      if (head->length() == 0) {
        head = std::move(head->metadata().next);
      }
    }
    len -= nof_bytes;
  }

  /// \brief Remove "nof_bytes" bytes at the end of the byte_buffer.
  /// If the length is greater than the length of the last segment, the function will fail and return -1 without
  /// modifying the byte_buffer.
  /// \return 0 if successful, -1 otherwise.
  int trim_tail(size_t nof_bytes)
  {
    if (tail->length() >= nof_bytes) {
      tail->trim_tail(nof_bytes);
      len -= nof_bytes;
      return 0;
    }
    return -1;
  }

  /// Checks whether byte_buffer is empty.
  bool empty() const { return length() == 0; }

  /// Checks byte_buffer length.
  size_t length() const { return len; }

  uint8_t&       back() { return tail->back(); }
  const uint8_t& back() const { return tail->back(); }

  const uint8_t& operator[](size_t i) const { return *(begin() + i); }

  template <typename Container>
  bool operator==(const Container& container) const
  {
    // TODO: Possible optimization to account batches of segments.
    return std::equal(begin(), end(), container.begin(), container.end());
  }
  bool operator!=(const byte_buffer& other) const { return !(*this == other); }

  iterator       begin() { return iterator{head.get(), 0}; }
  const_iterator cbegin() const { return const_iterator{head.get(), 0}; }
  const_iterator begin() const { return const_iterator{head.get(), 0}; }
  iterator       end() { return iterator{nullptr, 0}; }
  const_iterator end() const { return const_iterator{nullptr, 0}; }
  const_iterator cend() const { return const_iterator{nullptr, 0}; }

  /// Test if byte buffer is contiguous in memory, i.e. it has only one segment.
  bool is_contiguous() const { return head.get() == tail; }

  /// Moves the bytes stored in different segments of the byte_buffer into first segment.
  /// \return 0 if the data could fit in one segment. -1 otherwise, and the byte_buffer remains unaltered.
  int linearize()
  {
    if (is_contiguous()) {
      return 0;
    }
    size_t sz = length();
    if (sz > byte_buffer_segment::capacity() - head->headroom()) {
      return -1;
    }
    for (byte_buffer_segment* seg = head->next(); seg != nullptr; seg = seg->next()) {
      head->append(seg->begin(), seg->end());
    }
    head->metadata().next.reset();
    tail = head.get();
    return 0;
  }

  /// Set byte_buffer length. Note: It doesn't initialize newly created bytes.
  void resize(size_t new_sz)
  {
    size_t prev_len = length();
    if (new_sz == prev_len) {
      return;
    }
    if (new_sz > prev_len) {
      for (size_t to_add = new_sz - prev_len; to_add > 0;) {
        if (empty() or tail->tailroom() == 0) {
          append_segment();
        }
        size_t added = std::min(tail->tailroom(), to_add);
        tail->resize(added);
        to_add -= added;
      }
    } else {
      size_t count = 0;
      for (byte_buffer_segment* seg = head.get(); count < new_sz; seg = seg->next()) {
        size_t new_seg_len = std::min(seg->length(), new_sz - count);
        if (new_seg_len != seg->length()) {
          seg->resize(new_seg_len);
        }
        count += new_seg_len;
        if (count == new_sz) {
          seg->metadata().next = nullptr;
          tail                 = seg;
        }
      }
    }
    len = new_sz;
  }

private:
  void append_segment()
  {
    // TODO: Use memory pool.
    // TODO: Verify if allocation was successful. What to do if not?
    if (empty()) {
      // headroom given.
      head = std::make_shared<byte_buffer_segment>((size_t)byte_buffer_segment::DEFAULT_HEADROOM);
      tail = head.get();
    } else {
      // No headroom needed for later segments.
      tail->metadata().next = std::make_shared<byte_buffer_segment>(0);
      tail                  = tail->next();
    }
  }

  void prepend_segment()
  {
    // TODO: Use memory pool.
    // TODO: Verify if allocation was successful. What to do if not?
    auto buf = std::make_shared<byte_buffer_segment>();
    if (empty()) {
      head = buf;
      tail = buf.get();
    } else {
      buf->metadata().next = head;
      head                 = buf;
    }
  }

  // TODO: Optimize. shared_ptr<> has a lot of boilerplate we don't need.
  std::shared_ptr<byte_buffer_segment> head = nullptr;
  byte_buffer_segment*                 tail = nullptr;
  size_t                               len  = 0;
};

inline bool operator==(const byte_buffer& buf, span<const uint8_t> bytes)
{
  size_t i = 0;
  for (uint8_t val : buf) {
    if (i >= bytes.size() or val != bytes[i]) {
      return false;
    }
    i++;
  }
  return true;
}
inline bool operator==(span<const uint8_t> bytes, const byte_buffer& buf)
{
  return buf == bytes;
}
inline bool operator!=(const byte_buffer& buf, span<const uint8_t> bytes)
{
  return !(buf == bytes);
}
inline bool operator!=(span<const uint8_t> bytes, const byte_buffer& buf)
{
  return !(buf == bytes);
}

class byte_buffer_view
{
public:
  using value_type     = uint8_t;
  using iterator       = byte_buffer::iterator;
  using const_iterator = byte_buffer::const_iterator;

  byte_buffer_view() = default;
  byte_buffer_view(iterator it_begin_, iterator it_end_) : it(it_begin_), it_end(it_end_) {}
  byte_buffer_view(const byte_buffer& buffer) : it(buffer.begin()), it_end(buffer.end()) {}
  byte_buffer_view(const byte_buffer& buffer, size_t start, size_t sz) : it(buffer.begin() + start), it_end(it + sz) {}

  iterator       begin() { return it; }
  iterator       end() { return it_end; }
  const_iterator begin() const { return it; }
  const_iterator end() const { return it_end; }

  bool empty() const { return it == it_end; }

  size_t length() const { return it_end - it; }

  const uint8_t& operator[](size_t i) const { return *(it + i); }

  /// Returns another sub-view with dimensions specified in arguments.
  byte_buffer_view view(size_t offset, size_t size) const
  {
    srsgnb_sanity_check(offset + size <= length(), "Invalid view dimensions.");
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

  /// Compare byte_buffer_view with Range.
  template <typename Range>
  bool operator==(const Range& other) const
  {
    return std::equal(begin(), end(), other.begin(), other.end());
  }
  template <typename Range>
  bool operator!=(const Range& other) const
  {
    return not(*this == other);
  }

protected:
  iterator it{nullptr, 0};
  iterator it_end{nullptr, 0};
};

inline void byte_buffer::append(const byte_buffer_view& view)
{
  if (empty() and not view.empty()) {
    append_segment();
  }
  // TODO: Optimize.
  for (auto& b : view) {
    append(b);
  }
}

/// \brief This class represents a sub-interval or make_slice of a potentially larger byte_buffer.
/// Like byte_buffer and byte_buffer_view, the represented bytes by this class are not contiguous in memory.
/// Contrarily to byte_buffer_view, this class retains shared ownership of the segments held by the byte_buffer
/// which it references.
/// Due to the shared ownership model, the usage of this class may involve additional overhead associated with
/// reference counting, which does not take place when using byte_buffer_view.
class byte_buffer_slice
{
public:
  using value_type     = byte_buffer_view::value_type;
  using iterator       = byte_buffer_view::iterator;
  using const_iterator = byte_buffer_view::const_iterator;

  byte_buffer_slice() = default;
  byte_buffer_slice(byte_buffer&& buf_) : sliced_view(buf_.begin(), buf_.end()), buf(std::move(buf_)) {}
  explicit byte_buffer_slice(const byte_buffer& buf_) : sliced_view(buf_.begin(), buf_.end()), buf(buf_.copy()) {}
  byte_buffer_slice(const byte_buffer& buf_, size_t offset, size_t length) :
    sliced_view(buf_, offset, length), buf(buf_.copy())
  {
  }
  byte_buffer_slice(const byte_buffer& buf_, byte_buffer_view view) : sliced_view(view), buf(buf_.copy())
  {
    srsgnb_sanity_check(view.begin() - byte_buffer_view{buf}.begin() < (int)buf.length(),
                        "byte_buffer_view is not part of the owned byte_buffer");
  }

  void clear()
  {
    buf.clear();
    sliced_view = {};
  }

  /// Converts to non-owning byte buffer view.
  byte_buffer_view view() const { return sliced_view; }
  explicit         operator byte_buffer_view() const { return sliced_view; }

  /// Returns another owning sub-view with dimensions specified in arguments.
  byte_buffer_slice make_slice(size_t offset, size_t size) const
  {
    srsgnb_sanity_check(offset + size <= length(), "Invalid view dimensions.");
    return {buf, sliced_view.view(offset, size)};
  }

  /// Advances slice by provided offset. The length of the slice gets automatically reduced by the provided offset.
  byte_buffer_slice& advance(size_t offset)
  {
    sliced_view = byte_buffer_view{sliced_view.begin() + offset, sliced_view.end()};
    return *this;
  }

  bool   empty() const { return sliced_view.empty(); }
  size_t length() const { return sliced_view.length(); }

  const uint8_t& operator[](size_t idx) const { return sliced_view[idx]; }

  iterator       begin() { return sliced_view.begin(); }
  const_iterator begin() const { return sliced_view.begin(); }
  iterator       end() { return sliced_view.end(); }
  const_iterator end() const { return sliced_view.end(); }

  template <typename Range>
  bool operator==(const Range& r) const
  {
    return std::equal(begin(), end(), r.begin(), r.end());
  }

  template <typename Range>
  bool operator!=(const Range& r) const
  {
    return not(*this == r);
  }

private:
  byte_buffer_view sliced_view;
  byte_buffer      buf;
};

inline void byte_buffer::append(const byte_buffer_slice& view)
{
  if (empty() and not view.empty()) {
    append_segment();
  }
  // TODO: Optimize.
  for (auto& b : view) {
    append(b);
  }
}

/// Used to read a range of bytes stored in a byte_buffer.
class byte_buffer_reader : private byte_buffer_view
{
public:
  using byte_buffer_view::begin;
  using byte_buffer_view::byte_buffer_view;
  using byte_buffer_view::empty;
  using byte_buffer_view::end;
  using byte_buffer_view::length;

  byte_buffer_reader(const byte_buffer_view& other) : byte_buffer_view(other) {}

  /// Obtain a range view to the bytes pointed by the reader.
  byte_buffer_view view() const { return {it, it_end}; }

  const uint8_t& operator*() const { return *it; }

  const_iterator operator++() { return ++it; }

  /// Advance reader by offset bytes. Returns an iterator to new position.
  const_iterator operator+=(size_t offset) { return it += offset; }

  /// Advance offset bytes and returns view to skipped bytes.
  byte_buffer_view split_and_advance(size_t offset)
  {
    auto prev_it = it;
    it += offset;
    return {prev_it, it};
  }
};

/// Used to write into a range of bytes stored in a byte_buffer.
class byte_buffer_writer
{
public:
  byte_buffer_writer(byte_buffer& other) : buffer(&other) {}

  /// Obtain a range view to the bytes pointed by the reader.
  byte_buffer_view view() const { return *buffer; }

  /// Appends bytes.
  void append(byte_buffer_view bytes)
  {
    // TODO: do batch append.
    for (uint8_t byte : bytes) {
      buffer->append(byte);
    }
  }

  /// Appends initializer list of bytes.
  void append(const std::initializer_list<uint8_t>& bytes)
  {
    buffer->append(span<const uint8_t>{bytes.begin(), bytes.size()});
  }

  /// Appends span of bytes.
  void append(span<const uint8_t> bytes) { buffer->append(bytes); }

  /// Appends single byte.
  void append(uint8_t byte) { buffer->append(byte); }

  void append_zeros(size_t nof_zeros)
  {
    // TODO: optimize.
    for (size_t i = 0; i < nof_zeros; ++i) {
      buffer->append(0);
    }
  }

  /// Checks last appended byte.
  uint8_t& back() { return buffer->back(); }

  /// Number of bytes in the byte_buffer.
  size_t length() const { return buffer->length(); }

  /// Checks whether any byte has been written.
  bool empty() const { return buffer->empty(); }

private:
  byte_buffer* buffer;
};

inline byte_buffer_view byte_buffer::reserve_prepend(size_t nof_bytes)
{
  if (empty()) {
    prepend_segment();
  }
  size_t count = nof_bytes;
  while (count > 0) {
    if (head->headroom() == 0) {
      prepend_segment();
    }
    size_t to_reserve = std::min(head->headroom(), count);
    head->reserve_prepend(to_reserve);
    count -= to_reserve;
  }
  len += nof_bytes;
  return byte_buffer_view{begin(), begin() + nof_bytes};
}

/// Converts a hex string (e.g. 01FA02) to a byte buffer.
inline byte_buffer make_byte_buffer(std::string hex_str)
{
  srsgnb_assert(hex_str.size() % 2 == 0, "The number of hex digits must be even");
  byte_buffer ret;
  for (size_t i = 0; i < hex_str.size(); i += 2) {
    uint8_t val;
    sscanf(hex_str.data() + i, "%02hhX", &val);
    ret.append(val);
  }
  return ret;
}

} // namespace srsgnb

namespace fmt {

/// \brief Custom formatter for byte_buffer_view.
template <>
struct formatter<srsgnb::byte_buffer_view> {
  enum { hexadecimal, binary } mode = hexadecimal;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
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
  auto format(const srsgnb::byte_buffer_view& buf, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    if (mode == hexadecimal) {
      return format_to(ctx.out(), "{:0>2x}", fmt::join(buf.begin(), buf.end(), " "));
    }
    return format_to(ctx.out(), "{:0>8b}", fmt::join(buf.begin(), buf.end(), " "));
  }
};

/// \brief Custom formatter for byte_buffer.
template <>
struct formatter<srsgnb::byte_buffer> {
  enum { hexadecimal, binary } mode = hexadecimal;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
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
  auto format(const srsgnb::byte_buffer& buf, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    if (mode == hexadecimal) {
      return format_to(ctx.out(), "{:0>2x}", fmt::join(buf.begin(), buf.end(), " "));
    }
    return format_to(ctx.out(), "{:0>8b}", fmt::join(buf.begin(), buf.end(), " "));
  }
};

/// \brief Custom formatter for byte_buffer_slice.
template <>
struct formatter<srsgnb::byte_buffer_slice> : public formatter<srsgnb::byte_buffer_view> {
  template <typename FormatContext>
  auto format(const srsgnb::byte_buffer_slice& buf, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return formatter<srsgnb::byte_buffer_view>::format(buf.view(), ctx);
  }
};

} // namespace fmt
