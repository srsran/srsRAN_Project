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
#include "srsgnb/srslog/bundled/fmt/format.h"
#include <vector>

namespace srsgnb {

class byte_buffer_view;
class byte_buffer_slice;

/// \brief Byte sequence, which represents its data in memory via an intrusive linked list of memory chunks.
/// This container is not contiguous in memory.
/// Default copy ctor and assignment is disabled in this container. The user should instead std::move to transfer
/// ownership, .copy() for shallow copies with shared ownership and .deep_copy() for byte-wise copies.
class byte_buffer
{
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
    for (byte_buffer_segment* seg = head.get(); seg != nullptr; seg = seg->next()) {
      buf.append(span<uint8_t>{seg->data(), seg->length()});
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
  void append(span<const uint8_t> bytes)
  {
    if (empty() and not bytes.empty()) {
      append_segment();
    }
    // segment-wise copy.
    for (size_t count = 0; count < bytes.size();) {
      if (get_tail()->tailroom() == 0) {
        append_segment();
      }
      size_t              to_write = std::min(get_tail()->tailroom(), bytes.size() - count);
      span<const uint8_t> subspan  = bytes.subspan(count, to_write);
      get_tail()->append(subspan);
      count += to_write;
      head->metadata().pkt_len += to_write;
    }
  }

  /// Appends an initializer list of bytes.
  void append(const std::initializer_list<uint8_t>& bytes) { append(span<const uint8_t>{bytes.begin(), bytes.size()}); }

  /// Appends bytes from another byte_buffer. This function may allocate new segments.
  void append(const byte_buffer& other)
  {
    srsgnb_sanity_check(&other != this, "Self-append not supported");
    if (empty() and not other.empty()) {
      append_segment();
    }
    for (byte_buffer_segment* seg = other.head.get(); seg != nullptr; seg = seg->next()) {
      auto other_it = seg->begin();
      while (other_it != seg->end()) {
        if (get_tail()->tailroom() == 0) {
          append_segment();
        }
        auto to_append = std::min(seg->end() - other_it, (iterator::difference_type)get_tail()->tailroom());
        get_tail()->append(other_it, other_it + to_append);
        other_it += to_append;
      }
      head->metadata().pkt_len += seg->length();
    }
  }

  /// Appends bytes to the byte buffer. This function may allocate new segments.
  void append(uint8_t byte)
  {
    if (empty() or get_tail()->tailroom() == 0) {
      append_segment();
    }
    get_tail()->append(byte);
    head->metadata().pkt_len++;
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
      head->metadata().pkt_len += to_write;
      count += to_write;
    }
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
      head->metadata().pkt_len += after.head->metadata().pkt_len;
      byte_buffer_segment* new_tail = after.get_tail();
      get_tail()->metadata().next   = std::move(after.head);
      set_tail(new_tail);
    }
    return *this;
  }

  /// Chains two byte buffers. The before byte buffer becomes empty.
  byte_buffer& chain_before(byte_buffer before)
  {
    if (empty()) {
      *this = std::move(before);
    } else {
      before.head->metadata().pkt_len += head->metadata().pkt_len;
      byte_buffer_segment* new_tail      = get_tail();
      before.get_tail()->metadata().next = std::move(head);
      before.set_tail(new_tail);
      head = std::move(before.head);
    }
    return *this;
  }

  /// Clear byte buffer.
  void clear()
  {
    set_tail(nullptr);
    head->metadata().pkt_len = 0;
    head                     = nullptr;
  }

  /// Removes "nof_bytes" from the head of the byte_buffer.
  void trim_head(size_t nof_bytes)
  {
    srsgnb_sanity_check(length() >= nof_bytes, "Trying to trim more bytes than those available");
    for (size_t trimmed = 0; trimmed != nof_bytes;) {
      size_t to_trim = std::min(nof_bytes - trimmed, head->length());
      head->trim_head(to_trim);
      head->metadata().pkt_len -= to_trim;
      trimmed += to_trim;
      if (head->length() == 0) {
        // Remove the first segment.
        if (head->metadata().next != nullptr) {
          head->metadata().next->metadata().pkt_len = length();
          head->metadata().next->metadata().tail    = get_tail();
        }
        head = std::move(head->metadata().next);
      }
    }
  }

  /// \brief Remove "nof_bytes" bytes at the end of the byte_buffer.
  /// If the length is greater than the length of the last segment, the function will fail and return -1 without
  /// modifying the byte_buffer.
  /// \return 0 if successful, -1 otherwise.
  int trim_tail(size_t nof_bytes)
  {
    if (get_tail()->length() >= nof_bytes) {
      get_tail()->trim_tail(nof_bytes);
      head->metadata().pkt_len -= nof_bytes;
      return 0;
    }
    return -1;
  }

  /// Checks whether byte_buffer is empty.
  bool empty() const { return length() == 0; }

  /// Checks byte_buffer length.
  size_t length() const { return head != nullptr ? head->metadata().pkt_len : 0; }

  uint8_t&       back() { return get_tail()->back(); }
  const uint8_t& back() const { return get_tail()->back(); }

  const uint8_t& operator[](size_t i) const { return *(begin() + i); }
  uint8_t&       operator[](size_t i) { return *(begin() + i); }

  template <typename Container, std::enable_if_t<std::is_convertible<Container, span<const uint8_t>>::value, int> = 0>
  bool operator==(const Container& container) const
  {
    // Use size for fast comparison.
    if (static_cast<ssize_t>(length()) != container.end() - container.begin()) {
      return false;
    }
    // Compare segment by segment.
    auto segs = segments();
    auto it   = container.begin();
    for (auto seg_it = segs.begin(); seg_it != segs.end(); ++seg_it) {
      auto next_it = it + (seg_it->end() - seg_it->begin());
      if (not std::equal(seg_it->begin(), seg_it->end(), it, next_it)) {
        return false;
      }
      it = next_it;
    }
    srsgnb_sanity_check(it == container.end(), "Invalid byte_buffer::length()");
    return true;
  }
  template <typename Container,
            std::enable_if_t<not std::is_convertible<Container, span<const uint8_t>>::value, int> = 0>
  bool operator==(const Container& container) const
  {
    return std::equal(begin(), end(), container.begin(), container.end());
  }
  template <typename Container>
  bool operator!=(const Container& other) const
  {
    return !(*this == other);
  }

  iterator       begin() { return iterator{head.get(), 0}; }
  const_iterator cbegin() const { return const_iterator{head.get(), 0}; }
  const_iterator begin() const { return const_iterator{head.get(), 0}; }
  iterator       end() { return iterator{nullptr, 0}; }
  const_iterator end() const { return const_iterator{nullptr, 0}; }
  const_iterator cend() const { return const_iterator{nullptr, 0}; }

  /// Test if byte buffer is contiguous in memory, i.e. it has only one segment.
  bool is_contiguous() const { return head.get() == get_tail(); }

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
    set_tail(head.get());
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
        if (empty() or get_tail()->tailroom() == 0) {
          append_segment();
        }
        size_t added = std::min(get_tail()->tailroom(), to_add);
        get_tail()->resize(added);
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
          set_tail(seg);
        }
      }
    }
    head->metadata().pkt_len = new_sz;
  }

  /// Returns a non-owning list of segments that compose the byte_buffer.
  byte_buffer_segment_range       segments() { return byte_buffer_segment_range(head.get(), 0, length()); }
  const_byte_buffer_segment_range segments() const { return const_byte_buffer_segment_range(head.get(), 0, length()); }

private:
  void append_segment()
  {
    // TODO: Use memory pool.
    // TODO: Verify if allocation was successful. What to do if not?
    if (empty()) {
      // For first segment of byte_buffer, add a headroom.
      head = std::make_shared<byte_buffer_segment>((size_t)byte_buffer_segment::DEFAULT_HEADROOM);
      set_tail(head.get());
    } else {
      // No headroom needed for later segments.
      get_tail()->metadata().next = std::make_shared<byte_buffer_segment>(0);
      set_tail(get_tail()->next());
    }
  }

  void prepend_segment()
  {
    // TODO: Use memory pool.
    // TODO: Verify if allocation was successful. What to do if not?
    auto buf = std::make_shared<byte_buffer_segment>();
    if (empty()) {
      head = std::move(buf);
      set_tail(head.get());
    } else {
      unsigned pkt_len         = length();
      buf->metadata().next     = std::move(head);
      head                     = std::move(buf);
      head->metadata().pkt_len = pkt_len;
    }
  }

  void                       set_tail(byte_buffer_segment* new_tail) { head->metadata().tail = new_tail; }
  const byte_buffer_segment* get_tail() const { return head->metadata().tail; }
  byte_buffer_segment*       get_tail() { return head->metadata().tail; }

  // TODO: Optimize. shared_ptr<> has a lot of boilerplate we don't need.
  std::shared_ptr<byte_buffer_segment> head = nullptr;
};

inline bool operator==(const byte_buffer& buf, span<const uint8_t> bytes)
{
  if (buf.length() != bytes.size()) {
    return false;
  }
  unsigned i = 0;
  for (uint8_t val : buf) {
    if (val != bytes[i]) {
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
  byte_buffer_segment_range       segments() { return {it, length()}; }
  const_byte_buffer_segment_range segments() const { return {it, length()}; }

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
  // append segment by segment.
  auto view_segs = view.segments();
  for (auto seg_it = view_segs.begin(); seg_it != view_segs.end(); ++seg_it) {
    append(*seg_it);
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
  byte_buffer_segment_range       segments() { return sliced_view.segments(); }
  const_byte_buffer_segment_range segments() const { return sliced_view.segments(); }

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
  head->metadata().pkt_len += nof_bytes;
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
