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

#include "srsran/support/srsran_assert.h"
#include <limits>
#include <vector>

namespace srsran {
namespace soa_table {

template <typename T>
class cell_view
{
public:
  explicit cell_view(std::vector<T>& data_, unsigned offset_) : data(&data_), offset(offset_) {}

  T&       value() { return (*data)[offset]; }
  const T& value() const { return (*data)[offset]; }
  T&       operator*() { return (*data)[offset]; }
  const T& operator*() const { return (*data)[offset]; }

  bool operator==(const cell_view& other) const { return value() == other.value(); }
  bool operator!=(const cell_view& other) const { return not(*this == other); }

private:
  std::vector<T>* data   = nullptr;
  unsigned        offset = 0;
};

template <typename T>
class column_view
{
  static constexpr unsigned INVALID_INDEX = std::numeric_limits<unsigned>::max();

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
  explicit column_view(const std::vector<T>&        data_,
                       const std::vector<unsigned>& index_map_,
                       const std::vector<unsigned>& index_reverse_map_) :
    data(&data_), index_map(&index_map_), index_reverse_map(&index_reverse_map_)
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
    return cell_view{*data, (*index_map)[row_id]};
  }

  cell_view<const T> operator[](unsigned row_id) const
  {
    srsran_assert(has_row_id(row_id), "Invalid row_id");
    return cell_view{*data, (*index_map)[row_id]};
  }

private:
  const std::vector<T>*        data;
  const std::vector<unsigned>* index_map;
  const std::vector<unsigned>* index_reverse_map;
};

template <typename... ColumnTypes>
class table
{
public:
  template <std::size_t ColIndex>
  using column_type = std::tuple_element_t<ColIndex, std::tuple<ColumnTypes...>>;

  template <std::size_t I>
  auto& column()
  {
    return make_column_view(std::get<I>(columns), index_map, index_reverse_map);
  }
  template <std::size_t I>
  const auto& column() const
  {
    return make_column_view(std::get<I>(columns), index_map, index_reverse_map);
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

  // template <typename... Vals>
  // unsigned insert(Vals&&... vals)
  // {
  //   unsigned offset = index_reverse_map.size();
  // }

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
      std::apply([offset](const auto& col) { std::swap(col[offset], col.back()); }, columns);
      index_reverse_map[offset]            = index_reverse_map.back();
      index_map[index_reverse_map[offset]] = offset;
    }
    std::apply([offset](const auto& col) { col.pop_back(); }, columns);
    index_reverse_map.pop_back();
    index_map[row_id] = free_list_head;
    free_list_head    = row_id;
    return true;
  }

  void reserve(unsigned size)
  {
    std::apply([size](const auto& col) { col.reserve(size); }, columns);
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
    return column_view<T>{data, index_map, index_reverse_map};
  }

  std::tuple<std::vector<ColumnTypes>...> columns;
  std::vector<unsigned>                   index_map;
  std::vector<unsigned>                   index_reverse_map;
  unsigned                                free_list_head{0};
};

} // namespace soa_table
} // namespace srsran
