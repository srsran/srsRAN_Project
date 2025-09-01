/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/span.h"
#include "srsran/support/srsran_assert.h"
#include <limits>
#include <vector>

namespace srsran {
namespace soa {

template <typename T>
class cell_view
{
  using VectorType = std::conditional_t<std::is_const_v<T>, const std::vector<std::remove_const_t<T>>, std::vector<T>>;

public:
  explicit cell_view(VectorType& data_, unsigned offset_) : col(&data_), offset(offset_) {}

  T&       value() { return (*col)[offset]; }
  const T& value() const { return (*col)[offset]; }
  T&       operator*() { return (*col)[offset]; }
  const T& operator*() const { return (*col)[offset]; }
  T*       operator->() { return &(*col)[offset]; }
  const T* operator->() const { return &(*col)[offset]; }

  bool operator==(const cell_view& other) const { return value() == other.value(); }
  bool operator!=(const cell_view& other) const { return not(*this == other); }

private:
  VectorType* col    = nullptr;
  unsigned    offset = 0;
};

/// View of a column in a table.
template <typename T>
class column_view
{
  static constexpr unsigned INVALID_INDEX = std::numeric_limits<unsigned>::max();
  using VectorType = std::conditional_t<std::is_const_v<T>, const std::vector<std::remove_const_t<T>>, std::vector<T>>;

  template <typename Data>
  class unsorted_iter_impl
  {
    using parent_type = std::conditional_t<std::is_const_v<Data>, column_view<T>*, const column_view<T>*>;

  public:
    using value_type      = T;
    using difference_type = std::ptrdiff_t;

    unsorted_iter_impl(parent_type* parent_, unsigned index_) : parent(parent_), index(index_) {}

    unsorted_iter_impl& operator++()
    {
      ++index;
      return *this;
    }

    unsorted_iter_impl operator++(int)
    {
      unsorted_iter_impl tmp = *this;
      ++index;
      return tmp;
    }

    T&       operator*() { return (*parent->data)[index]; }
    const T& operator*() const { return (*parent->data)[index]; }

    T* operator->() { return &(*parent->data)[index]; }

    bool operator==(const unsorted_iter_impl& other) const { return index == other.index and parent == other.parent; }

    bool operator!=(const unsorted_iter_impl& other) const { return not(*this == other); }

    parent_type* parent;
    unsigned     index;
  };

public:
  explicit column_view(VectorType&                  data_,
                       const std::vector<unsigned>& index_map_,
                       const std::vector<unsigned>& index_reverse_map_) :
    col_elems(&data_), index_map(&index_map_), index_reverse_map(&index_reverse_map_)
  {
  }

  bool has_row_id(unsigned row_id) const
  {
    if (row_id >= index_map->size()) {
      return false;
    }
    unsigned offset = (*index_map)[row_id];
    return offset < index_reverse_map->size();
  }

  cell_view<T> operator[](unsigned row_id)
  {
    srsran_assert(has_row_id(row_id), "Invalid row_id");
    return cell_view<T>{*col_elems, (*index_map)[row_id]};
  }

  cell_view<const T> operator[](unsigned row_id) const
  {
    srsran_assert(has_row_id(row_id), "Invalid row_id");
    return cell_view<const T>{*col_elems, (*index_map)[row_id]};
  }

  std::size_t size() const { return index_reverse_map->size(); }
  bool        empty() const { return index_reverse_map->empty(); }
  T*          data() { return col_elems->data(); }
  const T*    data() const { return col_elems->data(); }

  /// Unsorted view of the column (i.e. the index does not correspond to the row id).
  span<const T> unsorted() const { return span<const T>(col_elems->data(), col_elems->size()); }
  span<T>       unsorted() { return span<T>(col_elems->data(), col_elems->size()); }

private:
  VectorType*                  col_elems;
  const std::vector<unsigned>* index_map;
  const std::vector<unsigned>* index_reverse_map;
};

/// \brief Struct-of-arrays representation of a list of objects.
/// Each row corresponds to an object, and each column to a field of the object.
/// Objects have a stable row ID, but their offset or address in the internal storage may change.
/// Each column of the table is represented as a vector. All elements are stored contiguously in memory, with no holes.
template <typename... ColumnTypes>
class table
{
public:
  template <std::size_t ColIndex>
  using column_type = std::tuple_element_t<ColIndex, std::tuple<ColumnTypes...>>;

  template <std::size_t I>
  auto column()
  {
    return make_column_view(std::get<I>(columns));
  }
  template <std::size_t I>
  auto column() const
  {
    return make_column_view(std::get<I>(columns));
  }

  bool     empty() const { return index_reverse_map.empty(); }
  unsigned size() const { return index_reverse_map.size(); }
  unsigned capacity() const { return index_reverse_map.capacity(); }

  bool has_row_id(unsigned row_id) const
  {
    if (row_id >= index_map.size()) {
      return false;
    }
    unsigned offset = index_map[row_id];
    return offset < index_reverse_map.size();
  }

  template <std::size_t I>
  auto cell(unsigned row_id)
  {
    srsran_assert(has_row_id(row_id), "Invalid row_id");
    return make_cell_view(std::get<I>(columns), index_map[row_id]);
  }
  template <std::size_t I>
  auto cell(unsigned row_id) const
  {
    srsran_assert(has_row_id(row_id), "Invalid row_id");
    return make_cell_view(std::get<I>(columns), index_map[row_id]);
  }

  template <typename... Vals>
  unsigned insert(Vals&&... vals)
  {
    static_assert(sizeof...(Vals) == sizeof...(ColumnTypes), "Invalid number of values to insert");
    static_assert((std::is_constructible_v<ColumnTypes, Vals&&> && ...),
                  "Argument types must be constructible into corresponding vector::value_type.");
    unsigned offset = index_reverse_map.size();
    unsigned row_id = free_list_head;
    if (row_id < index_map.size()) {
      free_list_head    = index_map[row_id];
      index_map[row_id] = offset;
    } else {
      srsran_sanity_check(row_id == index_map.size(), "Invalid free_list_head");
      index_map.push_back(offset);
      free_list_head = row_id + 1;
    }
    index_reverse_map.push_back(row_id);
    push_back_impl(std::index_sequence_for<ColumnTypes...>{}, std::forward<Vals>(vals)...);

    return row_id;
  }

  bool erase(unsigned row_id)
  {
    if (row_id >= index_map.size()) {
      return false;
    }
    unsigned offset = index_map[row_id];
    if (offset >= index_reverse_map.size()) {
      return false;
    }
    srsran_sanity_check(index_reverse_map[offset] == row_id, "Invalid row_id");

    if (offset != index_reverse_map.size() - 1) {
      std::apply([offset](auto&... cols) { (std::swap(cols[offset], cols.back()), ...); }, columns);
      index_reverse_map[offset]            = index_reverse_map.back();
      index_map[index_reverse_map[offset]] = offset;
    }
    std::apply([](auto&... cols) { (cols.pop_back(), ...); }, columns);
    index_reverse_map.pop_back();
    index_map[row_id] = free_list_head;
    free_list_head    = row_id;
    return true;
  }

  void reserve(unsigned size)
  {
    std::apply([size](auto&... cols) { (cols.reserve(size), ...); }, columns);
    index_map.reserve(size);
    index_reverse_map.reserve(size);
  }

private:
  template <typename T>
  cell_view<T> make_cell_view(std::vector<T>& data, unsigned offset)
  {
    return cell_view<T>{data, offset};
  }
  template <typename T>
  cell_view<const T> make_cell_view(const std::vector<T>& data, unsigned offset) const
  {
    return cell_view<const T>{data, offset};
  }
  template <typename T>
  column_view<T> make_column_view(std::vector<T>& data)
  {
    return column_view<T>{data, index_map, index_reverse_map};
  }
  template <typename T>
  column_view<const T> make_column_view(const std::vector<T>& data) const
  {
    return column_view<const T>{data, index_map, index_reverse_map};
  }

  template <typename... Vals, std::size_t... Is>
  void push_back_impl(std::index_sequence<Is...>, Vals&&... vals)
  {
    ((std::get<Is>(columns).emplace_back(std::forward<Vals>(vals))), ...);
  }

  /// Columns storage.
  std::tuple<std::vector<ColumnTypes>...> columns;
  /// Map from row_id to offset in the columns vectors.
  std::vector<unsigned> index_map;
  /// Map from offset in the columns vectors to row_id.
  std::vector<unsigned> index_reverse_map;
  /// Linked List of free row_ids (using holes of index_map as next pointer).
  unsigned free_list_head{0};
};

} // namespace soa
} // namespace srsran
