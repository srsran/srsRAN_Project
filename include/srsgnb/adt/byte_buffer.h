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

#include "detail/byte_buffer_segment.h"
#include "fmt/format.h"
#include <vector>

namespace srsgnb {

/// \brief Non-owning view to a byte sequence.
/// The underlying byte sequence is not contiguous in memory. Instead, it is represented as an intrusive linked list of
/// memory chunks, aka byte buffer segments.
class byte_buffer_view
{
  /// Checks whether type is a byte_buffer/byte_buffer_slice.
  template <typename ByteBufferType>
  using is_owning_byte_buffer_type =
      std::conditional_t<std::is_same<std::decay_t<ByteBufferType>, byte_buffer_view>::value,
                         std::false_type,
                         is_byte_buffer_range<ByteBufferType>>;

public:
  using value_type     = uint8_t;
  using iterator       = detail::byte_buffer_iterator_impl<uint8_t>;
  using const_iterator = detail::byte_buffer_iterator_impl<const uint8_t>;

  byte_buffer_view() = default;

  /// Conversion from a pair of iterators.
  byte_buffer_view(iterator it_begin_, iterator it_end_) : it(it_begin_), it_end(it_end_) {}

  /// Conversion from byte_buffer/byte_buffer_slice.
  template <typename ByteBufferType, std::enable_if_t<is_owning_byte_buffer_type<ByteBufferType>::value, int> = 0>
  byte_buffer_view(const ByteBufferType& buffer) : it(buffer.begin()), it_end(buffer.end())
  {
  }
  template <typename ByteBufferType, std::enable_if_t<is_owning_byte_buffer_type<ByteBufferType>::value, int> = 0>
  byte_buffer_view(const ByteBufferType& buffer, size_t start, size_t sz) : it(buffer.begin() + start), it_end(it + sz)
  {
  }

  iterator       begin() { return it; }
  iterator       end() { return it_end; }
  const_iterator begin() const { return it; }
  const_iterator end() const { return it_end; }

  bool empty() const { return it == it_end; }

  size_t length() const { return it_end - it; }

  const uint8_t& operator[](size_t i) const { return *(it + i); }
  uint8_t&       operator[](size_t i) { return *(it + i); }

  /// Returns another sub-view with dimensions specified in arguments.
  byte_buffer_view view(size_t offset, size_t size) const
  {
    srsgnb_assert(offset + size <= length(), "Invalid view dimensions.");
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
  const_byte_buffer_segment_range segments() const { return {it, length()}; }

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
  iterator it{nullptr, 0};
  iterator it_end{nullptr, 0};
};

class byte_buffer_slice;

/// \brief Byte sequence, which represents its data in memory via an intrusive linked list of memory chunks.
/// This container is not contiguous in memory.
/// Default copy ctor and assignment is disabled in this container. The user should instead std::move to transfer
/// ownership, .copy() for shallow copies with shared ownership and .deep_copy() for byte-wise copies.
class byte_buffer
{
  /// Control block of byte_buffer used to store the head and tail of the segment linked list and the number of total
  /// bytes of the buffer.
  struct control_block {
    /// First segment of the linked list of segments.
    std::unique_ptr<byte_buffer_segment> head;
    /// Tail of linked list of segments. This value is != nullptr if the list is not empty.
    byte_buffer_segment* tail = nullptr;
    /// Length in bytes of the linked list of segments.
    size_t pkt_len = 0;
  };

public:
  using value_type     = uint8_t;
  using iterator       = detail::byte_buffer_iterator_impl<uint8_t>;
  using const_iterator = detail::byte_buffer_iterator_impl<const uint8_t>;

  byte_buffer() = default;
  /// Explicit copy ctor. User should use copy() method for copy assignments.
  explicit byte_buffer(const byte_buffer&) noexcept = default;
  byte_buffer(span<const uint8_t> bytes) { append(bytes); }
  byte_buffer(std::initializer_list<uint8_t> lst) : byte_buffer(span<const uint8_t>{lst.begin(), lst.size()}) {}
  template <typename It>
  byte_buffer(It other_begin, It other_end)
  {
    append(other_begin, other_end);
  }
  byte_buffer(byte_buffer&& other) noexcept = default;

  /// copy assignment is disabled. Use std::move, .copy() or .deep_copy() instead.
  byte_buffer& operator=(const byte_buffer&) noexcept = delete;

  /// Move assignment of byte_buffer. It avoids unnecessary reference counting increment.
  byte_buffer& operator=(byte_buffer&& other) noexcept = default;

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
    if (ctrl_blk_ptr != nullptr) {
      for (byte_buffer_segment* seg = ctrl_blk_ptr->head.get(); seg != nullptr; seg = seg->next().get()) {
        buf.append(span<uint8_t>{seg->data(), seg->length()});
      }
    }
    return buf;
  }

  /// Performs a shallow copy. Head segment reference counter is incremented.
  byte_buffer copy() const { return byte_buffer{*this}; }

  /// Append bytes of a iterator range.
  template <typename Iterator>
  void append(Iterator begin, Iterator end)
  {
    static_assert(std::is_same<std::decay_t<decltype(*begin)>, uint8_t>::value or
                      std::is_same<std::decay_t<decltype(*begin)>, const uint8_t>::value,
                  "Iterator value type is not uint8_t");
    for (auto it = begin; it != end; ++it) {
      append(*it);
    }
  }

  /// Appends bytes to the byte buffer. This function may retrieve new segments from a memory pool.
  bool append(span<const uint8_t> bytes)
  {
    if (empty() and not bytes.empty()) {
      if (not append_segment()) {
        return false;
      }
    }
    // segment-wise copy.
    for (size_t count = 0; count < bytes.size();) {
      if (ctrl_blk_ptr->tail->tailroom() == 0) {
        if (not append_segment()) {
          return false;
        }
      }
      size_t              to_write = std::min(ctrl_blk_ptr->tail->tailroom(), bytes.size() - count);
      span<const uint8_t> subspan  = bytes.subspan(count, to_write);
      ctrl_blk_ptr->tail->append(subspan);
      count += to_write;
      ctrl_blk_ptr->pkt_len += to_write;
    }
    return true;
  }

  /// Appends an initializer list of bytes.
  void append(const std::initializer_list<uint8_t>& bytes) { append(span<const uint8_t>{bytes.begin(), bytes.size()}); }

  /// Appends bytes from another byte_buffer. This function may allocate new segments.
  bool append(const byte_buffer& other)
  {
    srsgnb_sanity_check(&other != this, "Self-append not supported");
    if (empty() and not other.empty()) {
      if (not append_segment()) {
        return false;
      }
    }
    for (byte_buffer_segment* seg = other.ctrl_blk_ptr->head.get(); seg != nullptr; seg = seg->next().get()) {
      auto other_it = seg->begin();
      while (other_it != seg->end()) {
        if (ctrl_blk_ptr->tail->tailroom() == 0) {
          if (not append_segment()) {
            return false;
          }
        }
        auto to_append = std::min(seg->end() - other_it, (iterator::difference_type)ctrl_blk_ptr->tail->tailroom());
        ctrl_blk_ptr->tail->append(other_it, other_it + to_append);
        other_it += to_append;
      }
      ctrl_blk_ptr->pkt_len += seg->length();
    }
    return true;
  }

  /// Appends bytes to the byte buffer. This function may allocate new segments.
  bool append(uint8_t byte)
  {
    if (empty() or ctrl_blk_ptr->tail->tailroom() == 0) {
      if (not append_segment()) {
        return false;
      }
    }
    ctrl_blk_ptr->tail->append(byte);
    ctrl_blk_ptr->pkt_len++;
    return true;
  }

  /// Appends a view of bytes into current byte buffer.
  void append(const byte_buffer_view& view)
  {
    // append segment by segment.
    auto view_segs = view.segments();
    for (span<const uint8_t> seg : view_segs) {
      append(seg);
    }
  }

  /// Appends an owning view of bytes into current byte buffer.
  void append(const byte_buffer_slice& view);

  /// Prepends bytes to byte_buffer. This function may allocate new segments.
  bool prepend(span<const uint8_t> bytes)
  {
    if (empty()) {
      // the byte buffer is empty. Prepending is the same as appending.
      return append(bytes);
    }
    for (size_t count = 0; count < bytes.size();) {
      if (ctrl_blk_ptr->head->headroom() == 0) {
        if (not prepend_segment(std::min((size_t)byte_buffer_segment::SEGMENT_SIZE, bytes.size() - count))) {
          return false;
        }
      }
      size_t              to_write = std::min(ctrl_blk_ptr->head->headroom(), bytes.size() - count);
      span<const uint8_t> subspan  = bytes.subspan(bytes.size() - to_write - count, to_write);
      ctrl_blk_ptr->head->prepend(subspan);
      ctrl_blk_ptr->pkt_len += to_write;
      count += to_write;
    }
    return true;
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
      ctrl_blk_ptr->pkt_len += after.ctrl_blk_ptr->pkt_len;
      byte_buffer_segment* new_tail = after.ctrl_blk_ptr->tail;
      ctrl_blk_ptr->tail->next()    = std::move(after.ctrl_blk_ptr->head);
      ctrl_blk_ptr->tail            = new_tail;
    }
    return *this;
  }

  /// Chains two byte buffers. The before byte buffer becomes empty.
  byte_buffer& chain_before(byte_buffer before)
  {
    if (empty()) {
      *this = std::move(before);
    } else {
      before.ctrl_blk_ptr->pkt_len += ctrl_blk_ptr->pkt_len;
      byte_buffer_segment* new_tail     = ctrl_blk_ptr->tail;
      before.ctrl_blk_ptr->tail->next() = std::move(ctrl_blk_ptr->head);
      before.ctrl_blk_ptr->tail         = new_tail;
      ctrl_blk_ptr                      = std::move(before.ctrl_blk_ptr);
    }
    return *this;
  }

  /// Clear byte buffer.
  void clear() { ctrl_blk_ptr.reset(); }

  /// Removes "nof_bytes" from the head of the byte_buffer.
  void trim_head(size_t nof_bytes)
  {
    srsgnb_sanity_check(length() >= nof_bytes, "Trying to trim more bytes than those available");
    for (size_t trimmed = 0; trimmed != nof_bytes;) {
      size_t to_trim = std::min(nof_bytes - trimmed, ctrl_blk_ptr->head->length());
      ctrl_blk_ptr->head->trim_head(to_trim);
      ctrl_blk_ptr->pkt_len -= to_trim;
      trimmed += to_trim;
      if (ctrl_blk_ptr->head->length() == 0) {
        // Remove the first segment.
        ctrl_blk_ptr->head = std::move(ctrl_blk_ptr->head->next());
      }
    }
  }

  /// \brief Remove "nof_bytes" bytes at the end of the byte_buffer.
  /// If the length is greater than the length of the last segment, the function will fail and return -1 without
  /// modifying the byte_buffer.
  void trim_tail(size_t nof_bytes)
  {
    srsgnb_assert(length() >= nof_bytes, "Trimming too many bytes from byte_buffer");
    if (nof_bytes == 0) {
      return;
    }

    if (ctrl_blk_ptr->tail->length() >= nof_bytes) {
      // Simplest scenario where the last segment is larger than the number of bytes to trim.
      ctrl_blk_ptr->tail->trim_tail(nof_bytes);
      ctrl_blk_ptr->pkt_len -= nof_bytes;
      if (ctrl_blk_ptr->tail->length() == 0) {
        pop_last_segment();
      }
      return;
    }
    size_t new_len = length() - nof_bytes;
    auto   seg     = ctrl_blk_ptr->head.get();
    for (size_t count = 0; seg != nullptr; seg = seg->next().get()) {
      if (count + seg->length() >= new_len) {
        seg->next() = nullptr;
        seg->resize(new_len - count);
        ctrl_blk_ptr->tail    = seg;
        ctrl_blk_ptr->pkt_len = new_len;
        break;
      }
      count += seg->length();
    }
  }

  /// Checks whether byte_buffer is empty.
  bool empty() const { return length() == 0; }

  /// Checks byte_buffer length.
  size_t length() const { return ctrl_blk_ptr != nullptr ? ctrl_blk_ptr->pkt_len : 0; }

  uint8_t&       back() { return ctrl_blk_ptr->tail->back(); }
  const uint8_t& back() const { return ctrl_blk_ptr->tail->back(); }

  const uint8_t& operator[](size_t i) const { return *(begin() + i); }
  uint8_t&       operator[](size_t i) { return *(begin() + i); }

  iterator       begin() { return iterator{ctrl_blk_ptr != nullptr ? ctrl_blk_ptr->head.get() : nullptr, 0}; }
  const_iterator cbegin() const
  {
    return const_iterator{ctrl_blk_ptr != nullptr ? ctrl_blk_ptr->head.get() : nullptr, 0};
  }
  const_iterator begin() const
  {
    return const_iterator{ctrl_blk_ptr != nullptr ? ctrl_blk_ptr->head.get() : nullptr, 0};
  }
  iterator       end() { return iterator{nullptr, 0}; }
  const_iterator end() const { return const_iterator{nullptr, 0}; }
  const_iterator cend() const { return const_iterator{nullptr, 0}; }

  /// Test if byte buffer is contiguous in memory, i.e. it has only one segment.
  bool is_contiguous() const { return empty() or ctrl_blk_ptr->head.get() == ctrl_blk_ptr->tail; }

  /// Moves the bytes stored in different segments of the byte_buffer into first segment.
  /// \return 0 if the data could fit in one segment. -1 otherwise, and the byte_buffer remains unaltered.
  int linearize()
  {
    if (is_contiguous()) {
      return 0;
    }
    size_t sz = length();
    if (sz > byte_buffer_segment::capacity() - ctrl_blk_ptr->head->headroom()) {
      return -1;
    }
    for (byte_buffer_segment* seg = ctrl_blk_ptr->head->next().get(); seg != nullptr; seg = seg->next().get()) {
      ctrl_blk_ptr->head->append(seg->begin(), seg->end());
    }
    ctrl_blk_ptr->head->next() = nullptr;
    ctrl_blk_ptr->tail         = ctrl_blk_ptr->head.get();
    return 0;
  }

  /// Set byte_buffer length. Note: It doesn't initialize newly created bytes.
  bool resize(size_t new_sz)
  {
    size_t prev_len = length();
    if (new_sz == prev_len) {
      return true;
    }
    if (new_sz > prev_len) {
      for (size_t to_add = new_sz - prev_len; to_add > 0;) {
        if (empty() or ctrl_blk_ptr->tail->tailroom() == 0) {
          if (not append_segment()) {
            return false;
          }
        }
        size_t added = std::min(ctrl_blk_ptr->tail->tailroom(), to_add);
        ctrl_blk_ptr->tail->resize(added);
        to_add -= added;
      }
    } else {
      size_t count = 0;
      for (byte_buffer_segment* seg = ctrl_blk_ptr->head.get(); count < new_sz; seg = seg->next().get()) {
        size_t new_seg_len = std::min(seg->length(), new_sz - count);
        if (new_seg_len != seg->length()) {
          seg->resize(new_seg_len);
        }
        count += new_seg_len;
        if (count == new_sz) {
          seg->next()        = nullptr;
          ctrl_blk_ptr->tail = seg;
        }
      }
    }
    ctrl_blk_ptr->pkt_len = new_sz;
    return true;
  }

  /// Returns a non-owning list of segments that compose the byte_buffer.
  byte_buffer_segment_range segments()
  {
    return byte_buffer_segment_range(ctrl_blk_ptr != nullptr ? ctrl_blk_ptr->head.get() : nullptr, 0, length());
  }
  const_byte_buffer_segment_range segments() const
  {
    return const_byte_buffer_segment_range(ctrl_blk_ptr != nullptr ? ctrl_blk_ptr->head.get() : nullptr, 0, length());
  }

  /// \brief Equality comparison between byte buffer view and another range.
  template <typename R>
  friend bool operator==(const byte_buffer& lhs, const R& r)
  {
    return detail::compare_byte_buffer_range(lhs, r);
  }
  template <typename T, std::enable_if_t<std::is_convertible<T, span<const uint8_t>>::value, int> = 0>
  friend bool operator==(const T& r, const byte_buffer& rhs)
  {
    return detail::compare_byte_buffer_range(rhs, r);
  }
  template <typename T>
  friend bool operator!=(const byte_buffer& lhs, const T& r)
  {
    return !(lhs == r);
  }
  template <typename T, std::enable_if_t<std::is_convertible<T, span<const uint8_t>>::value, int> = 0>
  friend bool operator!=(const T& r, const byte_buffer& rhs)
  {
    return !(rhs == r);
  }

private:
  bool append_segment()
  {
    if (empty()) {
      ctrl_blk_ptr = std::make_shared<control_block>();
      // For first segment of byte_buffer, add a headroom.
      std::unique_ptr<byte_buffer_segment> p{new (std::nothrow)
                                                 byte_buffer_segment(byte_buffer_segment::DEFAULT_HEADROOM)};
      if (p == nullptr) {
        warn_alloc_failure();
        return false;
      }
      ctrl_blk_ptr->tail = p.get();
      ctrl_blk_ptr->head = std::move(p);
    } else {
      // No headroom needed for later segments.
      std::unique_ptr<byte_buffer_segment> p{new (std::nothrow) byte_buffer_segment(0)};
      if (p == nullptr) {
        warn_alloc_failure();
        return false;
      }
      ctrl_blk_ptr->tail->next() = std::move(p);
      ctrl_blk_ptr->tail         = ctrl_blk_ptr->tail->next().get();
    }
    return true;
  }

  bool prepend_segment(size_t headroom_size)
  {
    std::unique_ptr<byte_buffer_segment> p{new (std::nothrow) byte_buffer_segment(headroom_size)};
    if (p == nullptr) {
      warn_alloc_failure();
      return false;
    }
    if (empty()) {
      ctrl_blk_ptr       = std::make_shared<control_block>();
      ctrl_blk_ptr->tail = p.get();
      ctrl_blk_ptr->head = std::move(p);
    } else {
      p->next()          = std::move(ctrl_blk_ptr->head);
      ctrl_blk_ptr->head = std::move(p);
    }
    return true;
  }

  /// \brief Removes last segment of the byte_buffer.
  /// Note: This operation is O(N), as it requires recomputing the tail.
  void pop_last_segment()
  {
    if (ctrl_blk_ptr->tail == nullptr) {
      return;
    }
    if (ctrl_blk_ptr->head.get() == ctrl_blk_ptr->tail) {
      // there is only one segment.
      clear();
      return;
    }
    ctrl_blk_ptr->pkt_len -= ctrl_blk_ptr->tail->length();
    // recompute tail.
    byte_buffer_segment* new_tail = ctrl_blk_ptr->head.get();
    for (; new_tail->next().get() != ctrl_blk_ptr->tail; new_tail = new_tail->next().get()) {
    }
    ctrl_blk_ptr->tail         = new_tail;
    ctrl_blk_ptr->tail->next() = nullptr;
  }

  void warn_alloc_failure()
  {
    static srslog::basic_logger& logger = srslog::fetch_basic_logger("ALL");
    logger.warning("POOL: Failure to allocate byte buffer segment");
  }

  // TODO: Optimize. shared_ptr<> has a lot of boilerplate we don't need.
  std::shared_ptr<control_block> ctrl_blk_ptr = nullptr;
};

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

  byte_buffer_slice()                                           = default;
  explicit byte_buffer_slice(const byte_buffer_slice&) noexcept = default;
  byte_buffer_slice(byte_buffer_slice&&) noexcept               = default;
  byte_buffer_slice(span<const uint8_t> bytes) : byte_buffer_slice(byte_buffer{bytes}) {}
  byte_buffer_slice(std::initializer_list<uint8_t> bytes) : byte_buffer_slice(byte_buffer{bytes}) {}
  byte_buffer_slice(byte_buffer&& buf_) : buf(std::move(buf_)), sliced_view(buf) {}
  byte_buffer_slice(const byte_buffer& buf_) : buf(buf_.copy()), sliced_view(buf) {}
  byte_buffer_slice(const byte_buffer& buf_, size_t offset, size_t length) :
    buf(buf_.copy()), sliced_view(buf, offset, length)
  {
  }
  byte_buffer_slice(const byte_buffer& buf_, byte_buffer_view view) : buf(buf_.copy()), sliced_view(view)
  {
    srsgnb_sanity_check(view.begin() - byte_buffer_view{buf}.begin() < (int)buf.length(),
                        "byte_buffer_view is not part of the owned byte_buffer");
  }

  /// Copy assignment is disabled. Use std::move or .copy() instead
  byte_buffer_slice& operator=(const byte_buffer_slice&) noexcept = delete;

  /// Move assignment of byte_buffer_slice. It avoids unnecessary reference counting increment.
  byte_buffer_slice& operator=(byte_buffer_slice&& other) noexcept = default;

  /// Performs a shallow copy. Nested segment reference counter is incremented.
  byte_buffer_slice copy() const { return byte_buffer_slice{*this}; }

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
    srsgnb_assert(offset + size <= length(), "Invalid view dimensions.");
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

  /// Returns a non-owning list of segments that compose the byte_buffer.
  const_byte_buffer_segment_range segments() const { return sliced_view.segments(); }

  /// \brief Equality comparison between byte buffer slice and another range.
  template <typename T>
  friend bool operator==(const byte_buffer_slice& lhs, const T& r)
  {
    return detail::compare_byte_buffer_range(lhs, r);
  }
  template <typename T, std::enable_if_t<not is_byte_buffer_range<T>::value, int> = 0>
  friend bool operator==(const T& r, const byte_buffer_slice& rhs)
  {
    return detail::compare_byte_buffer_range(rhs, r);
  }
  template <typename T>
  friend bool operator!=(const byte_buffer_slice& lhs, const T& r)
  {
    return not(lhs == r);
  }
  template <typename T, std::enable_if_t<not is_byte_buffer_range<T>::value, int> = 0>
  friend bool operator!=(const T& r, const byte_buffer_slice& rhs)
  {
    return not(rhs == r);
  }

private:
  byte_buffer      buf;
  byte_buffer_view sliced_view;
};

inline void byte_buffer::append(const byte_buffer_slice& slice)
{
  append(slice.view());
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
  size_t rem_bytes = nof_bytes;
  while (rem_bytes > 0) {
    if (empty() or ctrl_blk_ptr->head->headroom() == 0) {
      if (not prepend_segment(std::min((size_t)byte_buffer_segment::SEGMENT_SIZE, rem_bytes))) {
        return {};
      }
    }
    size_t to_reserve = std::min(ctrl_blk_ptr->head->headroom(), rem_bytes);
    ctrl_blk_ptr->head->reserve_prepend(to_reserve);
    rem_bytes -= to_reserve;
  }
  ctrl_blk_ptr->pkt_len += nof_bytes;
  return byte_buffer_view{begin(), begin() + nof_bytes};
}

/// Converts a hex string (e.g. 01FA02) to a byte buffer.
inline byte_buffer make_byte_buffer(const std::string& hex_str)
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

/// Perfoms a segment-wise copy of the byte_buffer into a span<uint8_t> object.
/// The length is limited by the length of src and dst, whichever is smaller.
///
/// \param src Source byte_buffer.
/// \param dst Destination span<uint8_t>.
/// \return Number of bytes copied.
inline size_t copy_segments(const byte_buffer& src, span<uint8_t> dst)
{
  size_t bytes_copied    = 0;
  size_t bytes_remaining = std::min(src.length(), dst.size_bytes());
  for (const auto& src_segment : src.segments()) {
    size_t        block_size  = std::min(bytes_remaining, src_segment.size());
    span<uint8_t> dst_segment = dst.subspan(bytes_copied, block_size);
    std::copy(src_segment.begin(), src_segment.begin() + block_size, dst_segment.begin());
    bytes_copied += block_size;
    bytes_remaining -= block_size;
    if (bytes_remaining == 0) {
      break;
    }
  }
  return bytes_copied;
}

/// Perfoms a segment-wise copy of the byte_buffer to a generic iterator over uint8_t.
/// The destination must have sufficient space to fit the whole byte_buffer's length.
///
/// \param src Source byte_buffer.
/// \param dst_begin Destination iterator over uint8_t.
template <typename It>
void copy_segments(const byte_buffer& src, It dst_begin)
{
  static_assert(std::is_same<std::decay_t<decltype(*dst_begin)>, uint8_t>::value, "Iterator value type is not uint8_t");
  for (const auto& src_segment : src.segments()) {
    dst_begin = std::copy(src_segment.begin(), src_segment.end(), dst_begin);
  }
}

/// Provides a contiguous view of a byte_buffer.
/// It is bound to the lifetime of the byte_buffer \p src and the temporary buffer \p tmp_mem, whichever is shorter.
/// Important: \p tmp_mem must be at least as large as \p src.
///
/// The memory used for the result and the complexity depends on whether \p src is contiguous or not:
/// - Contiguous: Result is a view of the first and only segment of \p src.
/// - Non-Contiguous: Result is a view of \p tmp_mem in which all segments of \p src are copied.
///
/// \param src Source byte_buffer.
/// \param tmp_mem Temporary memory for a possible copy. Must be at least as large as \p src.
/// \return A contiguous view of the byte_buffer
inline span<const uint8_t> to_span(const byte_buffer& src, span<uint8_t> tmp_mem)
{
  // empty buffer
  if (src.empty()) {
    return {};
  }

  // is contiguous: shortcut without copy
  if (src.is_contiguous()) {
    return *src.segments().begin();
  }
  // non-contiguous: copy required
  srsgnb_assert(src.length() <= tmp_mem.size_bytes(),
                "Insufficient temporary memory to fit the byte_buffer. buffer_size={}, tmp_size={}",
                src.length(),
                tmp_mem.size());
  span<uint8_t> result = {tmp_mem.data(), src.length()};
  copy_segments(src, result);
  return result;
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
