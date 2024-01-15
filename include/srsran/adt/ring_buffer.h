/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/adt/detail/operations.h"
#include "srsran/adt/detail/type_storage.h"
#include "srsran/adt/expected.h"
#include "srsran/adt/span.h"
#include "srsran/support/srsran_assert.h"

#include <array>
#include <type_traits>
#include <vector>

namespace srsran {

namespace detail {

template <typename Container>
struct rebind_buffer_storage;

template <typename T>
struct rebind_buffer_storage<std::vector<T>> {
  using type = std::vector<type_storage<T>>;
};

template <typename T, size_t N>
struct rebind_buffer_storage<std::array<T, N>> {
  using type = std::array<type_storage<T>, N>;
};

template <typename Container, bool ForcePower2Size>
class ring_buffer_storage;

/// \brief Specialization of internal ring_buffer data storage for the case a std::array is used.
/// This specialization tries to leverage the fact that the buffer size is known at compile time. If \c N is a power
/// of 2, the compiler should be able to convert the % operation into a bit-wise and.
template <typename T, size_t N, bool ForcePower2Size>
class ring_buffer_storage<std::array<T, N>, ForcePower2Size>
{
  static_assert(not ForcePower2Size or ((N & (N - 1)) == 0), "N must be a power of 2 if ForcePower2Size is set");

public:
  constexpr ring_buffer_storage() = default;
  ring_buffer_storage(size_t /**/, unsigned rpos_, unsigned count_) : rpos(rpos_), count(count_) {}
  ring_buffer_storage(ring_buffer_storage&& other) noexcept :
    rpos(std::exchange(other.rpos, 0)), count(std::exchange(other.count, 0))
  {
    static_assert(std::is_move_constructible<T>::value, "T must be move-constructible");
    for (unsigned i = 0; i != count; ++i) {
      unsigned idx = advance_pos(rpos, i);
      data[idx].emplace(std::move(other.data[idx].get()));
      other.data[idx].destroy();
    }
  }
  ring_buffer_storage& operator=(ring_buffer_storage&& other) noexcept
  {
    for (unsigned i = 0; i != count; ++i) {
      data[advance_pos(rpos, i)].destroy();
    }
    rpos  = std::exchange(other.rpos, 0);
    count = std::exchange(other.count, 0);
    for (unsigned i = 0; i < count; ++i) {
      unsigned idx = advance_pos(rpos, i);
      data[idx].emplace(std::move(other.data[idx].get()));
      other.data[idx].destroy();
    }
    return *this;
  }

  static constexpr size_t max_size() noexcept { return N; }

  /// \brief Advance \c pos by \c n positions accounting for wrap-around.
  static unsigned advance_pos(unsigned pos, unsigned n) noexcept { return (pos + n) % max_size(); }

  void set_size(unsigned sz)
  {
    // do nothing.
  }

  unsigned         rpos  = 0; ///< Reading position
  unsigned         count = 0; ///< Number of elements in the buffer
  std::array<T, N> data;
};

/// \brief Specialization of internal ring_buffer data storage for the case a std::vector is used.
/// For this specialization, the compiler does not know the size of the buffer at compile time.
template <typename T>
class ring_buffer_storage<std::vector<T>, false>
{
public:
  explicit ring_buffer_storage(unsigned sz = 0) : data(sz) {}
  ring_buffer_storage(size_t max_size_, unsigned rpos_, unsigned count_) : rpos(rpos_), count(count_), data(max_size_)
  {
  }
  ring_buffer_storage(ring_buffer_storage&& other) noexcept :
    rpos(std::exchange(other.rpos, 0)), count(std::exchange(other.count, 0)), data(std::move(other.data))
  {
  }
  ring_buffer_storage& operator=(ring_buffer_storage&& other) noexcept
  {
    for (unsigned i = 0; i != count; ++i) {
      data[this->advance_pos(rpos, i)].destroy();
    }
    rpos  = std::exchange(other.rpos, 0);
    count = std::exchange(other.count, 0);
    data  = std::move(other.data);
    return *this;
  }

  void swap(ring_buffer_storage& other) noexcept
  {
    std::swap(rpos, other.rpos);
    std::swap(count, other.count);
    std::swap(data, other.data);
  }

  size_t max_size() const noexcept { return data.size(); }

  void set_size(unsigned sz)
  {
    srsran_assert(count == 0 or sz == data.size(), "Dynamic resizes not supported when ring buffer is not empty");
    data.resize(sz);
  }

  /// \brief Advance \c pos by \c n positions accounting for wrap-around.
  unsigned advance_pos(unsigned pos, unsigned n) const noexcept { return (pos + n) % max_size(); }

  unsigned       rpos  = 0; ///< Reading position
  unsigned       count = 0; ///< Number of elements in the buffer
  std::vector<T> data;      ///< Container where elements are stored
};

/// \brief Specialization of internal ring_buffer data storage for the case a std::vector is used.
/// For this specialization, the compiler does not know the size of the buffer at compile time, but it knows that it is
/// a power of 2. Thus, it is able to avoid the % operator, and uses instead a bit-wise and.
template <typename T>
class ring_buffer_storage<std::vector<T>, true> : private ring_buffer_storage<std::vector<T>, false>
{
  using super_type = ring_buffer_storage<std::vector<T>, false>;

  static unsigned to_next_pow2(unsigned sz) { return pow(2, ceil(log(sz) / log(2))); }

public:
  using super_type::count;
  using super_type::data;
  using super_type::max_size;
  using super_type::rpos;
  using super_type::super_type;
  using super_type::swap;
  explicit ring_buffer_storage(unsigned sz) : super_type(to_next_pow2(sz)) {}

  void set_size(unsigned sz)
  {
    // Round up to closest power of 2.
    super_type::set_size(to_next_pow2(sz));
  }

  /// \brief Advance \c pos by \c n positions accounting for wrap-around. Leverages the fact that max_size() is a power
  /// of 2.
  unsigned advance_pos(unsigned pos, unsigned n) const noexcept
  {
    // x mod y == x * (y - 1), when y is power of 2.
    return (pos + n) & (this->max_size() - 1);
  }
};

/// Base common class for definition of ring buffer data structures with the following features:
/// - no allocations while pushing/popping new elements. Just an internal index update
/// - it provides an api to push/pop elements like a queue
/// - it provides an iterator interface to iterate over added elements in the buffer
/// - not thread-safe.
/// \tparam Container underlying container type used as buffer (e.g. std::array<T, N> or std::vector<T>).
template <typename T, typename Container, bool ForcePower2Size>
class ring_buffer_impl
{
  static_assert(std::is_same<T, typename Container::value_type>::value, "Invalid container value_type");
  static_assert(std::is_same<Container, std::vector<T>>::value or is_std_array<Container>::value,
                "Invalid container type. Only std::vector and std::array are supported");

  constexpr static bool has_static_size = is_std_array<Container>::value;

  using raw_container    = typename detail::rebind_buffer_storage<Container>::type;
  using buffer_type      = ring_buffer_storage<raw_container, ForcePower2Size>;
  using ring_buffer_type = ring_buffer_impl<T, Container, ForcePower2Size>;

  /// Iterator implementation.
  /// \tparam DataType can be T or const T.
  template <typename DataType>
  class iterator_impl
  {
    using parent_type = std::conditional_t<std::is_same<DataType, T>::value, ring_buffer_type, const ring_buffer_type>;

    iterator_impl(parent_type& parent_, unsigned ring_idx_) : parent(&parent_), ring_idx(ring_idx_)
    {
      srsran_assert(ring_idx <= parent->size(), "Invalid iterator position");
    }

  public:
    using value_type        = std::remove_const_t<DataType>;
    using reference         = DataType&;
    using pointer           = DataType*;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    iterator_impl<DataType>& operator++()
    {
      inc_();
      return *this;
    }

    iterator_impl<DataType> operator++(int)
    {
      iterator_impl<DataType> tmp(*this);
      ++(*this);
      return tmp;
    }

    /// \brief Creates an iterator object that corresponds to the current iterator, incremented by \c n positions.
    iterator_impl<DataType> operator+(difference_type n)
    {
      iterator_impl<DataType> tmp(*this);
      tmp += n;
      return tmp;
    }

    /// \brief Increment iterator by \c n positions.
    iterator_impl<DataType>& operator+=(difference_type n)
    {
      inc_(n);
      return *this;
    }

    pointer   operator->() { return &get(); }
    pointer   operator->() const { return &get(); }
    reference operator*() { return get(); }
    reference operator*() const { return get(); }

    bool operator==(const iterator_impl<DataType>& it) const { return it.parent == parent and it.ring_idx == ring_idx; }
    bool operator!=(const iterator_impl<DataType>& it) const { return not(*this == it); }

    difference_type operator-(const iterator_impl<DataType>& other) const { return ring_idx - other.ring_idx; }

  private:
    friend class ring_buffer_impl<T, Container, ForcePower2Size>;

    void assert_idx_within_bounds()
    {
      srsran_assert(ring_idx < parent->size(),
                    "index={} is out-of-bounds [{}, {})",
                    ring_idx,
                    parent->buffer.rpos,
                    parent->size());
    }

    void inc_()
    {
      srsran_assert(*this != parent->end(), "Incrementing iterator beyond end()");
      ring_idx++;
    }

    void inc_(difference_type n)
    {
      if (n > 0) {
        srsran_assert(ring_idx + n <= parent->size(), "Incrementing iterator beyond end()");
      } else if (n < 0) {
        srsran_assert(ring_idx + n <= parent->size(), "Decrementing iterator beyond begin()");
      }
      ring_idx += n;
    }

    reference get()
    {
      assert_idx_within_bounds();
      return parent->buffer.data[parent->buffer.advance_pos(parent->buffer.rpos, ring_idx)].get();
    }
    reference get() const
    {
      assert_idx_within_bounds();
      return parent->buffer.data[parent->buffer.advance_pos(parent->buffer.rpos, ring_idx)].get();
    }

    parent_type* parent;
    unsigned     ring_idx; ///< When iterator is at begin(), ring_idx is zero.
  };

public:
  using index_type      = unsigned;
  using value_type      = std::remove_const_t<T>;
  using difference_type = typename Container::difference_type;
  using size_type       = unsigned;

  static constexpr bool max_size_is_power_of_2 = ForcePower2Size;

  using iterator       = iterator_impl<T>;
  using const_iterator = iterator_impl<const T>;

  constexpr ring_buffer_impl() = default;
  ~ring_buffer_impl() { clear(); }
  template <typename Dummy                                                                                  = T,
            std::enable_if_t<not ring_buffer_impl<Dummy, Container, ForcePower2Size>::has_static_size, int> = 0>
  explicit ring_buffer_impl(size_type sz) : buffer(sz)
  {
  }
  ring_buffer_impl(const ring_buffer_impl& other) noexcept :
    buffer(other.max_size(), other.buffer.rpos, other.buffer.count)
  {
    static_assert(std::is_copy_constructible<T>::value, "T must be copy-constructible");
    for (unsigned i = 0; i != buffer.count; ++i) {
      unsigned n = buffer.advance_pos(buffer.rpos, i);
      buffer.data[n].emplace(other.buffer.data[n].get());
    }
  }
  ring_buffer_impl(ring_buffer_impl&& other) noexcept = default;
  ring_buffer_impl& operator=(const ring_buffer_impl& other) noexcept
  {
    static_assert(std::is_copy_constructible<T>::value, "T must be copy-constructible");
    if (this != &other) {
      clear();
      buffer.set_size(other.buffer.data.size());
      buffer.rpos  = other.buffer.rpos;
      buffer.count = other.buffer.count;
      for (unsigned i = 0; i != buffer.count; ++i) {
        unsigned n = buffer.advance_pos(buffer.rpos, i);
        buffer.data[n].emplace(other.buffer.data[n].get());
      }
    }
    return *this;
  }
  ring_buffer_impl& operator=(ring_buffer_impl&& other) noexcept = default;

  /// \brief Checks the maximum number of elements that can be stored in the ring_buffer.
  constexpr size_t max_size() const noexcept { return buffer.max_size(); }

  /// \brief Checks if ring_buffer is full.
  bool full() const noexcept { return size() == max_size(); }

  /// \brief Checks if ring_buffer is empty.
  constexpr bool empty() const noexcept { return buffer.count == 0; }

  /// \brief Checks the number of elements currently stored in the ring_buffer.
  constexpr size_type size() const noexcept { return buffer.count; }

  template <typename Dummy                                                                                  = T,
            std::enable_if_t<not ring_buffer_impl<Dummy, Container, ForcePower2Size>::has_static_size, int> = 0>
  void set_size(size_type sz) noexcept
  {
    buffer.set_size(sz);
  }

  /// \brief Push an element to the ring buffer. Asserts if ring_buffer is full.
  template <typename U, std::enable_if_t<std::is_convertible<U, T>::value, int> = 0>
  void push(U&& u) noexcept
  {
    srsran_assert(not full(), "Circular buffer is full.");
    buffer.data[get_wpos()].emplace(std::forward<U>(u));
    buffer.count++;
  }

  /// \brief Push elements of a range into the ring buffer in a batch. Asserts if ring_buffer is full.
  template <typename It>
  void push(It b, It e) noexcept
  {
    static_assert(std::is_convertible<std::decay_t<decltype(*std::declval<It>())>, T>::value,
                  "Invalid type passed to ::push");
    for (auto it = b; it != e; ++it) {
      push(*it);
    }
  }

  /// \brief Push elements of a span into the ring buffer in a batch. Asserts if ring_buffer is full.
  void push(span<value_type> range) noexcept { push(range.begin(), range.end()); }

  /// \brief Tries to push an element to the circular buffer. If ring_buffer is full, returns false.
  template <typename U, std::enable_if_t<std::is_convertible<U, T>::value, int> = 0>
  bool try_push(U&& u) noexcept
  {
    if (full()) {
      return false;
    }
    push(std::forward<U>(u));
    return true;
  }

  /// \brief Pushes elements to the ring buffer in a batch. Stops when ring_buffer is full.
  /// \return Number of elements pushed.
  template <typename It>
  unsigned try_push(It b, It e) noexcept
  {
    static_assert(std::is_convertible<std::decay_t<decltype(*std::declval<It>())>, T>::value,
                  "Invalid type passed to ::push");
    unsigned count = 0;
    for (auto it = b; it != e; ++it) {
      if (not try_push(*it)) {
        break;
      }
      ++count;
    }
    return count;
  }

  /// \brief Pushes elements of a span into the ring buffer in a batch. Stops when ring_buffer is full.
  unsigned try_push(span<value_type> range) noexcept { return try_push(range.begin(), range.end()); }

  /// \brief Pops element from ring_buffer. If ring_buffer is empty, asserts.
  void pop()
  {
    srsran_assert(not empty(), "Cannot call pop() in empty circular buffer");
    buffer.data[buffer.rpos].destroy();
    buffer.rpos = buffer.advance_pos(buffer.rpos, 1);
    buffer.count--;
  }

  /// \brief Pops elements from ring_buffer into a range.
  /// \return Number of popped elements.
  template <typename It>
  unsigned pop_into(It b, It e)
  {
    unsigned orig_size = size();
    for (auto it = b; it != e; ++it) {
      if (empty()) {
        break;
      }
      *it = std::move(buffer.data[buffer.rpos].get());
      pop();
    }
    return orig_size - size();
  }

  /// \brief Pops elements from ring_buffer into a span.
  /// \return Number of popped elements.
  unsigned pop_into(span<value_type> range) { return pop_into(range.begin(), range.end()); }

  /// \brief Fetches the top position of ring_buffer. The function asserts if empty.
  T& top()
  {
    srsran_assert(not empty(), "Cannot call top() in empty circular buffer");
    return buffer.data[buffer.rpos].get();
  }
  const T& top() const
  {
    srsran_assert(not empty(), "Cannot call top() in empty circular buffer");
    return buffer.data[buffer.rpos].get();
  }

  /// \brief Clears the content of the ring_buffer.
  void clear()
  {
    for (unsigned i = 0; i != buffer.count; ++i) {
      buffer.data[buffer.advance_pos(buffer.rpos, i)].destroy();
    }
    buffer.rpos  = 0;
    buffer.count = 0;
  }

  /// \brief Random access to position of the ring_buffer.
  T& operator[](index_type i)
  {
    srsran_assert(i < size(), "Out-of-bounds access to circular buffer ({} >= {})", i, buffer.count);
    return buffer.data[buffer.advance_pos(buffer.rpos, i)].get();
  }
  const T& operator[](index_type i) const
  {
    srsran_assert(i < size(), "Out-of-bounds access to circular buffer ({} >= {})", i, buffer.count);
    return buffer.data[buffer.advance_pos(buffer.rpos, i)].get();
  }

  iterator       begin() { return iterator(*this, 0); }
  const_iterator begin() const { return const_iterator(*this, 0); }
  iterator       end() { return iterator(*this, buffer.count); }
  const_iterator end() const { return const_iterator(*this, buffer.count); }

  bool operator==(const ring_buffer_impl& other) const
  {
    return std::equal(begin(), end(), other.begin(), other.end());
  }
  bool operator!=(const ring_buffer_impl& other) const { return not(*this == other); }

  /// \brief Apply predicate callable to elements until the predicate returns true.
  /// \param func Predicate functor.
  /// \return true or false if an element was found for which funct(element) returned true.
  template <typename F>
  bool apply_first(const F& func)
  {
    for (auto it = begin(); it != end(); it++) {
      if (func(*it)) {
        return true;
      }
    }
    return false;
  }

protected:
  size_type get_wpos() const { return buffer.advance_pos(buffer.rpos, buffer.count); }

  buffer_type buffer;
};

} // namespace detail

/// Ring buffer with internal data storage via a std::vector<T>.
/// - size can be defined at construction or via set_size.
/// - not thread-safe.
/// \tparam T value type stored by buffer
/// \tparam RoundUpSizeToPowerOf2 Whether the ring buffer size gets round up to the next power of 2. When set to true,
/// the compiler is able to avoid the % operation and use instead the "bitwise-and" operation.
template <typename T, bool RoundUpSizeToPowerOf2 = true>
using ring_buffer = detail::ring_buffer_impl<T, std::vector<T>, RoundUpSizeToPowerOf2>;

/// \brief Ring buffer with fixed, embedded data storage via a std::array<T, N>. It can contain up to N elements.
/// - Given that the buffer size is known at compile-time, the circular iteration over the buffer may be more optimized
/// (e.g. when N is a power of 2, % operator can be avoided).
/// - not thread-safe.
/// \tparam T value type stored by buffer.
/// \tparam N Maximum capacity of the ring buffer.
template <typename T, size_t N>
using static_ring_buffer = detail::ring_buffer_impl<T, std::array<T, N>, false>;

} // namespace srsran
