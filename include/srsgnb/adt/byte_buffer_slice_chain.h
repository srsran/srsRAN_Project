/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_ADT_BYTE_BUFFER_SLICE_CHAIN_H
#define SRSGNB_ADT_BYTE_BUFFER_SLICE_CHAIN_H

#include "srsgnb/adt/byte_buffer.h"
#include <deque>

namespace srsgnb {

/// \brief This class represents the concatenation of several individual data packets, in which one of the packets
/// represents a header.
/// The class iterator type automatically traverses the concatenated data_packets in a byte by byte fashion.
class byte_buffer_slice_chain
{
  using fragment_container_t = std::deque<byte_buffer_slice>;

  template <typename T>
  class iterator_impl
  {
    using fragment_it_t = std::
        conditional_t<std::is_const<T>::value, fragment_container_t::const_iterator, fragment_container_t::iterator>;
    using underlying_it_t =
        std::conditional_t<std::is_const<T>::value, byte_buffer_view::const_iterator, byte_buffer_view::iterator>;
    using parent_t =
        std::conditional_t<std::is_const<T>::value, const byte_buffer_slice_chain, byte_buffer_slice_chain>;

  public:
    using iterator_type     = iterator_impl<T>;
    using value_type        = std::remove_const_t<T>;
    using reference         = T&;
    using pointer           = T*;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    iterator_impl() = default;
    iterator_impl(parent_t& parent_, fragment_it_t frag_it_, underlying_it_t it_) :
      parent(&parent_), frag_it(frag_it_), it(it_)
    {
      if (it == frag_it->end() and frag_it != parent->fragments.end()) {
        ++frag_it;
        it = frag_it->begin();
      }
    }

    reference operator*() { return *it; }
    reference operator*() const { return *it; }
    pointer   operator->() { return &(*it); }
    pointer   operator->() const { return &(*it); }

    iterator_type& operator++()
    {
      srsran_assert(frag_it != parent->fragments.end(), "Out-of-bound access");
      ++it;
      if (it == frag_it->end()) {
        ++frag_it;
        if (frag_it != parent->fragments.end()) {
          it = frag_it->begin();
        } else {
          it = underlying_it_t(nullptr, 0);
        }
      }
      return *this;
    }

    iterator_type operator++(int)
    {
      iterator_type ret_it(*this);
      ++(*this);
      return ret_it;
    }

    iterator_impl& operator+=(unsigned n)
    {
      while (n > 0) {
        unsigned remaining = frag_it->end() - it;
        if (remaining <= n) {
          n -= remaining;
          ++frag_it;
          srsran_assert(frag_it != parent->fragments.end(), "Operator+= out-of-bounds access");
          it = frag_it->begin();
        } else {
          it += n;
          n = 0;
        }
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
    fragment_it_t   frag_it;
    underlying_it_t it;
  };

public:
  using value_type     = uint8_t;
  using iterator       = iterator_impl<uint8_t>;
  using const_iterator = iterator_impl<const uint8_t>;

  byte_buffer_slice_chain() = default;
  explicit byte_buffer_slice_chain(byte_buffer buf_) { push_back(std::move(buf_)); }
  byte_buffer_slice_chain(byte_buffer_slice_chain&&) = default;
  byte_buffer_slice_chain(byte_buffer buf_, size_t start, size_t sz) { push_back(std::move(buf_), start, sz); }
  byte_buffer_slice_chain& operator=(byte_buffer_slice_chain&&) = default;

  void push_front(byte_buffer_slice slice)
  {
    if (slice.empty()) {
      return;
    }
    fragments.push_front(std::move(slice));
  }

  void push_front(byte_buffer buf) { push_front(byte_buffer_slice{std::move(buf)}); }

  void push_back(byte_buffer_slice buf)
  {
    if (buf.empty()) {
      return;
    }
    fragments.push_back(std::move(buf));
  }

  void push_back(byte_buffer buf) { push_back(byte_buffer_slice{std::move(buf)}); }

  void push_back(byte_buffer buf, size_t start, size_t sz) { push_back(byte_buffer_slice{std::move(buf), start, sz}); }

  void clear() { fragments.clear(); }

  bool empty() const { return fragments.empty(); }

  size_t length() const
  {
    size_t count = 0;
    for (const auto& f : fragments) {
      count += f.length();
    }
    return count;
  }

  const uint8_t& operator[](size_t i) const
  {
    size_t rem_pos = i;
    for (const byte_buffer_slice& v : fragments) {
      if (rem_pos < v.length()) {
        return v[rem_pos];
      }
      rem_pos -= v.length();
    }
    srsran_terminate("Out-of-bounds access ({} >= {})", i, length());
  }

  const fragment_container_t& slices() const { return fragments; }

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

  iterator begin()
  {
    return iterator{
        *this, fragments.begin(), empty() ? byte_buffer_view::iterator{nullptr, 0} : fragments.begin()->begin()};
  }
  const_iterator begin() const
  {
    return const_iterator{
        *this, fragments.begin(), empty() ? byte_buffer_view::const_iterator{nullptr, 0} : fragments.begin()->begin()};
  }
  iterator       end() { return iterator{*this, fragments.end(), byte_buffer_view::iterator{nullptr, 0}}; }
  const_iterator end() const
  {
    return const_iterator{*this, fragments.end(), byte_buffer_view::const_iterator{nullptr, 0}};
  }

private:
  /// List of data_packets that compose the current chain.
  // TODO: optimize the chain data structure.
  fragment_container_t fragments;
};

} // namespace srsgnb

namespace fmt {

/// \brief Custom formatter for byte_buffer_slice_chain.
template <>
struct formatter<srsgnb::byte_buffer_slice_chain> : public formatter<srsgnb::byte_buffer_view> {
  template <typename FormatContext>
  auto format(const srsgnb::byte_buffer_slice_chain& buf, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    if (mode == hexadecimal) {
      return format_to(ctx.out(), "{:0>2x}", fmt::join(buf.begin(), buf.end(), " "));
    }
    return format_to(ctx.out(), "{:0>8b}", fmt::join(buf.begin(), buf.end(), " "));
  }
};

} // namespace fmt

#endif // SRSGNB_ADT_BYTE_BUFFER_SLICE_CHAIN_H
