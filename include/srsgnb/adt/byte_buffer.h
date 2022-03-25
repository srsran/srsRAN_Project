#ifndef SRSGNB_ADT_BYTE_BUFFER_H
#define SRSGNB_ADT_BYTE_BUFFER_H

#include "srsgnb/adt/span.h"
#include "srsgnb/srslog/bundled/fmt/format.h"
#include "srsgnb/support/srsran_assert.h"
#include <array>
#include <cstdint>
#include <vector>

namespace srsgnb {

class byte_buffer_segment
{
public:
  constexpr static size_t SEGMENT_SIZE     = 256;
  constexpr static size_t DEFAULT_HEADROOM = 16;

  struct metadata_storage {
    std::shared_ptr<byte_buffer_segment> next = nullptr;
  };

  size_t headroom() const { return headroom_; }
  size_t length() const { return payload_length; }
  size_t tailroom() const { return SEGMENT_SIZE - length() - headroom(); }

  void append(span<const uint8_t> bytes)
  {
    srsran_assert(tailroom() >= bytes.size(), "There is not enough tailroom space.");
    std::copy(bytes.begin(), bytes.end(), buffer.begin() + tailroom_start());
    payload_length += bytes.size();
  }

  void append(uint8_t byte)
  {
    srsran_assert(tailroom() >= 1, "There is not enough tailroom space.");
    buffer[tailroom_start()] = byte;
    payload_length++;
  }

  void prepend(span<const uint8_t> bytes)
  {
    srsran_assert(headroom() >= bytes.size(), "There is not enough headroom space.");
    headroom_ -= bytes.size();
    payload_length += bytes.size();
    std::copy(bytes.begin(), bytes.end(), buffer.begin() + headroom());
  }

  void trim_front(size_t nof_bytes)
  {
    srsran_assert(nof_bytes <= length(), "There is not enough headroom space.");
    headroom_ += nof_bytes;
    payload_length -= nof_bytes;
  }

  void trim_back(size_t nof_bytes)
  {
    srsran_assert(nof_bytes <= length(), "There is not enough headroom space.");
    payload_length -= nof_bytes;
  }

  uint8_t&       operator[](size_t idx) { return *(data() + idx); }
  const uint8_t& operator[](size_t idx) const { return *(data() + idx); }

  metadata_storage&       metadata() { return metadata_; }
  const metadata_storage& metadata() const { return metadata_; }

  const uint8_t* data() const { return buffer.data() + headroom(); }
  uint8_t*       data() { return buffer.data() + headroom(); }

  bool operator==(const byte_buffer_segment& other) const
  {
    return std::equal(buffer.begin() + headroom(),
                      buffer.begin() + headroom() + length(),
                      other.buffer.begin() + other.headroom(),
                      other.buffer.begin() + other.headroom() + other.length());
  }
  bool operator!=(const byte_buffer_segment& other) const { return not(*this == other); }

private:
  size_t tailroom_start() const { return headroom() + length(); }

  metadata_storage                  metadata_;
  std::array<uint8_t, SEGMENT_SIZE> buffer;
  size_t                            headroom_      = DEFAULT_HEADROOM;
  size_t                            payload_length = 0;
};

class byte_buffer_view;

/// Type to store a dynamic amount of contiguous bytes.
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
        current_segment = current_segment->metadata().next.get();
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
        current_segment = current_segment->metadata().next.get();
      }
      return *this;
    }

    difference_type operator-(const iterator_impl<T>& other) const
    {
      difference_type      diff = 0;
      byte_buffer_segment* seg  = other.current_segment;
      for (; seg != current_segment; seg = seg->metadata().next.get()) {
        diff += seg->length();
      }
      diff += offset - other.offset;
      return diff;
    }

    bool operator==(const iterator_impl<T>& other) const
    {
      return current_segment == other.current_segment and offset == other.offset;
    }
    bool operator!=(const iterator_impl<T>& other) const { return not(*this == other); }

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

  byte_buffer clone()
  {
    byte_buffer buf;
    for (byte_buffer_segment* seg = head.get(); seg != nullptr; seg = seg->metadata().next.get()) {
      buf.append(span<uint8_t>{seg->data(), seg->length()});
    }
    return buf;
  }

  void append(span<const uint8_t> bytes)
  {
    if (empty()) {
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

  void append(uint8_t byte)
  {
    if (empty() or tail->tailroom() == 0) {
      append_segment();
    }
    tail->append(byte);
    len++;
  }

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

  void clear()
  {
    head = nullptr;
    tail = nullptr;
  }

  bool empty() const { return length() == 0; }

  size_t length() const { return len; }

  bool operator==(const byte_buffer& other) const
  {
    auto it = begin(), it2 = other.begin();
    for (; it != end() and it2 != other.end(); ++it, ++it2) {
      if (*it != *it2) {
        return false;
      }
    }
    return (it == end()) and (it2 == other.end());
  }
  bool operator!=(const byte_buffer& other) const { return not(*this == other); }

  iterator       begin() { return iterator{head.get(), 0}; }
  const_iterator cbegin() const { return const_iterator{head.get(), 0}; }
  const_iterator begin() const { return const_iterator{head.get(), 0}; }
  iterator       end() { return iterator{nullptr, 0}; }
  const_iterator end() const { return const_iterator{nullptr, 0}; }
  const_iterator cend() const { return const_iterator{nullptr, 0}; }

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
  return not(buf == bytes);
}
inline bool operator!=(span<const uint8_t> bytes, const byte_buffer& buf)
{
  return not(buf == bytes);
}

class byte_buffer_view
{
public:
  using const_iterator = byte_buffer::const_iterator;

  byte_buffer_view() = default;
  byte_buffer_view(const_iterator it_begin_, const_iterator it_end_) : it(it_begin_), it_end(it_end_) {}
  byte_buffer_view(const byte_buffer& buffer) : it(buffer.begin()), it_end(buffer.end()) {}

  const_iterator begin() const { return it; }
  const_iterator end() const { return it_end; }

  const_iterator operator++() { return ++it; }

  bool empty() const { return it == it_end; }

  size_t length() const { return it_end - it; }

  const uint8_t& operator*() const { return *it; }

  byte_buffer_view slice(size_t offset, size_t size) const
  {
    srsran_sanity_check(offset + size <= length(), "Invalid slice dimensions.");
    return {it + offset, it + offset + size};
  }

  /// Split byte buffer view into two contiguous views, with break point defined by "offset".
  std::pair<byte_buffer_view, byte_buffer_view> split(size_t offset)
  {
    auto it_split = begin() + offset;
    return {{begin(), it_split}, {it_split, end()}};
  }

  bool operator==(const byte_buffer_view& other) const
  {
    return std::equal(begin(), end(), other.begin(), other.end());
  }

private:
  const_iterator it     = const_iterator{nullptr, 0};
  const_iterator it_end = const_iterator{nullptr, 0};
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