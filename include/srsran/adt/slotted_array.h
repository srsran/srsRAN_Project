/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "detail/slotted_array_detail.h"
#include "srsran/adt/tiny_optional.h"
#include "srsran/support/srsran_assert.h"
#include <array>

/// \file Definitions of slotted_array<T, N> used to manage containers with optional elements. All the defined classes
/// provide methods to access, remove, add elements by index with O(1) complexity, iterators that skip absent elements,
/// and track number of present elements.

namespace srsran {

namespace detail {

/// Iterator implementation for an array of optional types that automatically skips positions without a value.
template <typename ArrayOfOpts>
class slotted_array_iter_impl
{
  using iterator_type = slotted_array_iter_impl<ArrayOfOpts>;

public:
  using iterator_category = std::forward_iterator_tag;
  using value_type        = typename ArrayOfOpts::value_type::value_type;
  using difference_type   = std::ptrdiff_t;
  using reference         = std::conditional_t<std::is_const_v<ArrayOfOpts>, const value_type, value_type>&;
  using pointer           = std::conditional_t<std::is_const_v<ArrayOfOpts>, const value_type, value_type>*;

  slotted_array_iter_impl() = default;
  slotted_array_iter_impl(ArrayOfOpts& vec_, size_t idx_) : vec(&vec_), idx(idx_)
  {
    while (idx < vec->size() and not(*vec)[idx].has_value()) {
      ++idx;
    }
  }
  template <typename U, std::enable_if_t<not std::is_same_v<ArrayOfOpts, U>, int> = 0>
  slotted_array_iter_impl(const slotted_array_iter_impl<U>& other) : vec(other.vec), idx(other.idx)
  {
  }

  template <typename U, std::enable_if_t<not std::is_same_v<ArrayOfOpts, U>, int> = 0>
  slotted_array_iter_impl& operator=(const slotted_array_iter_impl<U>& other)
  {
    vec = other.vec;
    idx = other.idx;
    return *this;
  }

  reference operator*() { return (*vec)[idx].value(); }
  reference operator*() const { return (*vec)[idx].value(); }
  pointer   operator->() { return &(*vec)[idx].value(); }
  pointer   operator->() const { return &(*vec)[idx].value(); }

  iterator_type& operator++()
  {
    while (++idx < vec->size() and not(*vec)[idx].has_value()) {
    }
    return *this;
  }

  iterator_type& operator--()
  {
    while (--idx < vec->size() and not(*vec)[idx].has_value()) {
    }
    return *this;
  }

  template <typename U>
  bool operator==(const slotted_array_iter_impl<U>& other) const
  {
    return idx == other.idx and vec == other.vec;
  }
  template <typename U>
  bool operator!=(const slotted_array_iter_impl<U>& other) const
  {
    return not(*this == other);
  }

private:
  template <typename Vec>
  friend class base_slotted_array_view;
  template <typename U>
  friend class slotted_array_iter_impl;

  ArrayOfOpts* vec = nullptr;
  size_t       idx = std::numeric_limits<size_t>::max();
};

} // namespace detail

/// \brief Array of optional elements, with the following characteristics:
/// - Index-based lookup to elements,
/// - The iteration is in order of indexes and correctly skips absent elements,
/// - Stable - Pointer/References/Iterators remain valid throughout the object lifetime.
/// - Memory overhead - The container creates an array/vector up-front of size \c N, even if its number of elements is
/// still zero.
/// - Fragmentation - The container has holes in its internal data storage for every absent indexes. This reduces
/// the efficiency of operations like iteration in scenarios of high fragmentation.
/// The advantages of this container include:
/// - O(1) complexity for index-based lookup, creation, removal (just one array indexing),
/// - No allocations during element creation or deletion,
/// - Pointer stability of its elements.
/// - Index-based lookup is thread-safe if different threads use different indexes.
/// This container may be unsuitable for scenarios where \c N is large, due to the increased memory overhead and
/// potential fragmentation.
/// \tparam T type of the slotted_array elements.
/// \tparam N static size of the maximum number of elements.
/// \tparam EmbeddedStorage Whether the array is stored in the object (via std::array) or separately in the heap (via
/// std::vector).
template <typename T, size_t N, bool EmbeddedStorage = true>
class slotted_array
{
  using array_type =
      std::conditional_t<EmbeddedStorage, static_vector<tiny_optional<T>, N>, std::vector<tiny_optional<T>>>;

public:
  using value_type     = T;
  using iterator       = detail::slotted_array_iter_impl<array_type>;
  using const_iterator = detail::slotted_array_iter_impl<const array_type>;

  slotted_array()
  {
    if constexpr (not EmbeddedStorage) {
      this->vec.reserve(N);
    }
  }
  slotted_array(slotted_array&& other) noexcept : nof_elems(other.nof_elems), vec(std::move(other.vec))
  {
    other.clear();
  }
  slotted_array(const slotted_array& other) = default;
  slotted_array& operator=(slotted_array&& other) noexcept
  {
    if (this != &other) {
      nof_elems = other.nof_elems;
      vec       = std::move(other.vec);
      other.clear();
    }
    return *this;
  }
  slotted_array& operator=(const slotted_array& other) = default;

  [[nodiscard]] bool contains(size_t idx) const noexcept { return idx < vec.size() and vec[idx].has_value(); }

  /// \brief Index-based element lookup. This operation has O(1) complexity in the worst-case scenario.
  T& operator[](size_t idx) noexcept
  {
    srsran_assert(contains(idx), "Bad access for element with index={}", idx);
    return *vec[idx];
  }
  const T& operator[](size_t idx) const noexcept
  {
    srsran_assert(contains(idx), "Bad access for element with index={}", idx);
    return *vec[idx];
  }

  /// \brief Checks whether the container is empty.
  constexpr bool empty() const noexcept { return nof_elems == 0; }

  /// \brief Checks the number of elements stored in the container.
  constexpr size_t size() const noexcept { return nof_elems; }

  iterator       begin() { return iterator{vec, 0}; }
  iterator       end() { return iterator{vec, vec.size()}; }
  const_iterator begin() const { return const_iterator{vec, 0}; }
  const_iterator end() const { return const_iterator{vec, vec.size()}; }

  /// Insert element into the container if an element with the same index does not exist yet.
  /// \param idx Position of the constructed element in the array
  /// \param u object to insert
  /// \return true if insertion took place. False if an element with the same index already existed.
  template <typename U>
  bool insert(size_t idx, U&& u)
  {
    static_assert(std::is_convertible_v<U, value_type>, "Ctor T(U&&) does not exist.");
    srsran_assert(idx < N, "Index {} exceeds maximum size of slotted_array {}", idx, N);
    if (contains(idx)) {
      return false;
    }
    if (idx >= this->vec.size()) {
      this->vec.resize(idx + 1);
    }
    ++this->nof_elems;
    this->vec[idx] = std::forward<U>(u);
    return true;
  }

  /// Add or overwrite element with given index with a newly constructed object
  /// \param idx Position of the constructed element in the array
  /// \param args Arguments to pass to element ctor
  template <typename... Args>
  void emplace(size_t idx, Args&&... args)
  {
    static_assert(std::is_constructible_v<value_type, Args&&...>, "Ctor T(Args...) does not exist.");
    srsran_assert(idx < N, "Index {} exceeds maximum size of slotted_array {}", idx, N);
    if (idx >= this->vec.size()) {
      this->vec.resize(idx + 1);
    }
    this->nof_elems += this->contains(idx) ? 0 : 1;
    this->vec[idx].emplace(std::forward<Args>(args)...);
    srsran_assert(this->vec[idx].has_value(), "Inserted object must be represent an optional with value");
  }

  /// Erase object pointed by the given index
  /// \param idx Position of the erased element in the array
  bool erase(size_t idx) noexcept
  {
    if (this->contains(idx)) {
      --this->nof_elems;
      this->vec[idx].reset();
      clear_back_elems();
      return true;
    }
    return false;
  }

  /// Takes the element out of the container pointed by the given index
  /// \param idx Position of the erased element in the array
  T take(size_t idx) noexcept
  {
    srsran_assert(this->contains(idx), "Empty position in index {}", idx);

    --this->nof_elems;
    T obj = std::move(*this->vec[idx]);
    this->vec[idx].reset();
    clear_back_elems();
    return obj;
  }

  /// Erase object pointed by the given iterator. Iterator must point to valid element
  /// \param it container iterator
  void erase(iterator it) noexcept
  {
    bool ret = erase(this->extract_iterator_index(it));
    (void)ret;
    srsran_assert(ret, "Iterator must point to valid element");
  }

  /// Clear all elements of the container
  void clear() noexcept
  {
    this->nof_elems = 0;
    this->vec.clear();
  }

  /// Get iterator with index equal or higher than the provided index.
  iterator       lower_bound(size_t idx) { return iterator{vec, std::min(idx, vec.size())}; }
  const_iterator lower_bound(size_t idx) const { return const_iterator{vec, std::min(idx, vec.size())}; }

  /// Find first position that is empty
  [[nodiscard]] size_t find_first_empty(size_t start_guess = 0) const
  {
    if (start_guess >= N) {
      return N;
    }
    const size_t sz = vec.size();
    if (nof_elems == sz) {
      // there are no holes.
      return std::max(start_guess, sz);
    }
    for (size_t i = start_guess; i < sz; ++i) {
      if (not vec[i].has_value()) {
        return i;
      }
    }
    return std::max(start_guess, sz);
  }

private:
  template <typename It>
  static size_t extract_iterator_index(It it)
  {
    return it.idx;
  }

  void clear_back_elems()
  {
    while (vec.size() > 0 and not vec.back().has_value()) {
      vec.pop_back();
    }
  }

  size_t     nof_elems = 0; ///< Number of present fields
  array_type vec;           ///< Container to store optional elements
};

/// \brief Represents a slotted array that is indexed via an ID type that is convertible to an integer (e.g. enum).
template <typename IdType,
          typename T,
          size_t N,
          bool   EmbeddedStorage     = true,
          typename IdToIntConversion = detail::cast_to_size_operator<IdType>>
class slotted_id_table : private IdToIntConversion
{
  static_assert(
      std::is_convertible_v<decltype(std::declval<IdToIntConversion>().get_index(std::declval<IdType>())), size_t>,
      "IdType must be convertible to size_t");

public:
  using key_type       = IdType;
  using value_type     = typename slotted_array<T, N, EmbeddedStorage>::value_type;
  using iterator       = typename slotted_array<T, N, EmbeddedStorage>::iterator;
  using const_iterator = typename slotted_array<T, N, EmbeddedStorage>::const_iterator;

  using IdToIntConversion::IdToIntConversion;

  bool contains(key_type id) const noexcept { return sl_ar.contains(to_index(id)); }

  constexpr T&       operator[](key_type id) { return sl_ar[to_index(id)]; }
  constexpr const T& operator[](key_type id) const { return sl_ar[to_index(id)]; }

  constexpr bool   empty() const noexcept { return sl_ar.empty(); }
  constexpr size_t size() const { return sl_ar.size(); }

  iterator       begin() { return sl_ar.begin(); }
  iterator       end() { return sl_ar.end(); }
  const_iterator begin() const { return sl_ar.begin(); }
  const_iterator end() const { return sl_ar.end(); }

  /// \param id ID the constructed element in the table.
  /// \param u object to insert.
  /// \return true if insertion took place. False if an element with the same ID already existed.
  template <typename U>
  bool insert(key_type id, U&& u)
  {
    return sl_ar.insert(to_index(id), std::forward<U>(u));
  }

  /// Overwrite array element with given index with a newly constructed object
  /// \param id ID of the constructed element in the table
  /// \param args Arguments to pass to element ctor
  template <typename... Args>
  void emplace(key_type id, Args&&... args)
  {
    sl_ar.emplace(to_index(id), std::forward<Args>(args)...);
  }

  /// Erase object pointed by the given index
  /// \param id ID of the erased element in the table
  bool erase(key_type id) noexcept { return sl_ar.erase(to_index(id)); }

  /// Erase object pointed by the given iterator. Iterator must point to valid element
  /// \param it container iterator
  void erase(iterator it) noexcept { sl_ar.erase(it); }

  /// Takes the element out of the container pointed by the given index
  /// \param id ID of the taken element in the table
  T take(key_type id) noexcept { return sl_ar.take(id); }

  /// Clear all elements of the container
  void clear() noexcept { sl_ar.clear(); }

  /// Find first position that is empty
  key_type find_first_empty(key_type start_guess = 0) const
  {
    return IdToIntConversion::get_id(find_first_empty(to_index(start_guess)));
  }

  iterator       lower_bound(key_type id) { return sl_ar.lower_bound(to_index(id)); }
  const_iterator lower_bound(key_type id) const { return sl_ar.lower_bound(to_index(id)); }

private:
  constexpr size_t to_index(key_type k) const { return IdToIntConversion::get_index(k); }

  slotted_array<T, N, EmbeddedStorage> sl_ar;
};

} // namespace srsran
