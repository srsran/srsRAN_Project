/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_BUFFER_SEGMENT_H
#define SRSGNB_RLC_BUFFER_SEGMENT_H

#include "srsgnb/adt/byte_buffer.h"

namespace srsgnb {

class rlc_byte_buffer
{
  template <typename T>
  class iterator_impl
  {
    using underlying_it_t =
        std::conditional_t<std::is_const<T>::value, byte_buffer_view::const_iterator, byte_buffer_view::iterator>;
    using parent_t = std::conditional_t<std::is_const<T>::value, const rlc_byte_buffer, rlc_byte_buffer>;

  public:
    using iterator_type     = iterator_impl<T>;
    using value_type        = std::remove_const_t<T>;
    using reference         = T&;
    using pointer           = T*;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    iterator_impl() = default;
    iterator_impl(parent_t& parent_, bool in_header_, underlying_it_t it_) :
      parent(&parent_), it(it_), in_header(in_header_)
    {}

    reference operator*() { return *it; }
    reference operator*() const { return *it; }
    pointer   operator->() { return &(*it); }
    pointer   operator->() const { return &(*it); }

    iterator_type& operator++()
    {
      ++it;
      if (in_header and it == parent->header.end()) {
        it        = parent->payload.begin();
        in_header = false;
      }
      return *this;
    }

    iterator_type operator++(int)
    {
      iterator_type ret_it = it;
      ++ret_it;
      return ret_it;
    }

    iterator_impl& operator+=(difference_type n)
    {
      if (in_header) {
        difference_type remaining_header = parent->header.end() - it;
        it += std::min(remaining_header, n);
        n -= remaining_header;
        if (n >= 0) {
          it        = parent->payload.begin() + n;
          in_header = false;
        }
      } else {
        it += n;
      }
      return *this;
    }

    iterator_impl operator+(difference_type n) const
    {
      iterator_impl tmp(*this);
      tmp += n;
      return tmp;
    }

    bool operator==(const iterator_impl<T>& other) const { return it == other.it and parent == other.parent; }
    bool operator!=(const iterator_impl<T>& other) const { return !(*this == other); }

  private:
    parent_t*       parent = nullptr;
    underlying_it_t it;
    /// Used to signal whether iterator it points to rlc buffer header or payload.
    bool in_header;
  };

public:
  using value_type     = uint8_t;
  using iterator       = iterator_impl<uint8_t>;
  using const_iterator = iterator_impl<const uint8_t>;

  rlc_byte_buffer() = default;
  explicit rlc_byte_buffer(const byte_buffer& buf_) : payload(buf_) {}
  rlc_byte_buffer(const byte_buffer& buf_, size_t start, size_t sz) : payload(buf_, start, sz) {}

  void set_header(byte_buffer buf) { header = std::move(buf); }

  template <typename Iterator>
  void set_header(Iterator b, Iterator e)
  {
    header = byte_buffer{b, e};
  }

  void set_header(span<const uint8_t> bytes) { header = bytes; }

  void set_header(std::initializer_list<uint8_t> bytes) { header = bytes; }

  void set_payload(const byte_buffer& buf) { payload = byte_buffer_slice{buf}; }

  void set_payload(byte_buffer_slice buf) { payload = std::move(buf); }

  void set_payload(const byte_buffer& buf, size_t start, size_t sz) { payload = byte_buffer_slice{buf, start, sz}; }

  /// Prepends bytes to rlc buffer segment. This function may allocate new segments.
  void prepend_header(span<const uint8_t> bytes) { header.prepend(bytes); }

  rlc_byte_buffer& chain_before(byte_buffer before)
  {
    header.chain_before(std::move(before));
    return *this;
  }

  void clear()
  {
    header.clear();
    payload.clear();
  }

  bool empty() const { return header.empty() and payload.empty(); }

  size_t length() const { return header.length() + payload.length(); }

  const uint8_t& operator[](size_t i) const
  {
    if (i < header.length()) {
      return header[i];
    }
    return payload[i - header.length()];
  }

  byte_buffer_view payload_view() const { return byte_buffer_view{payload}; }
  byte_buffer_view header_view() const { return byte_buffer_view{header}; }

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

  iterator begin() { return iterator{*this, not header.empty(), header.empty() ? payload.begin() : header.begin()}; }
  const_iterator begin() const
  {
    return const_iterator{*this, not header.empty(), header.empty() ? payload.begin() : header.begin()};
  }
  iterator       end() { return iterator{*this, false, payload.end()}; }
  const_iterator end() const { return const_iterator{*this, false, payload.end()}; }

private:
  byte_buffer       header;
  byte_buffer_slice payload;
};

} // namespace srsgnb

#endif // SRSGNB_RLC_BUFFER_SEGMENT_H
