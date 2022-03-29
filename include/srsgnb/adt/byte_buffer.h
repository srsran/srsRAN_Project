#ifndef SRSGNB_ADT_BYTE_BUFFER_H
#define SRSGNB_ADT_BYTE_BUFFER_H

#include "srsgnb/adt/span.h"
#include "srsgnb/srslog/bundled/fmt/format.h"
#include "srsgnb/support/srsran_assert.h"
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

  byte_buffer_segment() : data_(buffer.data() + DEFAULT_HEADROOM), data_end_(buffer.data() + DEFAULT_HEADROOM) {}
  byte_buffer_segment(const byte_buffer_segment& other) noexcept : data_(buffer.data() + other.headroom()),
                                                                   data_end_(buffer.data() + other.tailroom_start())
  {
    std::copy(other.begin(), other.end(), begin());
  }
  byte_buffer_segment(byte_buffer_segment&& other) noexcept : data_(buffer.data() + other.headroom()),
                                                              data_end_(buffer.data() + other.tailroom_start())
  {
    std::copy(other.begin(), other.end(), begin());
  }
  byte_buffer_segment& operator=(const byte_buffer_segment& other) noexcept
  {
    if (this != &other) {
      data_     = buffer.data() + other.headroom();
      data_end_ = buffer.data() + other.tailroom_start();
      std::copy(other.begin(), other.end(), begin());
    }
    return *this;
  }
  byte_buffer_segment& operator=(byte_buffer_segment&& other) noexcept
  {
    data_     = buffer.data() + other.headroom();
    data_end_ = buffer.data() + other.tailroom_start();
    std::copy(other.begin(), other.end(), begin());
    return *this;
  }

  static size_t capacity() { return SEGMENT_SIZE; }

  /// Returns how much space in bytes there is at the head of the segment.
  size_t headroom() const { return data() - buffer.data(); }

  /// Returns size in bytes of segment.
  size_t length() const { return end() - begin(); }

  /// Returns how much space in bytes there is at the tail of the segment.
  size_t tailroom() const { return buffer.end() - end(); }

  /// Appends a span of bytes at the tail of the segment.
  void append(span<const uint8_t> bytes)
  {
    srsran_sanity_check(bytes.size() <= tailroom(), "There is not enough tailroom for append.");
    append(bytes.begin(), bytes.end());
  }

  /// Appends a iterator range of bytes at the tail of the segment.
  template <typename It>
  void append(It it_begin, It it_end)
  {
    static_assert(std::is_same<std::decay_t<decltype(*it_begin)>, uint8_t>::value, "Invalid value_type");
    data_end_ = std::copy(it_begin, it_end, end());
  }

  /// Appends single byte at the tail of the segment.
  void append(uint8_t byte)
  {
    srsran_assert(tailroom() >= 1, "There is not enough tailroom space.");
    buffer[tailroom_start()] = byte;
    data_end_++;
  }

  /// Prepends segment with provided span of bytes.
  void prepend(span<const uint8_t> bytes)
  {
    srsran_assert(headroom() >= bytes.size(), "There is not enough headroom space.");
    data_ -= bytes.size();
    std::copy(bytes.begin(), bytes.end(), begin());
  }

  /// Removes "nof_bytes" from the head of the segment.
  void trim_head(size_t nof_bytes)
  {
    srsran_assert(nof_bytes <= length(), "There is not enough headroom space.");
    data_ += nof_bytes;
  }

  /// Removes "nof_bytes" from the tail of the segment.
  void trim_tail(size_t nof_bytes)
  {
    srsran_assert(nof_bytes <= length(), "There is not enough headroom space.");
    data_end_ -= nof_bytes;
  }

  uint8_t& operator[](size_t idx)
  {
    srsran_sanity_check(idx < length(), "Out-of-bound access");
    return *(begin() + idx);
  }
  const uint8_t& operator[](size_t idx) const
  {
    srsran_sanity_check(idx < length(), "Out-of-bound access");
    return *(begin() + idx);
  }

  const uint8_t* data() const { return begin(); }
  uint8_t*       data() { return begin(); }

  iterator       begin() { return data_; }
  iterator       end() { return data_end_; }
  const_iterator begin() const { return data_; }
  const_iterator end() const { return data_end_; }

  metadata_storage&       metadata() { return metadata_; }
  const metadata_storage& metadata() const { return metadata_; }

  byte_buffer_segment*       next() { return metadata().next.get(); }
  const byte_buffer_segment* next() const { return metadata().next.get(); }

  template <typename Container>
  bool operator==(const Container& other) const
  {
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
  uint8_t*                          data_;
  uint8_t*                          data_end_;
};

class byte_buffer_view;

/// Memory buffer that store bytes in a linked list of memory chunks.
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

    iterator_impl(byte_buffer_segment* start_segment, size_t offset_) : current_segment(start_segment), offset(offset_)
    {}

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
    iterator_impl operator+(difference_type n) const
    {
      iterator_impl tmp(*this);
      tmp += n;
      return tmp;
    }
    iterator_impl& operator+=(difference_type n)
    {
      offset += n;
      while (offset > current_segment->length()) {
        offset -= current_segment->length();
        current_segment = current_segment->next();
      }
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

    byte_buffer_segment* current_segment;
    size_t               offset;
  };

public:
  using value_type     = uint8_t;
  using iterator       = iterator_impl<uint8_t>;
  using const_iterator = iterator_impl<const uint8_t>;

  byte_buffer() = default;
  byte_buffer(std::initializer_list<uint8_t> lst) : byte_buffer(span<const uint8_t>{lst.begin(), lst.size()}) {}
  byte_buffer(span<const uint8_t> bytes) { append(bytes); }
  template <typename It>
  byte_buffer(It b, It e)
  {
    // TODO: optimize
    for (; b != e; ++b) {
      append(*b);
    }
  }
  byte_buffer(const byte_buffer&) = delete;
  byte_buffer(byte_buffer&& other) noexcept : head(std::move(other.head)), tail(other.tail), len(other.len)
  {
    other.tail = nullptr;
    other.len  = 0;
  }
  byte_buffer& operator=(const byte_buffer&) noexcept = delete;
  byte_buffer& operator                               =(byte_buffer&& other) noexcept
  {
    head       = std::move(other.head);
    tail       = other.tail;
    len        = other.len;
    other.tail = nullptr;
    other.len  = 0;
    return *this;
  }

  /// Performs a deep copy (byte by bytes) of this byte_buffer.
  byte_buffer clone()
  {
    byte_buffer buf;
    for (byte_buffer_segment* seg = head.get(); seg != nullptr; seg = seg->next()) {
      buf.append(span<uint8_t>{seg->data(), seg->length()});
    }
    return buf;
  }

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
    }
    len += bytes.size();
  }

  /// Appends bytes in iterator range to the byte buffer. This function may allocate new segments.
  template <typename It>
  void append(It it_begin, It it_end)
  {
    if (empty() and it_begin != it_end) {
      append_segment();
    }
    size_t to_add = it_end - it_begin;
    while (it_begin != it_end) {
      if (tail->tailroom() == 0) {
        append_segment();
      }
      It it_next = it_begin + std::min(tail->tailroom(), it_end - it_begin);
      tail->append(it_begin, it_next);
      it_begin = it_next;
    }
    len += to_add;
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

  /// Clear byte buffer.
  void clear()
  {
    head = nullptr;
    tail = nullptr;
  }

  /// Removes "nof_bytes" from the head of the byte_buffer.
  void trim_head(size_t nof_bytes)
  {
    srsran_sanity_check(length() >= nof_bytes, "Trying to trim more bytes than those available");
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

  /// Compares bytes buffers byte-wise.
  bool operator==(const byte_buffer& other) const
  {
    // TODO: Probably can be done in batches of segments.
    auto it = begin(), it2 = other.begin();
    for (; it != end() and it2 != other.end(); ++it, ++it2) {
      if (*it != *it2) {
        return false;
      }
    }
    return (it == end()) and (it2 == other.end());
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

private:
  void append_segment()
  {
    // TODO: Use memory pool.
    // TODO: Verify if allocation was successful. What to do if not?
    auto buf = std::make_shared<byte_buffer_segment>();
    if (empty()) {
      head = buf;
      tail = buf.get();
    } else {
      tail->metadata().next = buf;
      tail                  = buf.get();
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

/// Non-owning range used to traverse a slice of a byte buffer.
class byte_buffer_view
{
public:
  using const_iterator = byte_buffer::const_iterator;

  byte_buffer_view() = default;
  byte_buffer_view(const_iterator it_begin_, const_iterator it_end_) : it(it_begin_), it_end(it_end_) {}
  byte_buffer_view(const byte_buffer& buffer) : it(buffer.begin()), it_end(buffer.end()) {}

  const_iterator begin() const { return it; }
  const_iterator end() const { return it_end; }

  bool empty() const { return it == it_end; }

  size_t length() const { return it_end - it; }

  const uint8_t& operator[](size_t i) const { return *(it + i); }

  /// Returns another sub-view with dimensions specified in arguments.
  byte_buffer_view slice(size_t offset, size_t size) const
  {
    srsran_sanity_check(offset + size <= length(), "Invalid slice dimensions.");
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

  bool operator==(const byte_buffer_view& other) const
  {
    return std::equal(begin(), end(), other.begin(), other.end());
  }

protected:
  const_iterator it     = const_iterator{nullptr, 0};
  const_iterator it_end = const_iterator{nullptr, 0};
};

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
  byte_buffer_view advance(size_t offset)
  {
    auto prev_it = it;
    it += offset;
    return {prev_it, it};
  }
};

} // namespace srsgnb

namespace fmt {

/// \brief Custom formatter for byte_buffer
template <>
struct formatter<srsgnb::byte_buffer> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::byte_buffer& buf, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{:0>2x}", fmt::join(buf.begin(), buf.end(), " "));
  }
};

} // namespace fmt

#endif // SRSGNB_ADT_BYTE_BUFFER_H