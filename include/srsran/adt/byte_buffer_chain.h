/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
      if (buffer_it != parent->main_block->end() and it == buffer_it->end()) {
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
      srsran_assert(buffer_it != parent->main_block->end(), "Out-of-bound access");
      ++it;
      if (it == buffer_it->end()) {
        ++buffer_it;
        if (buffer_it != parent->main_block->end()) {
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
          ++buffer_it;
          srsran_assert(buffer_it != parent->main_block->end(), "Operator+= out-of-bounds access");
          it = buffer_it->begin();
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
    void* block = detail::get_default_byte_buffer_segment_pool().allocate_node();
    if (block == nullptr) {
      srslog::fetch_basic_logger("ALL").warning("POOL: Failed to allocate memory block for byte_buffer_chain");
      return;
    }
    main_block.reset(new (block) master_block());
  }

  /// Default move constructor.
  byte_buffer_chain(byte_buffer_chain&&) noexcept = default;

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
  byte_buffer_chain& operator=(byte_buffer_chain&&) noexcept = default;

  /// No copy assignment operator.
  byte_buffer_chain& operator=(const byte_buffer_chain&) noexcept = delete;

  /// Performs a deep copy of this byte_buffer_chain.
  byte_buffer deep_copy() const
  {
    byte_buffer buf;
    for (const byte_buffer_slice& slice : slices()) {
      buf.append(slice);
    }
    return buf;
  }

  /// If the byte_buffer_chain failed to be allocated, this will return false.
  bool valid() const { return main_block != nullptr; }

  /// Checks whether the byte_buffer_chain can append/prepend more byte_buffers.
  bool full() const { return nof_slices() == max_nof_slices(); }

  /// Checks whether the byte_buffer_chain is empty.
  bool empty() const { return not valid() or main_block->nof_slices() == 0; }

  /// Returns the total length of the byte_buffer_chain in bytes.
  size_t length() const
  {
    if (not valid()) {
      return 0;
    }
    size_t len = 0;
    for (unsigned i = 0; i != main_block->nof_slices(); ++i) {
      len += main_block->slices[i].length();
    }
    return len;
  }

  /// Returns the number of byte_buffer_slices in the byte_buffer_chain.
  size_t nof_slices() const { return main_block->nof_slices(); }

  /// Returns the maximum number of byte_buffer_slices that the byte_buffer_chain can hold.
  size_t max_nof_slices() const { return main_block->max_nof_slices(); }

  /// Appends a byte_buffer_slice to the end of the byte_buffer_chain.
  void append(byte_buffer_slice obj) noexcept
  {
    srsran_assert(valid() and not full(), "append() called on full container");
    if (obj.empty()) {
      return;
    }
    main_block->slices[main_block->size++] = std::move(obj);
  }

  /// Appends a byte_buffer to the end of the byte_buffer_chain.
  void append(byte_buffer buf) { append(byte_buffer_slice{std::move(buf)}); }

  /// Appends the contents of another byte_buffer_chain to the end of this byte_buffer_chain.
  void append(byte_buffer_chain&& other)
  {
    srsran_assert(valid() and other.valid(), "append() called on an invalid container");
    srsran_assert(nof_slices() + other.nof_slices() <= max_nof_slices(),
                  "append() would exceed container max capacity");
    for (unsigned i = 0, end = other.nof_slices(); i != end; ++i) {
      main_block->slices[main_block->size + i] = std::move(other.main_block->slices[i]);
    }
    main_block->size += other.nof_slices();
    other.main_block->size = 0;
  }

  /// Prepends a byte_buffer_slice to the beginning of the byte_buffer_chain.
  void prepend(byte_buffer_slice slice)
  {
    srsran_assert(valid() and not full(), "prepend() called on full container");
    if (slice.empty()) {
      return;
    }
    for (size_t i = main_block->size; i > 0; --i) {
      main_block->slices[i] = std::move(main_block->slices[i - 1]);
    }
    main_block->slices[0] = std::move(slice);
    main_block->size++;
  }

  /// Prepends a byte_buffer to the beginning of the byte_buffer_chain.
  void prepend(byte_buffer buf) { prepend(byte_buffer_slice{std::move(buf)}); }

  /// Release all the byte buffer slices held by the byte_buffer_chain.
  void clear()
  {
    if (main_block != nullptr) {
      for (unsigned i = 0; i != main_block->nof_slices(); ++i) {
        main_block->slices[i] = {};
      }
      main_block->size = 0;
    }
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
    return (*main_block->begin())[0];
  }

  /// Returns the slices of the byte_buffer_chain.
  span<const byte_buffer_slice> slices() const
  {
    return span<const byte_buffer_slice>{main_block->begin(), main_block->end()};
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
    return iterator{
        *this, main_block->begin(), empty() ? byte_buffer_view::iterator{nullptr, 0} : main_block->begin()->begin()};
  }
  const_iterator begin() const
  {
    return const_iterator{
        *this, main_block->begin(), empty() ? byte_buffer_view::iterator{nullptr, 0} : main_block->begin()->begin()};
  }

  /// Returns an iterator to the end of the byte_buffer_chain.
  iterator       end() { return iterator{*this, main_block->end(), byte_buffer_view::iterator{nullptr, 0}}; }
  const_iterator end() const
  {
    return const_iterator{*this, main_block->end(), byte_buffer_view::iterator{nullptr, 0}};
  }

private:
  struct master_block {
    span<byte_buffer_slice> slices;
    size_t                  size = 0;

    master_block()
    {
      char* data_start =
          static_cast<char*>(align_next(advance_ptr(this, sizeof(master_block)), alignof(byte_buffer_slice)));
      char* data_end =
          static_cast<char*>(advance_ptr(this, detail::byte_buffer_segment_pool::get_instance().memory_block_size()));
      const size_t vec_sz = (data_end - data_start) / sizeof(byte_buffer_slice);
      srsran_assert(vec_sz > 0, "Memory block size is too small to fit a single byte_buffer_slice");
      byte_buffer_slice* first_elem = new (data_start) byte_buffer_slice{};
      for (unsigned i = 1; i != vec_sz; ++i) {
        new (data_start + i * sizeof(byte_buffer_slice)) byte_buffer_slice{};
      }
      slices = span<byte_buffer_slice>{first_elem, vec_sz};
    }
    master_block(const master_block&)            = delete;
    master_block(master_block&&)                 = delete;
    master_block& operator=(const master_block&) = delete;
    master_block& operator=(master_block&&)      = delete;
    ~master_block()
    {
      for (unsigned i = 0; i != slices.size(); ++i) {
        slices[i].~byte_buffer_slice();
      }
    }

    SRSRAN_FORCE_INLINE byte_buffer_slice*       begin() { return slices.begin(); }
    SRSRAN_FORCE_INLINE const byte_buffer_slice* begin() const { return slices.begin(); }
    SRSRAN_FORCE_INLINE byte_buffer_slice*       end() { return slices.begin() + size; }
    SRSRAN_FORCE_INLINE const byte_buffer_slice* end() const { return slices.begin() + size; }

    SRSRAN_FORCE_INLINE size_t nof_slices() const { return size; }
    SRSRAN_FORCE_INLINE size_t max_nof_slices() const { return slices.size(); }
  };

  struct block_deleter {
    void operator()(master_block* p)
    {
      if (p != nullptr) {
        detail::byte_buffer_segment_pool::get_instance().deallocate_node(p);
      }
    }
  };

  std::unique_ptr<master_block, block_deleter> main_block;
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
