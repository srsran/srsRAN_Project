/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"
#include <deque>

namespace srsran {

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
      if (frag_it != parent->fragments.end() and it == frag_it->end()) {
        ++frag_it;
        it = frag_it->begin();
      }
    }
    template <typename U, std::enable_if_t<not std::is_same<U, T>::value, bool> = true>
    iterator_impl(const iterator_impl<U>& other) : parent(other.parent), frag_it(other.frag_it), it(other.it)
    {
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
    template <typename OtherT>
    friend class iterator_impl;

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
  byte_buffer_slice_chain(byte_buffer_slice&& buf_) { push_back(std::move(buf_)); }
  byte_buffer_slice_chain(byte_buffer_slice_chain&&) = default;
  byte_buffer_slice_chain(byte_buffer buf_, size_t start, size_t sz) { push_back(std::move(buf_), start, sz); }
  byte_buffer_slice_chain& operator=(byte_buffer_slice_chain&&) = default;

  /// Performs a deep copy of this byte_buffer_slice_chain.
  byte_buffer deep_copy() const
  {
    byte_buffer buf;
    for (const byte_buffer_slice& slice : slices()) {
      buf.append(slice);
    }
    return buf;
  }

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
    srsran_assertion_failure("Out-of-bounds access ({} >= {})", i, length());
    return (*fragments.begin())[0];
  }

  const fragment_container_t& slices() const { return fragments; }

  void chain_after(byte_buffer_slice_chain&& other)
  {
    while (not other.fragments.empty()) {
      fragments.push_back(std::move(other.fragments.front()));
      other.fragments.pop_front();
    }
  }

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

} // namespace srsran

namespace fmt {

/// \brief Custom formatter for byte_buffer_slice_chain.
template <>
struct formatter<srsran::byte_buffer_slice_chain> : public formatter<srsran::byte_buffer_view> {
  template <typename FormatContext>
  auto format(const srsran::byte_buffer_slice_chain& buf, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    if (mode == hexadecimal) {
      return format_to(ctx.out(), "{:0>2x}", fmt::join(buf.begin(), buf.end(), " "));
    }
    return format_to(ctx.out(), "{:0>8b}", fmt::join(buf.begin(), buf.end(), " "));
  }
};

} // namespace fmt
