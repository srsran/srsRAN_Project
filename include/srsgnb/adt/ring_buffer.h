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

#include "srsgnb/adt/detail/operator.h"
#include "srsgnb/adt/detail/type_storage.h"
#include "srsgnb/adt/expected.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/support/srsgnb_assert.h"

#include <array>
#include <type_traits>
#include <vector>

namespace srsgnb {

namespace detail {

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
  ring_buffer_storage(size_t /**/, size_t rpos_, size_t count_) : rpos(rpos_), count(count_) {}
  ring_buffer_storage(ring_buffer_storage&& other) noexcept :
    rpos(std::exchange(other.rpos, 0)), count(std::exchange(other.count, 0))
  {
    static_assert(std::is_move_constructible<T>::value, "T must be move-constructible");
    for (size_t i = 0; i != count; ++i) {
      size_t idx = advance_pos_impl(rpos, i);
      buffer[idx].emplace(std::move(other.buffer[idx].get()));
      other.buffer[idx].destroy();
    }
  }
  ring_buffer_storage& operator=(ring_buffer_storage&& other) noexcept
  {
    for (size_t i = 0; i != count; ++i) {
      buffer[advance_pos_impl(rpos, i)].destroy();
    }
    rpos  = std::exchange(other.rpos, 0);
    count = std::exchange(other.count, 0);
    for (size_t i = 0; i < count; ++i) {
      size_t idx = advance_pos_impl(rpos, i);
      buffer[idx].emplace(std::move(other.buffer[idx].get()));
      other.buffer[idx].destroy();
    }
    return *this;
  }

  static constexpr size_t max_size() noexcept { return N; }

  static unsigned advance_pos_impl(unsigned pos, unsigned n) noexcept { return (pos + n) % max_size(); }

  void set_size(size_t sz)
  {
    // do nothing.
  }

  size_t           rpos  = 0; ///< Reading position
  size_t           count = 0; ///< Number of elements in the buffer
  std::array<T, N> buffer;
};

/// \brief Specialization of internal ring_buffer data storage for the case a std::vector is used.
/// For this specialization, the compiler does not know the size of the buffer at compile time.
template <typename T>
class ring_buffer_storage<std::vector<T>, false>
{
public:
  explicit ring_buffer_storage(size_t sz = 0) : buffer(sz) {}
  ring_buffer_storage(size_t max_size_, size_t rpos_, size_t count_) : rpos(rpos_), count(count_), buffer(max_size_) {}
  ring_buffer_storage(ring_buffer_storage&& other) noexcept :
    rpos(std::exchange(other.rpos, 0)), count(std::exchange(other.count, 0)), buffer(std::move(other.buffer))
  {
  }
  ring_buffer_storage& operator=(ring_buffer_storage&& other) noexcept
  {
    for (unsigned i = 0; i != count; ++i) {
      buffer[this->advance_pos_impl(rpos, i)].destroy();
    }
    rpos   = std::exchange(other.rpos, 0);
    count  = std::exchange(other.count, 0);
    buffer = std::move(other.buffer);
    return *this;
  }

  void swap(ring_buffer_storage& other) noexcept
  {
    std::swap(rpos, other.rpos);
    std::swap(count, other.count);
    std::swap(buffer, other.buffer);
  }

  size_t max_size() const noexcept { return buffer.size(); }

  void set_size(size_t sz)
  {
    srsgnb_assert(count == 0 or sz == buffer.size(), "Dynamic resizes not supported when circular buffer is not empty");
    buffer.resize(sz);
  }

  unsigned advance_pos_impl(unsigned pos, unsigned n) const noexcept { return (pos + n) % max_size(); }

  size_t         rpos  = 0; ///< Reading position
  size_t         count = 0; ///< Number of elements in the buffer
  std::vector<T> buffer;    ///< Container where elements are stored
};

/// \brief Specialization of internal ring_buffer data storage for the case a std::vector is used.
/// For this specialization, the compiler does not know the size of the buffer at compile time, but it knows that it is
/// a power of 2. Thus, it is able to avoid the % operator, and uses instead a bit-wise and.
template <typename T>
class ring_buffer_storage<std::vector<T>, true> : private ring_buffer_storage<std::vector<T>, false>
{
  using super_type = ring_buffer_storage<std::vector<T>, false>;

  static size_t to_next_pow2(size_t sz) { return pow(2, ceil(log(sz) / log(2))); }

public:
  using super_type::buffer;
  using super_type::count;
  using super_type::max_size;
  using super_type::rpos;
  using super_type::super_type;
  using super_type::swap;
  explicit ring_buffer_storage(size_t sz) : super_type(to_next_pow2(sz)) {}

  void set_size(size_t sz)
  {
    // Round up to closest power of 2.
    super_type::set_size(to_next_pow2(sz));
  }

  unsigned advance_pos_impl(unsigned pos, unsigned n) const noexcept
  {
    // x mod y == x * (y - 1), when y is power of 2.
    return (pos + n) & (this->max_size() - 1);
  }
};

/// Base common class for definition of circular buffer data structures with the following features:
/// - no allocations while pushing/popping new elements. Just an internal index update
/// - it provides helper methods to add/remove objects
/// - it provides an iterator interface to iterate over added elements in the buffer
/// - not thread-safe.
/// \tparam Container underlying container type used as buffer (e.g. std::array<T, N> or std::vector<T>).
template <typename Container, bool ForcePower2Size>
class base_ring_buffer
{
  using buffer_type = ring_buffer_storage<Container, ForcePower2Size>;
  using T           = typename Container::value_type::value_type;

  /// Iterator implementation.
  /// \tparam DataType can be T or const T.
  template <typename DataType>
  class iterator_impl
  {
    using parent_type = std::conditional_t<std::is_same<DataType, T>::value,
                                           base_ring_buffer<Container, ForcePower2Size>,
                                           const base_ring_buffer<Container, ForcePower2Size>>;

    iterator_impl(parent_type& parent_, size_t ring_idx_) : parent(&parent_), ring_idx(ring_idx_)
    {
      srsgnb_assert(ring_idx <= parent->size(), "Invalid iterator position");
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
    friend class base_ring_buffer<Container, ForcePower2Size>;

    void assert_idx_within_bounds()
    {
      srsgnb_assert(ring_idx < parent->size(),
                    "index={} is out-of-bounds [{}, {})",
                    ring_idx,
                    parent->buffer.rpos,
                    parent->size());
    }

    void inc_()
    {
      srsgnb_assert(*this != parent->end(), "Incrementing iterator beyond end()");
      ring_idx++;
    }

    void inc_(difference_type n)
    {
      if (n > 0) {
        srsgnb_assert(ring_idx + n <= parent->size(), "Incrementing iterator beyond end()");
      } else if (n < 0) {
        srsgnb_assert(ring_idx + n <= parent->size(), "Decrementing iterator beyond begin()");
      }
      ring_idx += n;
    }

    reference get()
    {
      assert_idx_within_bounds();
      return parent->buffer.buffer[parent->buffer.advance_pos_impl(parent->buffer.rpos, ring_idx)].get();
    }
    reference get() const
    {
      assert_idx_within_bounds();
      return parent->buffer.buffer[parent->buffer.advance_pos_impl(parent->buffer.rpos, ring_idx)].get();
    }

    parent_type* parent;
    unsigned     ring_idx; ///< When iterator is at begin(), ring_idx is zero.
  };

public:
  using index_type      = std::size_t;
  using value_type      = std::remove_const_t<T>;
  using difference_type = typename Container::difference_type;
  using size_type       = std::size_t;

  static constexpr bool max_size_is_power_of_2 = ForcePower2Size;

  using iterator       = iterator_impl<T>;
  using const_iterator = iterator_impl<const T>;

  constexpr base_ring_buffer() = default;
  ~base_ring_buffer() { clear(); }
  explicit base_ring_buffer(size_type sz) : buffer(sz) {}
  base_ring_buffer(const base_ring_buffer& other) noexcept :
    buffer(other.max_size(), other.buffer.rpos, other.buffer.count)
  {
    static_assert(std::is_copy_constructible<T>::value, "T must be copy-constructible");
    for (unsigned i = 0; i != buffer.count; ++i) {
      unsigned n = buffer.advance_pos_impl(buffer.rpos, i);
      buffer.buffer[n].emplace(other.buffer.buffer[n].get());
    }
  }
  base_ring_buffer(base_ring_buffer&& other) noexcept = default;
  base_ring_buffer& operator=(const base_ring_buffer& other) noexcept
  {
    static_assert(std::is_copy_constructible<T>::value, "T must be copy-constructible");
    if (this != &other) {
      clear();
      buffer.set_size(other.buffer.buffer.size());
      buffer.rpos  = other.buffer.rpos;
      buffer.count = other.buffer.count;
      for (unsigned i = 0; i != buffer.count; ++i) {
        unsigned n = buffer.advance_pos_impl(buffer.rpos, i);
        buffer.buffer[n].emplace(other.buffer.buffer[n].get());
      }
    }
    return *this;
  }
  base_ring_buffer& operator=(base_ring_buffer&& other) noexcept = default;

  /// \brief Checks the maximum number of elements that can be stored in the ring_buffer.
  constexpr size_t max_size() const noexcept { return buffer.max_size(); }

  /// \brief Checks if ring_buffer is full.
  bool full() const noexcept { return size() == max_size(); }

  /// \brief Checks if ring_buffer is empty.
  constexpr bool empty() const noexcept { return buffer.count == 0; }

  /// \brief Checks the number of elements currently stored in the ring_buffer.
  constexpr size_t size() const noexcept { return buffer.count; }

  /// \brief Push an element to the ring buffer. Asserts if ring_buffer is full.
  template <typename U, std::enable_if_t<std::is_convertible<U, T>::value, int> = 0>
  void push(U&& u) noexcept
  {
    srsgnb_assert(not full(), "Circular buffer is full.");
    buffer.buffer[get_wpos()].emplace(std::forward<U>(u));
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
    srsgnb_assert(not empty(), "Cannot call pop() in empty circular buffer");
    buffer.buffer[buffer.rpos].destroy();
    buffer.rpos = buffer.advance_pos_impl(buffer.rpos, 1);
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
      *it = std::move(buffer.buffer[buffer.rpos].get());
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
    srsgnb_assert(not empty(), "Cannot call top() in empty circular buffer");
    return buffer.buffer[buffer.rpos].get();
  }
  const T& top() const
  {
    srsgnb_assert(not empty(), "Cannot call top() in empty circular buffer");
    return buffer.buffer[buffer.rpos].get();
  }

  /// \brief Clears the content of the ring_buffer.
  void clear()
  {
    for (unsigned i = 0; i != buffer.count; ++i) {
      buffer.buffer[buffer.advance_pos_impl(buffer.rpos, i)].destroy();
    }
    buffer.rpos  = 0;
    buffer.count = 0;
  }

  /// \brief Random access to position of the ring_buffer.
  T& operator[](index_type i)
  {
    srsgnb_assert(i < size(), "Out-of-bounds access to circular buffer ({} >= {})", i, buffer.count);
    return buffer.buffer[buffer.advance_pos_impl(buffer.rpos, i)].get();
  }
  const T& operator[](index_type i) const
  {
    srsgnb_assert(i < size(), "Out-of-bounds access to circular buffer ({} >= {})", i, buffer.count);
    return buffer.buffer[buffer.advance_pos_impl(buffer.rpos, i)].get();
  }

  iterator       begin() { return iterator(*this, 0); }
  const_iterator begin() const { return const_iterator(*this, 0); }
  iterator       end() { return iterator(*this, buffer.count); }
  const_iterator end() const { return const_iterator(*this, buffer.count); }

  bool operator==(const base_ring_buffer& other) const
  {
    return std::equal(begin(), end(), other.begin(), other.end());
  }
  bool operator!=(const base_ring_buffer& other) const { return not(*this == other); }

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
  size_t get_wpos() const { return buffer.advance_pos_impl(buffer.rpos, buffer.count); }

  buffer_type buffer;
};

} // namespace detail

/// \brief Ring buffer with fixed, embedded data storage via a std::array<T, N>. It can contain up to N elements.
/// - Given that the buffer size is known at compile-time, the circular iteration over the buffer may be more optimized
/// (e.g. when N is a power of 2, % operator can be avoided).
/// - not thread-safe.
/// \tparam T value type stored by buffer.
/// \tparam N Maximum capacity of the ring buffer.
template <typename T, size_t N>
class static_ring_buffer : public detail::base_ring_buffer<std::array<detail::type_storage<T>, N>, false>
{
  using super_type = detail::base_ring_buffer<std::array<detail::type_storage<T>, N>, false>;

public:
  using super_type::super_type;
};

/// Ring buffer with storage via a std::vector<T>.
/// - size can be defined at start-time.
/// - not thread-safe.
/// \tparam T value type stored by buffer
/// \tparam ForcePower2Size Whether the ring buffer size gets round up to the next power of 2.
template <typename T, bool ForcePower2Size = false>
class ring_buffer : public detail::base_ring_buffer<std::vector<detail::type_storage<T>>, ForcePower2Size>
{
  using super_type = detail::base_ring_buffer<std::vector<detail::type_storage<T>>, ForcePower2Size>;

public:
  using super_type::super_type;

  void set_size(size_t sz) { this->buffer.set_size(sz); }
};

} // namespace srsgnb
