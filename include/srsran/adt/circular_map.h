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

#include "srsran/adt/detail/type_traits.h"
#include "srsran/adt/expected.h"
#include "srsran/support/srsran_assert.h"
#include <array>
#include <optional>
#include <vector>

namespace srsran {

namespace circular_map_traits {

template <typename Container, typename T>
struct rebind_map_storage;

template <typename T, size_t N, typename U>
struct rebind_map_storage<std::array<T, N>, U> {
  using type = std::array<U, N>;
};

template <typename T, typename U>
struct rebind_map_storage<std::vector<T>, U> {
  using type = std::vector<U>;
};

} // namespace circular_map_traits

namespace detail {

template <typename Container, bool ForcePower2Size>
struct circular_map_storage;

/// \brief Specialization of the internal data storage for the case an std::array is used.
///
/// This specialization tries to leverage the fact that the buffer size is known at compile time. If \c N is a power of
/// 2, the compiler should be able to convert the % operation into a bit-wise and operation.
template <typename T, size_t N, bool ForcePower2Size>
struct circular_map_storage<std::array<T, N>, ForcePower2Size> {
  constexpr circular_map_storage() = default;

  void clear()
  {
    // std::fill() and friends forward the 'fill object' by const reference, making non-copyable types to not compile.
    // Instead, use a rvalue for assignment.
    for (auto& elem : data) {
      elem = std::nullopt;
    }
    count = 0;
  }

  constexpr size_t get_circular_idx(size_t idx) const { return idx % N; }

  static constexpr size_t max_size() { return N; }

  std::array<T, N> data;
  size_t           count = 0;
};

/// \brief Specialization of the internal data storage for the case an std::vector is used.
///
/// For this specialization, the compiler does not know the size of the buffer at compile time.
template <typename T>
struct circular_map_storage<std::vector<T>, false> {
  constexpr circular_map_storage() = default;

  void clear()
  {
    // Restore size for a potentially moved from vector to bring it back to a known state.
    data.resize(capacity);
    // vector::assign() forward the 'fill object' by const reference, making non-copyable types to not compile. Instead,
    // use a rvalue for assignment.
    for (auto& elem : data) {
      elem = std::nullopt;
    }
    count = 0;
  }

  void set_size(size_t size)
  {
    srsran_assert(data.size() == 0 or size == data.size(),
                  "Dynamic resizes not supported when circular map is not empty");
    data.resize(size);
    capacity = size;
  }

  constexpr size_t get_circular_idx(size_t idx) const { return idx % max_size(); }

  constexpr size_t max_size() const { return data.size(); }

  std::vector<T> data;
  size_t         capacity = 0;
  size_t         count    = 0;
};

/// \brief Specialization of the internal data storage for the case an std::vector is used.
///
/// For this specialization, the compiler does not know the size of the buffer at compile time, but it knows that it is
/// a power of 2. Thus, it is able to avoid the % operator, and uses instead a bit-wise and operation.
template <typename T>
struct circular_map_storage<std::vector<T>, true> : public circular_map_storage<std::vector<T>, false> {
  using super_type = circular_map_storage<std::vector<T>, false>;

  using super_type::clear;
  using super_type::max_size;

  void set_size(size_t size)
  {
    report_fatal_error_if_not(
        (size & (size - 1)) == 0, "Size should be a power of 2, but a value of '{}' was used", size);
    super_type::set_size(size);
  }

  constexpr size_t get_circular_idx(size_t idx) const { return idx & (max_size() - 1); }
};

} // namespace detail

/// \brief Contiguous array of optional objects indexed circularly via a integer key. Random access, insertion and
/// removal should have complexity O(1).
///
/// Each container element contains 3 fields: (i) presence flag, (ii) key, (iii) stored object.
/// In case the presence flag is set to true, the stored object exists and its constructor has been called.
/// The following steps are performed to access an element:
/// 1. The provided key K is converted to an array index via the operation I = K % N.
/// 2. The presence flag for the index I is checked. If present,
/// 3. Check if the provided key K corresponds to the key stored in the element. If the keys match,
/// 4. Retrieve the stored object.
/// The container does not resolve collisions, i.e. it does not allow two objects of different keys but same index to be
/// inserted in the container. It is up to the user to resolve such situations. There is no pointer and iterator
/// invalidation.
/// \tparam K               Type of key used to retrieve an element index. K must be an unsigned integer.
/// \tparam V               Type of each element.
/// \tparam Container       Internal container storage type.
/// \tparam ForcePower2Size If true, the container capacity must be a power of 2 which has the benefit of using a
///                         bit-wise and operation instead of a modulus operation for indexing the elements.
template <typename K, typename V, typename Container, bool ForcePower2Size>
class circular_map_impl
{
  static_assert(std::is_integral_v<K> and std::is_unsigned_v<K>, "Container key must be an unsigned integer");

  static constexpr bool has_static_size = detail::is_std_array<Container>::value;

  /// Hand rolled pair-like type that is trivially copyable.
  template <typename F, typename S>
  struct kv_obj {
    template <typename A, typename B>
    kv_obj(A&& first_, B&& second_) : first(std::forward<A>(first_)), second(std::forward<B>(second_))
    {
    }

    template <typename A, typename... ArgsB>
    kv_obj(A&& first_, ArgsB&&... second_) : first(std::forward<A>(first_)), second(std::forward<ArgsB>(second_)...)
    {
    }

    F first;
    S second;
  };

  using obj_t          = kv_obj<K, V>;
  using container_elem = std::optional<obj_t>;
  using raw_container  = typename circular_map_traits::rebind_map_storage<Container, container_elem>::type;
  using buffer_type    = detail::circular_map_storage<raw_container, ForcePower2Size>;

  constexpr obj_t&       get_obj(size_t idx) { return *storage.data[idx]; }
  constexpr const obj_t& get_obj(size_t idx) const { return *storage.data[idx]; }

public:
  /// Member types.
  using key_type        = K;
  using mapped_type     = V;
  using value_type      = std::pair<K, V>;
  using difference_type = std::ptrdiff_t;

  /// Iterator type. It automatically skips absent elements of the container.
  class iterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = std::pair<K, V>;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type*;
    using reference         = value_type&;

    constexpr iterator() = default;

    constexpr iterator(circular_map_impl* map, size_t idx_) : ptr(map), idx(idx_)
    {
      if (idx < ptr->capacity() and not ptr->storage.data[idx]) {
        ++(*this);
      }
    }

    constexpr iterator& operator++()
    {
      while (++idx < ptr->capacity() and not ptr->storage.data[idx]) {
      }
      return *this;
    }

    constexpr obj_t& operator*()
    {
      srsran_assert(idx < ptr->capacity(), "Iterator out-of-bounds ({} >= {})", idx, ptr->capacity());
      return ptr->get_obj(idx);
    }

    constexpr const obj_t& operator*() const
    {
      srsran_assert(idx < ptr->capacity(), "Iterator out-of-bounds ({} >= {})", idx, ptr->capacity());
      return ptr->get_obj(idx);
    }

    constexpr obj_t* operator->()
    {
      srsran_assert(idx < ptr->capacity(), "Iterator out-of-bounds ({} >= {})", idx, ptr->capacity());
      return &ptr->get_obj(idx);
    }

    constexpr const obj_t* operator->() const
    {
      srsran_assert(idx < ptr->capacity(), "Iterator out-of-bounds ({} >= {})", idx, ptr->capacity());
      return &ptr->get_obj(idx);
    }

    constexpr bool operator==(const iterator& other) const { return ptr == other.ptr and idx == other.idx; }

    constexpr bool operator!=(const iterator& other) const { return not(*this == other); }

  private:
    friend class circular_map_impl;
    circular_map_impl* ptr = nullptr;
    size_t             idx = 0;
  };

  /// Const iterator type. It automatically skips absent elements of the container.
  class const_iterator
  {
  public:
    constexpr const_iterator() = default;

    constexpr const_iterator(const circular_map_impl* map, size_t idx_) : ptr(map), idx(idx_)
    {
      if (idx < ptr->capacity() and not ptr->storage.data[idx]) {
        ++(*this);
      }
    }

    constexpr const_iterator& operator++()
    {
      while (++idx < ptr->capacity() and not ptr->storage.data[idx]) {
      }
      return *this;
    }

    constexpr const obj_t& operator*() const { return ptr->get_obj(idx); }

    constexpr const obj_t* operator->() const { return &ptr->get_obj(idx); }

    constexpr bool operator==(const const_iterator& other) const { return ptr == other.ptr and idx == other.idx; }

    constexpr bool operator!=(const const_iterator& other) const { return not(*this == other); }

  private:
    friend class circular_map_impl;
    const circular_map_impl* ptr = nullptr;
    size_t                   idx = 0;
  };

  template <typename U                                                                                  = K,
            std::enable_if_t<circular_map_impl<U, V, Container, ForcePower2Size>::has_static_size, int> = 0>
  constexpr circular_map_impl()
  {
  }

  template <typename U                                                                                      = K,
            std::enable_if_t<not circular_map_impl<U, V, Container, ForcePower2Size>::has_static_size, int> = 0>
  circular_map_impl(size_t size)
  {
    storage.set_size(size);
  }

  /// Checks if there is an element with the given key in the container.
  /// \param key Key of the object to insert.
  constexpr bool contains(K key) const noexcept
  {
    size_t idx = storage.get_circular_idx(key);
    return storage.data[idx] and get_obj(idx).first == key;
  }

  /// Inserts a new element into the container constructed in-place with the given key if no collision is detected.
  /// \param key  Key of the object to insert.
  /// \param args Arguments forwarded to the object constructor.
  /// \return     True if no collision was detected and the object was inserted. False, otherwise.
  template <typename... Args>
  constexpr bool emplace(K key, Args&&... args)
  {
    static_assert(std::is_constructible_v<V, Args...>, "Invalid argument types");
    size_t idx = storage.get_circular_idx(key);
    if (storage.data[idx]) {
      return false;
    }

    storage.data[idx].emplace(key, std::forward<Args>(args)...);
    ++storage.count;
    return true;
  }

  /// Inserts a new element into the container with the given key if no collision is detected.
  /// \param key Key of the object to insert.
  /// \param obj Object to insert (lvalue version).
  /// \return    True if no collision was detected and the object was inserted. False, otherwise.
  constexpr bool insert(K key, const V& obj)
  {
    size_t idx = storage.get_circular_idx(key);
    if (storage.data[idx]) {
      return false;
    }

    storage.data[idx].emplace(key, obj);
    ++storage.count;
    return true;
  }

  /// Inserts a new element into the container with the given key if no collision is detected.
  /// \param key Key of the object to insert.
  /// \param obj Object to insert (rvalue version).
  /// \return    An iterator to the inserted element or the element that failed to be inserted.
  constexpr expected<iterator, V> insert(K key, V&& obj)
  {
    size_t idx = storage.get_circular_idx(key);
    if (storage.data[idx]) {
      return make_unexpected(std::move(obj));
    }

    storage.data[idx].emplace(key, std::move(obj));
    ++storage.count;
    return iterator(this, idx);
  }

  /// Inserts a new element into the container with the given key overwriting the previous one if it already exists.
  /// \param key Key of the object to insert or overwrite.
  /// \param obj Object to insert.
  template <typename U>
  constexpr void overwrite(K key, U&& obj)
  {
    size_t idx = storage.get_circular_idx(key);
    if (storage.data[idx]) {
      erase(get_obj(idx).first);
    }
    insert(key, std::forward<U>(obj));
  }

  /// Removes the element with the given key.
  /// \param key Key of the object to remove.
  /// \return    True if the element was removed. False, otherwise.
  constexpr bool erase(K key) noexcept
  {
    if (not contains(key)) {
      return false;
    }
    size_t idx = storage.get_circular_idx(key);
    storage.data[idx].reset();
    --storage.count;
    return true;
  }

  /// Removes the element with the given iterator.
  /// \param it Iterator pointing to the element to remove.
  /// \return   Iterator following the removed element.
  constexpr iterator erase(iterator it) noexcept
  {
    srsran_assert(it.idx < capacity() and it.ptr == this, "Iterator out-of-bounds ({} >= {})", it.idx, capacity());
    iterator next = it;
    ++next;
    storage.data[it.idx].reset();
    --storage.count;
    return next;
  }

  /// Erases all elements from the container.
  constexpr void clear() noexcept { storage.clear(); }

  /// Returns a reference to the value that is mapped to the given key.
  constexpr V& operator[](K key) noexcept
  {
    srsran_assert(contains(key), "Accessing non-existent ID={}", (size_t)key);
    return get_obj(storage.get_circular_idx(key)).second;
  }

  /// Returns a reference to the value that is mapped to the given key.
  constexpr const V& operator[](K key) const noexcept
  {
    srsran_assert(contains(key), "Accessing non-existent ID={}", (size_t)key);
    return get_obj(storage.get_circular_idx(key)).second;
  }

  /// Returns the number of elements in the container.
  constexpr size_t size() const noexcept { return storage.count; }

  /// Checks if the container has no elements.
  constexpr bool empty() const noexcept { return size() == 0; }

  /// Checks if the container has reached its maximum capacity.
  constexpr bool full() const noexcept { return size() == capacity(); }

  /// Returns the maximum capacity of the container.
  constexpr size_t capacity() const { return storage.max_size(); }

  /// Checks if there is an existing element with the given key.
  constexpr bool has_space(K key) noexcept { return not storage.data[storage.get_circular_idx(key)]; }

  /// Returns an iterator to the beginning of the container.
  constexpr iterator begin() { return iterator(this, 0); }

  /// Returns an iterator to the end of the container.
  constexpr iterator end() { return iterator(this, capacity()); }

  /// Returns an iterator to the beginning of the container.
  constexpr const_iterator begin() const { return const_iterator(this, 0); }

  /// Returns an iterator to the end of the container.
  constexpr const_iterator end() const { return const_iterator(this, capacity()); }

  /// Finds an element with the given key.
  constexpr iterator find(K key)
  {
    if (contains(key)) {
      return iterator(this, storage.get_circular_idx(key));
    }
    return end();
  }

  /// Finds an element with the given key.
  constexpr const_iterator find(K key) const
  {
    if (contains(key)) {
      return const_iterator(this, storage.get_circular_idx(key));
    }
    return end();
  }

private:
  /// Internal container storage.
  buffer_type storage;
};

/// Circular map with fixed and embedded data storage via a std::array<T, N>. It can contain up to N elements.
/// - Given that the map size is known at compile-time, the circular iteration over the map may be more optimized
/// (e.g. when N is a power of 2, % operator can be avoided).
/// \tparam K Type of key used to retrieve an element index. K must be an unsigned integer.
/// \tparam V Type of each element.
/// \tparam N Maximum number of elements.
template <typename K, typename V, size_t N>
using static_circular_map = circular_map_impl<K, V, std::array<V, N>, false>;

/// Circular map with internal data storage via a std::vector<T>.
/// - size has to be defined at construction time.
/// \tparam K               Type of key used to retrieve an element index. K must be an unsigned integer.
/// \tparam V               Type of each element.
/// \tparam ForcePower2Size If true, the container capacity must be a power of 2 which has the benefit of using a
///                         bit-wise and operation instead of a modulus operation for indexing the elements.
template <typename K, typename V, bool ForcePower2Size = true>
using circular_map = circular_map_impl<K, V, std::vector<V>, ForcePower2Size>;

} // namespace srsran
