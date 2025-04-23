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

/// \file Definitions of slotted_vector<T> used to manage containers with optional elements. All the defined classes
/// provide methods to access, remove, add elements by index with O(1) complexity, iterators that skip absent elements,
/// and track number of present elements.

#pragma once

#include "detail/slotted_array_detail.h"
#include <vector>

namespace srsran {

namespace detail {

/// Iterator implementation for a vector of optional types that automatically skips positions without a value.
template <typename VectorData>
class slotted_vector_iter_impl
{
  using iterator_type = slotted_vector_iter_impl<VectorData>;
  using elem_type     = std::conditional_t<std::is_const_v<VectorData>,
                                           const typename VectorData::value_type,
                                           typename VectorData::value_type>;

public:
  using iterator_category = std::forward_iterator_tag;
  using value_type        = std::remove_const_t<elem_type>;
  using difference_type   = std::ptrdiff_t;
  using pointer           = elem_type*;
  using reference         = elem_type&;

  slotted_vector_iter_impl() = default;
  slotted_vector_iter_impl(VectorData& vec_, const std::vector<size_t>& index_mapper_, size_t idx_) noexcept :
    vec(&vec_), index_mapper(&index_mapper_), idx(idx_)
  {
    while (idx < index_mapper->size() and (*index_mapper)[idx] == std::numeric_limits<size_t>::max()) {
      ++idx;
    }
  }
  template <typename OtherVectorData, std::enable_if_t<not std::is_same<OtherVectorData, VectorData>::value, int> = 0>
  slotted_vector_iter_impl(const slotted_vector_iter_impl<OtherVectorData>& other) noexcept :
    vec(other.vec), index_mapper(other.index_mapper), idx(other.idx)
  {
  }

  template <typename OtherVectorData, std::enable_if_t<not std::is_same<OtherVectorData, VectorData>::value, int> = 0>
  slotted_vector_iter_impl& operator=(const slotted_vector_iter_impl<OtherVectorData>& other) noexcept
  {
    vec          = other.vec;
    index_mapper = other.index_mapper;
    idx          = other.idx;
    return *this;
  }

  reference     operator*() { return (*vec)[(*index_mapper)[idx]]; }
  reference     operator*() const { return (*vec)[(*index_mapper)[idx]]; }
  pointer       operator->() { return &(*vec)[(*index_mapper)[idx]]; }
  const pointer operator->() const { return &(*vec)[(*index_mapper)[idx]]; }

  iterator_type& operator++()
  {
    while (++idx < index_mapper->size() and (*index_mapper)[idx] == std::numeric_limits<size_t>::max()) {
    }
    return *this;
  }

  iterator_type& operator--()
  {
    while (--idx < index_mapper->size() and (*index_mapper)[idx] == std::numeric_limits<size_t>::max()) {
    }
    return *this;
  }

  template <typename U>
  bool operator==(const slotted_vector_iter_impl<U>& other) const
  {
    return idx == other.idx and vec == other.vec and index_mapper == other.index_mapper;
  }
  template <typename U>
  bool operator!=(const slotted_vector_iter_impl<U>& other) const
  {
    return not(*this == other);
  }

private:
  template <typename U>
  friend class slotted_vector_iter_impl;

  VectorData*                vec          = nullptr;
  const std::vector<size_t>* index_mapper = nullptr;
  size_t                     idx          = std::numeric_limits<size_t>::max();
};

} // namespace detail

/// \brief Container representing a vector of optional elements. It has the following characteristics:
/// - Index-based lookup of elements, based internally on two vector index lookups,
/// - Stable index - The same object never changes its ID during addition/removal of elements in the container,
/// - Unstable reference/pointer - Pointer/References do not remain valid during addition/removal of elements,
/// - Unstable iterator - Iterators do not remain valid during addition/removal of elements,
/// - Composed by two vectors, one "elements vector" for storing the container's elements and one "indexing vector" to
/// convert external indexes to an index position in the first "elements vector",
/// - The iteration is in order of indexes and correctly skips absent elements,
/// - Memory overhead - Reduced memory overhead compared to \c static_array<T, N> because elements are stored
/// contiguously in memory in the "elements vector". However, some memory overhead is expected in the "indexing vector",
/// - Fragmentation - There is only fragmentation in the "indexing vector".
/// The advantages of this container include:
/// - Less memory overhead and fragmentation than \c slotted_array<T, N>,
/// - O(1) complexity for index-based lookup, consisting of two vector index accesses,
/// - O(1) creation in the best case scenario via push_back. O(N) if the "elements vector" needs to be resized,
/// - O(1) removal via erase-remove pattern.
/// This container may be unsuitable for scenarios where indexes are very scattered, due to the increased memory
/// overhead and fragmentation, or for scenarios where pointer stability is required.
/// \tparam T type of the slotted_vector elements.
template <typename T>
class slotted_vector
{
  static constexpr std::size_t absent_value = std::numeric_limits<std::size_t>::max();

public:
  using value_type     = T;
  using iterator       = detail::slotted_vector_iter_impl<std::vector<T>>;
  using const_iterator = detail::slotted_vector_iter_impl<const std::vector<T>>;

  slotted_vector() = default;
  slotted_vector(slotted_vector&& other) noexcept :
    objects(std::move(other.objects)), index_mapper(std::move(other.index_mapper))
  {
  }
  slotted_vector(const slotted_vector& other)            = default;
  slotted_vector<T>& operator=(const slotted_vector<T>&) = default;
  slotted_vector<T>& operator=(slotted_vector<T>&& other) noexcept
  {
    objects      = std::move(other.objects);
    index_mapper = std::move(other.index_mapper);
    return *this;
  }

  /// Increase capacity of slotted_vector to avoid reallocations.
  /// \param[in] sz New capacity.
  void reserve(size_t sz)
  {
    objects.reserve(sz);
    index_mapper.reserve(sz);
  }

  /// Determines whether the container is empty.
  constexpr bool empty() const noexcept { return objects.empty(); }

  /// Determines the number of objects stored in the vector.
  constexpr size_t size() const noexcept { return objects.size(); }

  /// Determines whether the container contains an element with the provided index.
  bool contains(size_t idx) const { return idx < index_mapper.size() and index_mapper[idx] != absent_value; }

  /// Fetch element with provided index.
  T& operator[](size_t idx) noexcept
  {
    srsran_assert(contains(idx), "Bad access for element with index={}", idx);
    return objects[index_mapper[idx]];
  }

  /// Fetch element with provided index.
  const T& operator[](size_t idx) const noexcept
  {
    srsran_assert(contains(idx), "Bad access for element with index={}", idx);
    return objects[index_mapper[idx]];
  }

  /// \brief Inserts a new element in the container if the corresponding position was unoccupied. Overwrites an
  /// existing element if the corresponding position was occupied. May allocate and cause pointer invalidation.
  template <typename... Args>
  T& emplace(size_t idx, Args&&... args)
  {
    static_assert(std::is_constructible_v<value_type, Args&&...>, "Ctor T(Args...) does not exist.");
    if (contains(idx)) {
      auto& elem = objects[index_mapper[idx]];
      elem       = T(std::forward<Args>(args)...);
      return elem;
    }
    objects.emplace_back(std::forward<Args>(args)...);
    if (idx >= index_mapper.size()) {
      if (idx == index_mapper.size()) {
        // leverage std::vector growth policy, when elements are being inserted in order.
        index_mapper.push_back(static_cast<std::size_t>(absent_value));
      } else {
        index_mapper.resize(idx + 1, static_cast<std::size_t>(absent_value));
      }
    }
    index_mapper[idx] = objects.size() - 1;
    return objects.back();
  }

  /// Insert element into the container if an element with the same index does not exist yet.
  /// \param idx Position of the constructed element in the array
  /// \param u object to insert.
  /// \return true if insertion took place. False if an element with the same index already existed.
  template <typename U>
  bool insert(size_t idx, U&& u)
  {
    static_assert(std::is_convertible_v<U, value_type>, "Ctor T(U&&) does not exist.");
    if (contains(idx)) {
      return false;
    }
    emplace(idx, std::forward<U>(u));
    return true;
  }

  /// Erase object pointed by the given index.
  /// \return true if object existed. False, otherwise.
  bool erase(size_t idx)
  {
    if (not contains(idx)) {
      return false;
    }
    if (index_mapper[idx] < objects.size() - 1) {
      // If it is not the last element, swap with last element.
      std::swap(objects[index_mapper[idx]], objects.back());
      auto it = std::find(index_mapper.begin(), index_mapper.end(), objects.size() - 1U);
      srsran_assert(it != index_mapper.end(), "Container in invalid state");
      *it = index_mapper[idx];
    }
    objects.pop_back();
    index_mapper[idx] = absent_value;
    if (idx == index_mapper.size() - 1) {
      // Trim last positions in the index_mapper.
      while (not index_mapper.empty() and index_mapper.back() == absent_value) {
        index_mapper.pop_back();
      }
    }
    return true;
  }

  /// Erase object pointed by the given iterator. Iterator must point to valid element.
  /// \param it container iterator
  void erase(iterator it) noexcept { erase(extract_iterator_index(it)); }

  /// Clear all elements of the container, while maintaining its capacity.
  void clear() noexcept
  {
    objects.clear();
    index_mapper.clear();
  }

  /// Find first position that is empty in slotted_vector.
  size_t find_first_empty(size_t start_guess = 0) const
  {
    if (objects.size() == index_mapper.size()) {
      return index_mapper.size();
    }
    for (size_t i = start_guess; i < index_mapper.size(); ++i) {
      if (index_mapper[i] == absent_value) {
        return i;
      }
    }
    return objects.size();
  }

  /// Get list of objects stored in the container, not following a particular sorting order.
  span<const T> unsorted() const { return objects; }
  span<T>       unsorted() { return objects; }

  iterator       begin() { return iterator{objects, index_mapper, 0}; }
  iterator       end() { return iterator{objects, index_mapper, index_mapper.size()}; }
  const_iterator begin() const { return const_iterator{objects, index_mapper, 0}; }
  const_iterator end() const { return const_iterator{objects, index_mapper, index_mapper.size()}; }

  bool operator==(const slotted_vector& other) const { return std::equal(begin(), end(), other.begin(), other.end()); }
  bool operator!=(const slotted_vector& other) const { return not(*this == other); }

private:
  std::vector<T>      objects;
  std::vector<size_t> index_mapper;
};

/// \brief Represents a slotted vector that is indexed via an ID type that is convertible to an integer (e.g. enum).
template <typename IdType, typename T, typename IdToIntConversion = detail::cast_to_size_operator<IdType>>
class slotted_id_vector : private IdToIntConversion
{
  static_assert(
      std::is_convertible_v<decltype(std::declval<IdToIntConversion>().get_index(std::declval<IdType>())), std::size_t>,
      "IdType must be convertible to size_t");

public:
  using key_type       = IdType;
  using value_type     = typename slotted_vector<T>::value_type;
  using iterator       = typename slotted_vector<T>::iterator;
  using const_iterator = typename slotted_vector<T>::const_iterator;

  using IdToIntConversion::IdToIntConversion;

  bool contains(key_type id) const noexcept { return sl_vec.contains(to_index(id)); }

  constexpr T&       operator[](key_type id) { return sl_vec[to_index(id)]; }
  constexpr const T& operator[](key_type id) const { return sl_vec[to_index(id)]; }

  constexpr bool   empty() const noexcept { return sl_vec.empty(); }
  constexpr size_t size() const { return sl_vec.size(); }

  iterator       begin() { return sl_vec.begin(); }
  iterator       end() { return sl_vec.end(); }
  const_iterator begin() const { return sl_vec.begin(); }
  const_iterator end() const { return sl_vec.end(); }

  /// Increase capacity of slotted_id_vector to avoid reallocations.
  /// \param[in] sz New capacity.
  void reserve(size_t sz) { sl_vec.reserve(sz); }

  /// Insert element into the container if an element with the same ID does not exist yet.
  /// \param id ID the constructed element in the table.
  /// \param u object to insert.
  /// \return true if insertion took place. False if an element with the same ID already existed.
  template <typename U>
  bool insert(key_type id, U&& u)
  {
    return sl_vec.insert(to_index(id), std::forward<U>(u));
  }

  /// Insert or overwrite element with given ID with a newly constructed object.
  /// \param id ID of the constructed element in the table
  /// \param args Arguments to pass to element ctor
  template <typename... Args>
  T& emplace(key_type id, Args&&... args)
  {
    return sl_vec.emplace(to_index(id), std::forward<Args>(args)...);
  }

  /// Erase object pointed by the given index
  /// \param id ID of the erased element in the table
  bool erase(key_type id) noexcept { return sl_vec.erase(to_index(id)); }

  /// Erase object pointed by the given iterator. Iterator must point to valid element
  /// \param it container iterator
  void erase(iterator it) noexcept { sl_vec.erase(it); }

  /// Clear all elements of the container
  void clear() noexcept { sl_vec.clear(); }

  /// Get list of objects stored in the container, not following a particular sorting order.
  span<const T> unsorted() const { return sl_vec.unsorted(); }
  span<T>       unsorted() { return sl_vec.unsorted(); }

  /// Find first position that is empty
  key_type find_first_empty(key_type start_guess = 0) const
  {
    return IdToIntConversion::get_id(find_first_empty(to_index(start_guess)));
  }

  bool operator==(const slotted_id_vector& other) const { return sl_vec == other.sl_vec; }
  bool operator!=(const slotted_id_vector& other) const { return sl_vec != other.sl_vec; }

private:
  constexpr std::size_t to_index(key_type k) const { return IdToIntConversion::get_index(k); }

  slotted_vector<T> sl_vec;
};

} // namespace srsran
