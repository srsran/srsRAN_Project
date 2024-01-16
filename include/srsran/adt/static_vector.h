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

#include "srsran/support/srsran_assert.h"
#include <iterator>
#include <memory>

namespace srsran {

namespace detail {

template <typename T, size_t Capacity>
struct trivial_storage {
  static_assert(std::is_trivial<T>::value, "Implementation only works for trivial types");

  using array_type     = std::conditional_t<std::is_const<T>::value,
                                        const std::array<std::remove_const_t<T>, Capacity>,
                                        std::array<T, Capacity>>;
  using iterator       = T*;
  using const_iterator = const T*;

  constexpr trivial_storage() noexcept                                  = default;
  constexpr trivial_storage(const trivial_storage&) noexcept            = default;
  constexpr trivial_storage& operator=(const trivial_storage&) noexcept = default;
  constexpr trivial_storage(trivial_storage&&) noexcept                 = default;
  constexpr trivial_storage& operator=(trivial_storage&&) noexcept      = default;
  ~trivial_storage()                                                    = default;

  constexpr T*       data() noexcept { return array.data(); }
  constexpr const T* data() const noexcept { return array.data(); }
  constexpr T&       operator[](size_t i) noexcept
  {
    srsran_assert(i < sz, "out-of-bounds access ({} >= {})", i, sz);
    return array[i];
  }
  constexpr const T& operator[](size_t i) const noexcept
  {
    srsran_assert(i < sz, "out-of-bounds access ({} >= {})", i, sz);
    return array[i];
  }
  constexpr T&       front() noexcept { return (*this)[0]; }
  constexpr const T& front() const noexcept { return (*this)[0]; }
  constexpr T&       back() noexcept
  {
    srsran_assert(sz != 0, "back() called for empty vector");
    return array[sz - 1];
  }
  constexpr const T& back() const noexcept
  {
    srsran_assert(sz != 0, "back() called for empty vector");
    return array[sz - 1];
  }

  // Iterators
  constexpr iterator       begin() noexcept { return data(); }
  constexpr iterator       end() noexcept { return begin() + sz; }
  constexpr const_iterator begin() const noexcept { return data(); }
  constexpr const_iterator end() const noexcept { return begin() + sz; }

  constexpr void construct_(size_t /**/) noexcept
  {
    // do nothing.
  }

  template <typename U, typename... Args>
  constexpr void construct_(size_t idx, U&& arg0, Args&&... args) noexcept
  {
    static_assert(std::is_constructible<T, Args...>::value, "T(Args...) does not exist");
    array[idx] = T(std::forward<U>(arg0), std::forward<Args>(args)...);
  }

  constexpr void destroy_(size_t /**/) noexcept
  {
    // do nothing.
  }
  constexpr void destroy_(iterator it) noexcept
  {
    // do nothing.
  }

  array_type array;
  size_t     sz = 0;
};

template <typename T, size_t Capacity>
struct non_trivial_storage {
  static_assert(not std::is_trivial<T>::value, "Implementation only works for non-trivial types");

  using element_storage_t = std::aligned_storage_t<sizeof(T), alignof(T)>;
  using array_type        = std::conditional_t<std::is_const<T>::value,
                                        const std::array<element_storage_t, Capacity>,
                                        std::array<element_storage_t, Capacity>>;
  using iterator          = T*;
  using const_iterator    = const T*;

  constexpr non_trivial_storage() noexcept                                      = default;
  constexpr non_trivial_storage(const non_trivial_storage&) noexcept            = default;
  constexpr non_trivial_storage& operator=(const non_trivial_storage&) noexcept = default;
  constexpr non_trivial_storage(non_trivial_storage&&) noexcept                 = default;
  constexpr non_trivial_storage& operator=(non_trivial_storage&&) noexcept      = default;
  ~non_trivial_storage()
  {
    for (size_t i = 0; i != sz; ++i) {
      destroy_(i);
    }
  }

  T*       data() noexcept { return reinterpret_cast<T*>(array.data()); }
  const T* data() const noexcept { return reinterpret_cast<const T*>(array.data()); }
  T&       operator[](size_t i) noexcept
  {
    srsran_assert(i < sz, "out-of-bounds access ({} >= {})", i, sz);
    return *(data() + i);
  }
  const T& operator[](size_t i) const noexcept
  {
    srsran_assert(i < Capacity, "out-of-bounds access ({} >= {})", i, sz);
    return *(data() + i);
  }
  T&       front() noexcept { return (*this)[0]; }
  const T& front() const noexcept { return (*this)[0]; }
  T&       back() noexcept
  {
    srsran_assert(sz != 0, "back() called for empty vector");
    return *(data() + sz - 1);
  }
  const T& back() const noexcept
  {
    srsran_assert(sz != 0, "back() called for empty vector");
    return *(data() + sz - 1);
  }

  // Iterators
  iterator       begin() noexcept { return data(); }
  iterator       end() noexcept { return begin() + sz; }
  const_iterator begin() const noexcept { return data(); }
  const_iterator end() const noexcept { return begin() + sz; }

  template <typename... Args>
  constexpr void construct_(size_t idx, Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value)
  {
    static_assert(std::is_constructible<T, Args...>::value, "T(Args...) does not exist");
    new (&array[idx]) T(std::forward<Args>(args)...);
  }

  void destroy_(size_t idx) noexcept { (*this)[idx].~T(); }
  void destroy_(iterator it) noexcept { (*it).~T(); }

  array_type array;
  size_t     sz = 0;
};

template <typename T, size_t Capacity>
using static_vector_storage =
    std::conditional_t<std::is_trivial<T>::value, trivial_storage<T, Capacity>, non_trivial_storage<T, Capacity>>;

} // namespace detail

/// Random access, variable-sized container with embedded storage.
/// Contrarily to std::vector<T>, its maximum size is limited at compile-time, and it does not rely on allocators.
/// Contrarily to std::array, the elements are constructed and destructed dynamically when the container size() changes.
/// @tparam T type of the elements
/// @tparam MAX_N Maximum number of elements of the static_vector
template <typename T, std::size_t MAX_N>
class static_vector : private detail::static_vector_storage<T, MAX_N>
{
  using base_type = detail::static_vector_storage<T, MAX_N>;

public:
  using iterator        = T*;
  using const_iterator  = const T*;
  using size_type       = std::size_t;
  using value_type      = T;
  using difference_type = std::make_signed_t<size_type>;

  // inherited methods.
  using base_type::base_type;
  using base_type::data;
  using base_type::operator[];
  using base_type::back;
  using base_type::begin;
  using base_type::end;
  using base_type::front;

  /// \brief Construct an empty static_vector.
  ///
  /// Note: The ctor needs to be user-defined to forbid zero-initialization of the storage via = {}.
  constexpr static_vector() noexcept {}
  explicit constexpr static_vector(size_type count) noexcept(std::is_nothrow_default_constructible<T>::value)
  {
    static_assert(std::is_default_constructible<T>::value, "T must be default-constructible");
    srsran_assert(count <= MAX_N, "static vector maximum size={} was exceeded", MAX_N);
    // Note: Gcc 11 fails compilation without this hint. Potentially a bug.
    srsran_assume(count <= MAX_N);
    for (size_type i = 0; i != count; ++i) {
      this->construct_(i);
    }
    this->sz = count;
  }
  constexpr static_vector(size_type count,
                          const T&  initial_value) noexcept(std::is_nothrow_copy_constructible<T>::value)
  {
    static_assert(std::is_copy_constructible<T>::value, "T must be copy-constructible");
    assign_unsafe_(count, initial_value);
  }
  constexpr static_vector(const static_vector& other) noexcept(std::is_nothrow_copy_constructible<T>::value)
  {
    static_assert(std::is_copy_constructible<T>::value, "T must be copyable");
    assign_unsafe_(other.begin(), other.end());
  }
  constexpr static_vector(static_vector&& other) noexcept(std::is_nothrow_move_constructible<value_type>::value)
  {
    static_assert(std::is_move_constructible<T>::value, "T must be move-constructible");
    assign_unsafe_(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
    other.clear();
  }
  template <typename Iterator>
  constexpr static_vector(Iterator it_begin, Iterator it_end)
  {
    static_assert(std::is_constructible<T, decltype(*it_begin)>::value, "Cannot convert Iterator value_type to T");
    assign_unsafe_(it_begin, it_end);
  }
  constexpr static_vector(std::initializer_list<T> init) : static_vector(init.begin(), init.end()) {}
  ~static_vector() = default;
  constexpr static_vector& operator=(const static_vector& other) noexcept(std::is_nothrow_copy_assignable<T>::value)
  {
    if (this != &other) {
      assign(other.begin(), other.end());
    }
    return *this;
  }
  constexpr static_vector& operator=(static_vector&& other) noexcept(std::is_nothrow_move_assignable<value_type>::value)
  {
    size_t min_common_size = std::min(other.size(), size());
    if (min_common_size > 0) {
      // move already constructed elements
      auto it = std::move(other.begin(), other.begin() + min_common_size, begin());
      destroy_range_(it, end());
    } else {
      clear();
    }
    // append the rest
    std::uninitialized_copy(
        std::make_move_iterator(other.begin() + min_common_size), std::make_move_iterator(other.end()), end());
    this->sz = other.size();
    other.clear();
    return *this;
  }

  // Size/Capacity
  constexpr bool             empty() const noexcept { return this->sz == 0; }
  constexpr size_type        size() const noexcept { return this->sz; }
  constexpr bool             full() const noexcept { return this->sz == MAX_N; }
  static constexpr size_type capacity() noexcept { return MAX_N; }

  // modifiers
  template <typename... Args>
  constexpr T& emplace_back(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value)
  {
    srsran_assert(size() < MAX_N, "Cannot emplace back in full vector");
    this->construct_(size(), std::forward<Args>(args)...);
    this->sz++;
    return this->back();
  }

  constexpr void push_back(const T& value) noexcept(std::is_nothrow_copy_constructible<T>::value)
  {
    static_assert(std::is_copy_constructible<T>::value, "T must be copy-constructible");
    emplace_back(value);
  }

  constexpr void push_back(T&& value) noexcept(std::is_nothrow_move_constructible<T>::value)
  {
    static_assert(std::is_move_constructible<T>::value, "T must be move-constructible");
    emplace_back(std::move(value));
  }

  constexpr void pop_back() noexcept
  {
    srsran_assert(not empty(), "Trying to erase element from empty vector");
    this->destroy_(&back());
    this->sz--;
  }

  constexpr void clear() noexcept
  {
    destroy_range_(begin(), end());
    this->sz = 0;
  }

  constexpr void resize(size_t new_size) noexcept
  {
    static_assert(std::is_default_constructible<T>::value, "T must be default constructible");
    srsran_assert(new_size <= capacity(), "out-of-bounds construction in static_vector<T,N>");
    if (new_size <= size()) {
      // vector is shrinking.
      destroy_range_(begin() + new_size, end());
    } else {
      // vector is growing.
      for (size_t i = size(); i != new_size; ++i) {
        this->construct_(i);
      }
    }
    this->sz = new_size;
  }

  constexpr void resize(size_t new_size, const T& value) noexcept
  {
    static_assert(std::is_copy_constructible<T>::value, "T must be copy-constructible");
    srsran_assert(new_size <= capacity(), "out-of-bounds construction in static_vector<T,N>");
    if (new_size <= size()) {
      // vector is shrinking.
      destroy_range_(begin() + new_size, end());
    } else {
      // vector is growing.
      for (size_t i = size(); i != new_size; ++i) {
        this->construct_(i, value);
      }
    }
    this->sz = new_size;
  }

  constexpr iterator erase(iterator pos) noexcept(std::is_move_assignable<T>::value)
  {
    srsran_assert(pos >= this->begin() and pos < this->end(), "Iterator to erase is out-of-bounds");
    iterator ret = pos;
    std::move(pos + 1, end(), pos);
    pop_back();
    return ret;
  }

  constexpr iterator erase(iterator it_start, iterator it_end) noexcept(std::is_move_assignable<T>::value)
  {
    srsran_assert(it_start <= it_end, "Trying to erase invalid range");
    srsran_assert(it_start >= begin() and it_end <= end(), "Range to erase is out-of-bounds");

    iterator ret = it_start;
    // Shift all elements down.
    iterator new_end = std::move(it_end, end(), it_start);
    destroy_range_(new_end, end());
    this->sz = new_end - begin();
    return ret;
  }

  constexpr void assign(size_type nof_elems, const T& value) noexcept(std::is_nothrow_copy_constructible<T>::value)
  {
    clear();
    assign_unsafe_(nof_elems, value);
  }

  template <typename Iterator>
  constexpr void assign(Iterator it_start, Iterator it_end) noexcept(std::is_nothrow_copy_constructible<T>::value)
  {
    clear();
    assign_unsafe_(it_start, it_end);
  }

  constexpr void assign(std::initializer_list<T> ilist) noexcept(std::is_nothrow_copy_constructible<T>::value)
  {
    assign(ilist.begin(), ilist.end());
  }

  bool operator==(const static_vector& other) const noexcept
  {
    return std::equal(begin(), end(), other.begin(), other.end());
  }
  bool operator!=(const static_vector& other) const noexcept { return not(*this == other); }

  void swap(static_vector& other) noexcept
  {
    using std::swap;

    auto&        small_vec  = size() < other.size() ? *this : other;
    auto&        big_vec    = size() < other.size() ? other : *this;
    const size_t small_size = small_vec.size();
    const size_t big_size   = big_vec.size();

    for (size_t i = 0; i < small_size; ++i) {
      swap(small_vec[i], big_vec[i]);
    }
    for (size_t i = small_size; i < big_size; ++i) {
      small_vec.construct_(i, std::move(big_vec[i]));
      big_vec.destroy_(i);
    }
    small_vec.sz = big_size;
    big_vec.sz   = small_size;
  }

private:
  constexpr void destroy_range_(iterator it_start, iterator it_end) noexcept
  {
    for (auto it = it_start; it != it_end; ++it) {
      this->destroy_(it);
    }
  }
  constexpr void assign_unsafe_(size_t N, const T& val) noexcept(std::is_nothrow_copy_constructible<T>::value)
  {
    srsran_assert(N <= MAX_N, "static vector maximum size={} was exceeded", MAX_N);
    // Note: Gcc 11 fails compilation without this hint. Potentially a bug.
    srsran_assume(N <= MAX_N);
    for (size_t i = 0; i != N; ++i) {
      this->construct_(i, val);
    }
    this->sz = N;
  }
  template <typename Iterator>
  constexpr void assign_unsafe_(Iterator it_begin,
                                Iterator it_end) noexcept(std::is_nothrow_copy_constructible<T>::value)
  {
    size_type N = std::distance(it_begin, it_end);
    srsran_assert(N <= MAX_N, "static vector maximum size={} was exceeded", MAX_N);
    // Note: Gcc 11 fails compilation without this hint. Potentially a bug.
    srsran_assume(N <= MAX_N);
    for (size_t i = 0; i != N; ++i) {
      this->construct_(i, *it_begin);
      ++it_begin;
    }
    this->sz = N;
  }
};

} // namespace srsran
