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
#include "srsran/adt/detail/byte_buffer_segment_pool.h"
#include "srsran/adt/span.h"
#include "srsran/support/memory_pool/memory_pool_utils.h"

namespace srsran {

/// \brief This class represents the concatenation of several individual data packets.
/// The class iterator type automatically traverses the concatenated data_packets in a byte by byte fashion.
class byte_buffer_chain
{
  template <typename DataType>
  class iter_impl
  {
    using buffer_it_t =
        std::conditional_t<std::is_const<DataType>::value, const byte_buffer_slice*, byte_buffer_slice*>;
    using buffer_offset_it_t = std::
        conditional_t<std::is_const<DataType>::value, byte_buffer_slice::const_iterator, byte_buffer_slice::iterator>;
    using parent_t = std::conditional_t<std::is_const<DataType>::value, const byte_buffer_chain, byte_buffer_chain>;

  public:
    using iterator_type     = iter_impl<DataType>;
    using value_type        = std::remove_const_t<DataType>;
    using reference         = DataType&;
    using pointer           = DataType*;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    iter_impl() = default;
    iter_impl(parent_t& parent_, buffer_it_t buffer_it_, buffer_offset_it_t it_) :
      parent(&parent_), buffer_it(buffer_it_), it(it_)
    {
      // Ensure we are not at the end of one of the byte buffers.
      if (buffer_it != parent->slices().end() and it == buffer_it->end()) {
        ++buffer_it;
        it = buffer_it->begin();
      }
    }

    /// Conversion between iterator types.
    template <typename U, std::enable_if_t<not std::is_same<U, DataType>::value, bool> = true>
    iter_impl(const iter_impl<U>& other) : parent(other.parent), buffer_it(other.buffer_it), it(other.it)
    {
    }

    reference operator*() { return *it; }
    reference operator*() const { return *it; }
    pointer   operator->() { return &(*it); }
    pointer   operator->() const { return &(*it); }

    iterator_type& operator++()
    {
      srsran_assert(buffer_it != parent->slices().end(), "Out-of-bound access");
      ++it;
      if (it == buffer_it->end()) {
        ++buffer_it;
        if (buffer_it != parent->slices().end()) {
          it = buffer_it->begin();
        } else {
          it = buffer_offset_it_t(nullptr, 0);
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

    iter_impl& operator+=(unsigned n)
    {
      while (n > 0) {
        unsigned remaining = buffer_it->end() - it;
        if (remaining <= n) {
          n -= remaining;
          srsran_assert(buffer_it != parent->slices().end(), "Out-of-bound access");
          ++buffer_it;
          if (buffer_it != parent->slices().end()) {
            it = buffer_it->begin();
          } else {
            it = {nullptr, 0};
          }
        } else {
          it += n;
          n = 0;
        }
      }
      return *this;
    }

    iter_impl operator+(difference_type n) const
    {
      iter_impl tmp(*this);
      tmp += n;
      return tmp;
    }

    bool operator==(const iter_impl& other) const { return it == other.it and parent == other.parent; }
    bool operator!=(const iter_impl& other) const { return !(*this == other); }

  private:
    template <typename OtherT>
    friend class iter_impl;

    parent_t*          parent = nullptr;
    buffer_it_t        buffer_it;
    buffer_offset_it_t it;
  };

public:
  using value_type      = uint8_t;
  using reference       = uint8_t&;
  using pointer         = uint8_t*;
  using size_type       = size_t;
  using difference_type = std::ptrdiff_t;

  using iterator       = iter_impl<uint8_t>;
  using const_iterator = iter_impl<const uint8_t>;

  /// \brief Creates an empty byte_buffer_chain.
  byte_buffer_chain()
  {
    // Allocate memory block from pool for the array of slices.
    mem_block.reset(detail::get_default_byte_buffer_segment_pool().allocate_node());
    if (mem_block == nullptr) {
      srslog::fetch_basic_logger("ALL").warning("POOL: Failed to allocate memory block for byte_buffer_chain");
      return;
    }

    // Initialize slice array in the allocated memory block.
    size_t max_nof_slices =
        detail::get_default_byte_buffer_segment_pool().memory_block_size() / sizeof(byte_buffer_slice);
    byte_buffer_slice* start = static_cast<byte_buffer_slice*>(align_next(mem_block.get(), alignof(byte_buffer_slice)));
    for (unsigned i = 0; i != max_nof_slices; ++i) {
      new (start + i) byte_buffer_slice();
    }
    slice_storage = {start, max_nof_slices};
  }

  ~byte_buffer_chain()
  {
    // Destroy slice array.
    for (auto& slice : slice_storage) {
      slice.~byte_buffer_slice();
    }
  }

  /// Default move constructor.
  byte_buffer_chain(byte_buffer_chain&& other) noexcept :
    byte_count(std::exchange(other.byte_count, 0)),
    slice_count(std::exchange(other.slice_count, 0)),
    mem_block(std::move(other.mem_block))
  {
    slice_storage       = other.slice_storage;
    other.slice_storage = {};
  }

  byte_buffer_chain(const byte_buffer_chain&) = delete;

  /// Conversion from byte_buffer to byte_buffer_chain.
  explicit byte_buffer_chain(byte_buffer buf_) : byte_buffer_chain() { append(std::move(buf_)); }

  /// Conversion from byte_buffer_slice to byte_buffer_chain.
  byte_buffer_chain(byte_buffer_slice&& buf_) : byte_buffer_chain() { append(std::move(buf_)); }

  /// Conversion from byte_buffer with specified offset and size to byte_buffer_chain.
  byte_buffer_chain(byte_buffer buf_, size_t start, size_t sz) :
    byte_buffer_chain(byte_buffer_slice(std::move(buf_), start, sz))
  {
  }

  /// Default move assignment operator.
  byte_buffer_chain& operator=(byte_buffer_chain&& other) noexcept
  {
    byte_count  = std::exchange(other.byte_count, 0);
    slice_count = std::exchange(other.slice_count, 0);
    std::swap(mem_block, other.mem_block);
    std::swap(slice_storage, other.slice_storage);
    return *this;
  }

  /// No copy assignment operator.
  byte_buffer_chain& operator=(const byte_buffer_chain&) noexcept = delete;

  /// Performs a deep copy of this byte_buffer_chain into a byte_buffer.
  byte_buffer deep_copy() const
  {
    byte_buffer buf;
    for (const byte_buffer_slice& slice : slices()) {
      buf.append(slice);
    }
    return buf;
  }

  /// If the byte_buffer_chain failed to be allocated, this will return false.
  bool valid() const { return not slice_storage.empty(); }

  /// Checks whether the byte_buffer_chain can append/prepend more byte_buffers.
  bool full() const { return nof_slices() == max_nof_slices(); }

  /// Checks whether the byte_buffer_chain is empty.
  bool empty() const { return not valid() or nof_slices() == 0; }

  /// Returns the total length of the byte_buffer_chain in bytes.
  size_t length() const { return byte_count; }

  /// Returns the number of byte_buffer_slices in the byte_buffer_chain.
  size_t nof_slices() const { return slice_count; }

  /// Returns the maximum number of byte_buffer_slices that the byte_buffer_chain can hold.
  size_t max_nof_slices() const { return slice_storage.size(); }

  /// Appends a byte_buffer_slice to the end of the byte_buffer_chain.
  ///
  /// \param obj Slice to append to the byte_buffer_chain.
  /// \return true if operation was successful, false otherwise.
  bool append(byte_buffer_slice obj) noexcept
  {
    if (obj.empty()) {
      return true;
    }
    if (not valid() or full()) {
      return false;
    }
    byte_count += obj.length();
    slice_storage[slice_count++] = std::move(obj);
    return true;
  }

  /// Appends a byte_buffer to the end of the byte_buffer_chain.
  /// \return true if operation was successful, false otherwise.
  bool append(byte_buffer buf) { return append(byte_buffer_slice{std::move(buf)}); }

  /// Appends the contents of another byte_buffer_chain to the end of this byte_buffer_chain.
  /// \return true if operation was successful, false otherwise.
  bool append(byte_buffer_chain&& other)
  {
    if (nof_slices() + other.nof_slices() > max_nof_slices()) {
      return false;
    }
    for (unsigned i = 0, end = other.nof_slices(); i != end; ++i) {
      slice_storage[slice_count + i] = std::move(other.slice_storage[i]);
    }
    byte_count += other.byte_count;
    slice_count += other.slice_count;
    other.slice_count = 0;
    other.byte_count  = 0;
    return true;
  }

  /// Prepends a byte_buffer_slice to the beginning of the byte_buffer_chain. This operation has O(N) complexity.
  /// \return true if operation was successful, false otherwise.
  bool prepend(byte_buffer_slice slice)
  {
    if (slice.empty()) {
      return true;
    }
    if (not valid() or full()) {
      return false;
    }
    for (size_t i = slice_count; i > 0; --i) {
      slice_storage[i] = std::move(slice_storage[i - 1]);
    }
    byte_count += slice.length();
    slice_count++;
    slice_storage[0] = std::move(slice);
    return true;
  }

  /// Prepends a byte_buffer to the beginning of the byte_buffer_chain.
  /// \return true if operation was successful, false otherwise.
  bool prepend(byte_buffer buf) { return prepend(byte_buffer_slice{std::move(buf)}); }

  /// Release all the byte buffer slices held by the byte_buffer_chain.
  void clear()
  {
    for (unsigned i = 0; i != nof_slices(); ++i) {
      slice_storage[i] = {};
    }
    slice_count = 0;
    byte_count  = 0;
  }

  /// Access to one byte of the byte_buffer_chain by a given index.
  const uint8_t& operator[](size_t idx) const
  {
    size_t                              rem_pos = idx;
    const span<const byte_buffer_slice> slices  = this->slices();
    for (const byte_buffer_slice& s : slices) {
      if (rem_pos < s.length()) {
        return s[rem_pos];
      }
      rem_pos -= s.length();
    }
    srsran_assertion_failure("Out-of-bounds access ({} >= {})", idx, length());
    return slice_storage[0][0];
  }

  /// Returns the slices of the byte_buffer_chain.
  span<const byte_buffer_slice> slices() const
  {
    return span<const byte_buffer_slice>{slice_storage.begin(), slice_storage.begin() + slice_count};
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

  /// Returns an iterator to the begin of the byte_buffer_chain.
  iterator begin()
  {
    return iterator{*this,
                    slice_storage.begin(),
                    empty() ? byte_buffer_view::iterator{nullptr, 0} : slice_storage.begin()->begin()};
  }
  const_iterator begin() const
  {
    return const_iterator{*this,
                          slice_storage.begin(),
                          empty() ? byte_buffer_view::iterator{nullptr, 0} : slice_storage.begin()->begin()};
  }

  /// Returns an iterator to the end of the byte_buffer_chain.
  iterator end()
  {
    return iterator{*this, slice_storage.begin() + slice_count, byte_buffer_view::iterator{nullptr, 0}};
  }
  const_iterator end() const
  {
    return const_iterator{*this, slice_storage.begin() + slice_count, byte_buffer_view::iterator{nullptr, 0}};
  }

private:
  struct block_deleter {
    void operator()(void* p)
    {
      if (p != nullptr) {
        detail::byte_buffer_segment_pool::get_instance().deallocate_node(p);
      }
    }
  };

  // Total number of bytes stored in this container.
  size_t byte_count = 0;
  // Total number of byte_buffer_slices stored in this container.
  size_t slice_count = 0;
  // Memory block managed by a memory pool, where the slices are stored.
  std::unique_ptr<void, block_deleter> mem_block;
  // Array where byte_buffer_slices are stored. This array is a view to the \c mem_block.
  span<byte_buffer_slice> slice_storage;
};

} // namespace srsran

namespace fmt {
/// \brief Custom formatter for byte_buffer_chain.
template <>
struct formatter<srsran::byte_buffer_chain> : public formatter<srsran::byte_buffer_view> {
  template <typename FormatContext>
  auto format(const srsran::byte_buffer_chain& buf, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    if (mode == hexadecimal) {
      return format_to(ctx.out(), "{:0>2x}", fmt::join(buf.begin(), buf.end(), " "));
    }
    return format_to(ctx.out(), "{:0>8b}", fmt::join(buf.begin(), buf.end(), " "));
  }
};

} // namespace fmt
