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

#include <array>
#include <memory>

namespace srsran {

/// \brief Container type that associates Id types to container elements.
/// Pointer/References/Iterators remain valid throughout the object lifetime.
/// This container is efficient memory-wise, avoiding reserving space for elements T before those elements are
/// actually created.
/// \tparam IdType
/// \tparam T
/// \tparam MAX_SIZE
template <typename IdType, typename T, size_t MAX_SIZE>
class stable_id_map
{
  static_assert(std::is_convertible<IdType, size_t>::value or std::is_enum<IdType>::value,
                "IdType must be convertible to index");

  using container_t = std::array<std::unique_ptr<T>, MAX_SIZE>;

  template <typename Data>
  class iter_impl
  {
    using iter_t = std::
        conditional_t<std::is_const<Data>::value, typename container_t::const_iterator, typename container_t::iterator>;

  public:
    using value_type        = Data;
    using reference         = Data&;
    using pointer           = Data*;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    iter_impl() = default;
    iter_impl(iter_t it_, iter_t it_end_) : it(it_), it_end(it_end_) { next_valid_it(); }

    iter_impl<Data>& operator++()
    {
      ++it;
      next_valid_it();
      return *this;
    }

    reference operator*() { return **it; }
    pointer   operator->() { return &(**it); }
    reference operator*() const { return **it; }
    pointer   operator->() const { return &(**it); }

    bool operator==(const iter_impl<Data>& other) const { return it == other.it; }
    bool operator!=(const iter_impl<Data>& other) const { return it != other.it; }

  private:
    void next_valid_it()
    {
      while (it != it_end and (*it) == nullptr) {
        ++it;
      }
    }

    iter_t it;
    iter_t it_end;
  };

public:
  using value_type     = T;
  using iterator       = iter_impl<T>;
  using const_iterator = iter_impl<const T>;

  bool contains(IdType id) const
  {
    size_t idx = static_cast<size_t>(id);
    srsran_assert(idx < MAX_SIZE, "Invalid Id={}", id);
    return elems[idx] != nullptr;
  }

  void insert(IdType id_value, std::unique_ptr<T> u)
  {
    srsran_assert(u, "Inserting a null element in Id={}", id_value);
    srsran_assert(not contains(id_value), "Id={} already exists", id_value);
    size_t idx = static_cast<size_t>(id_value);
    elems[idx] = std::move(u);
    ++nof_elems;
  }

  template <typename... Args>
  void emplace(IdType id_value, Args&&... args)
  {
    srsran_assert(not contains(id_value), "Id={} already exists", id_value);
    size_t idx = static_cast<size_t>(id_value);
    elems[idx] = std::make_unique<T>(std::forward<Args>(args)...);
    ++nof_elems;
  }

  bool erase(IdType id_value)
  {
    if (not contains(id_value)) {
      return false;
    }
    size_t idx = static_cast<size_t>(id_value);
    elems[idx].reset();
    nof_elems--;
    return true;
  }

  void clear()
  {
    for (auto& u : elems) {
      u.reset();
    }
    nof_elems = 0;
  }

  T& operator[](IdType id_value)
  {
    srsran_assert(contains(id_value), "Id={} does not exist", id_value);
    return *elems[static_cast<size_t>(id_value)];
  }

  const T& operator[](IdType id_value) const
  {
    srsran_assert(contains(id_value), "Id={} does not exist", id_value);
    return *elems[static_cast<size_t>(id_value)];
  }

  iterator find(IdType id_value)
  {
    return contains(id_value) ? iterator{elems.begin() + static_cast<size_t>(id_value), elems.end()} : end();
  }
  const_iterator find(IdType id_value) const
  {
    return contains(id_value) ? const_iterator{elems.begin() + static_cast<size_t>(id_value), elems.end()} : end();
  }

  size_t size() const { return nof_elems; }
  bool   empty() const { return nof_elems == 0; }

  iterator       begin() { return {elems.begin(), elems.end()}; }
  iterator       end() { return {elems.end(), elems.end()}; }
  const_iterator begin() const { return {elems.begin(), elems.end()}; }
  const_iterator end() const { return {elems.end(), elems.end()}; }

  iterator lower_bound(IdType id)
  {
    return static_cast<size_t>(id) >= MAX_SIZE ? iterator{elems.end(), elems.end()}
                                               : iterator{elems.begin() + static_cast<size_t>(id), elems.end()};
  }
  const_iterator lower_bound(IdType id) const
  {
    return static_cast<size_t>(id) >= MAX_SIZE ? const_iterator{elems.end(), elems.end()}
                                               : const_iterator{elems.begin() + static_cast<size_t>(id), elems.end()};
  }

private:
  size_t                                   nof_elems = 0;
  std::array<std::unique_ptr<T>, MAX_SIZE> elems;
};

} // namespace srsran
