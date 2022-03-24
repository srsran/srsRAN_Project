#ifndef SRSGNB_ADT_BYTE_BUFFER_H
#define SRSGNB_ADT_BYTE_BUFFER_H

#include "srsgnb/adt/span.h"
#include "srsgnb/support/srsran_assert.h"
#include <array>
#include <cstdint>
#include <vector>

namespace srsgnb {

class buffer_segment
{
public:
  constexpr static size_t SEGMENT_SIZE     = 256;
  constexpr static size_t DEFAULT_HEADROOM = 16;

  struct metadata_storage {
    std::shared_ptr<buffer_segment> next = nullptr;
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

  metadata_storage&       metadata() { return metadata_; }
  const metadata_storage& metadata() const { return metadata_; }

  const uint8_t* data() const { return buffer.data(); }
  uint8_t*       data() { return buffer.data(); }

  bool operator==(const buffer_segment& other) const
  {
    return std::equal(buffer.begin() + headroom(),
                      buffer.begin() + headroom() + length(),
                      other.buffer.begin() + other.headroom(),
                      other.buffer.begin() + other.headroom() + other.length());
  }
  bool operator!=(const buffer_segment& other) { return not(*this == other); }

private:
  size_t tailroom_start() const { return headroom() + length(); }

  metadata_storage                  metadata_;
  std::array<uint8_t, SEGMENT_SIZE> buffer;
  size_t                            headroom_      = DEFAULT_HEADROOM;
  size_t                            payload_length = 0;
};

/// Type to store a dynamic amount of contiguous bytes.
class byte_buffer
{
  template <typename T>
  struct iterator_impl {
    using value_type        = std::conditional_t<std::is_const<T>::value, const uint8_t, uint8_t>;
    using reference         = value_type&;
    using pointer           = value_type*;
    using iterator_category = std::forward_iterator_tag;

    iterator_impl(T* start_segment, size_t offset_) : current_segment(start_segment), offset(offset_) {}

    value_type& operator*() { return *(current_segment->data() + current_segment->headroom() + offset); }
    value_type* operator->() { return (current_segment->data() + current_segment->headroom() + offset); }

    iterator_impl& operator++()
    {
      offset++;
      if (offset >= current_segment->length()) {
        offset          = 0;
        current_segment = current_segment->metadata().next.get();
      }
      return *this;
    }

    bool operator==(const iterator_impl<T>& other) const
    {
      return current_segment == other.current_segment and offset == other.offset;
    }
    bool operator!=(const iterator_impl<T>& other) const { return not(*this == other); }

    T*     current_segment;
    size_t offset;
  };

public:
  using iterator       = iterator_impl<buffer_segment>;
  using const_iterator = iterator_impl<const buffer_segment>;

  byte_buffer()                       = default;
  byte_buffer(byte_buffer&&) noexcept = default;
  byte_buffer& operator=(byte_buffer&&) noexcept = default;

  byte_buffer clone()
  {
    byte_buffer buf;
    for (buffer_segment* seg = head.get(); seg != nullptr; seg = seg->metadata().next.get()) {
      buf.append(span<uint8_t>{seg->data() + seg->headroom(), seg->length()});
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
      span<const uint8_t> subspan  = bytes.subspan(0, to_write);
      tail->append(subspan);
      count += to_write;
    }
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
      span<const uint8_t> subspan  = bytes.subspan(bytes.size() - to_write, bytes.size());
      head->prepend(subspan);
      count += to_write;
    }
  }

  bool empty() const { return head == nullptr; }

  size_t length() const
  {
    size_t sz = 0;
    for (buffer_segment* seg = head.get(); seg != nullptr; seg = seg->metadata().next.get()) {
      sz += seg->length();
    }
    return sz;
  }

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
  iterator       end() { return iterator{nullptr, 0}; }
  const_iterator begin() const { return const_iterator{head.get(), 0}; }
  const_iterator end() const { return const_iterator{nullptr, 0}; }

private:
  byte_buffer(const byte_buffer&) = default;
  byte_buffer& operator=(const byte_buffer&) = default;

  void append_segment()
  {
    // TODO: Use memory pool.
    // TODO: Verify if allocation was successful. What to do if not?
    auto buf = std::make_shared<buffer_segment>();
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
    auto buf = std::make_shared<buffer_segment>();
    if (empty()) {
      head = buf;
      tail = buf.get();
    } else {
      buf->metadata().next = head;
      head                 = buf;
    }
  }

  // TODO: Optimize. shared_ptr<> has a lot of boilerplate we don't need.
  std::shared_ptr<buffer_segment> head = nullptr;
  buffer_segment*                 tail = nullptr;
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
bool operator!=(span<const uint8_t> bytes, const byte_buffer& buf)
{
  return not(buf == bytes);
}

} // namespace srsgnb

#endif // SRSGNB_ADT_BYTE_BUFFER_H