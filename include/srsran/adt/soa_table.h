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
#include "srsran/adt/strong_type.h"
#include "srsran/support/srsran_assert.h"
#include <limits>
#include <vector>

namespace srsran {
namespace soa {

/// Type representing a row ID.
struct row_id_tag;
using row_id = strong_type<uint32_t, row_id_tag, strong_equality, strong_comparison>;

/// View of a row in a table.
template <typename TableType>
class row_view
{
public:
  row_view(TableType& table_, unsigned offset_) : table(&table_), offset(offset_) {}
  row_view(const row_view<std::remove_const_t<TableType>>& other) : table(other.table), offset(other.offset) {}

  /// Number of columns in the table.
  static constexpr size_t nof_columns() { return TableType::nof_columns(); }

  /// Row ID of the row.
  row_id id() const { return row_id{table->index_reverse_map[offset]}; }

  /// Retrieves a cell from the table for column I.
  template <std::size_t ColIndex>
  auto& at()
  {
    static_assert(ColIndex < nof_columns(), "Column index out of range");
    auto& cols = std::get<ColIndex>(table->columns);
    srsran_sanity_check(offset < cols.size(), "Row offset out of range");
    return cols[offset];
  }
  template <std::size_t ColIndex>
  const auto& at() const
  {
    static_assert(ColIndex < nof_columns(), "Column index out of range");
    auto& cols = std::get<ColIndex>(table->columns);
    srsran_sanity_check(offset < cols.size(), "Row offset out of range");
    return cols[offset];
  }

  /// Retrieves a cell from the table for column of type T.
  template <typename T>
  auto& at()
  {
    auto& cols = std::get<std::vector<T>>(table->columns);
    srsran_sanity_check(offset < cols.size(), "Row offset out of range");
    return cols[offset];
  }
  template <typename T>
  const auto& at() const
  {
    auto& cols = std::get<std::vector<T>>(table->columns);
    srsran_sanity_check(offset < cols.size(), "Row offset out of range");
    return cols[offset];
  }

  template <std::size_t ColIndex>
  friend auto& get(const row_view& rv)
  {
    return rv.at<ColIndex>();
  }
  template <std::size_t ColIndex>
  friend auto& get(row_view&& rv)
  {
    return rv.at<ColIndex>();
  }
  template <typename T>
  friend auto& get(const row_view& rv)
  {
    return rv.at<T>();
  }
  template <typename T>
  friend auto& get(row_view&& rv)
  {
    return rv.at<T>();
  }

  bool operator==(const row_view& other) const
  {
    bool result = true;
    std::apply(
        [&](const auto&... lhs) mutable {
          std::apply([&](const auto&... rhs) mutable { result = ((lhs[offset] == rhs[other.offset]) && ...); },
                     other.table->columns);
        },
        table->columns);
    return result;
  }
  bool operator!=(const row_view& other) const { return not(*this == other); }

private:
  friend class row_view<const TableType>;

  TableType* table;
  unsigned   offset;
};

/// View of a column in a table.
template <typename T>
class column_view
{
  static constexpr unsigned INVALID_INDEX = std::numeric_limits<unsigned>::max();
  using VectorType = std::conditional_t<std::is_const_v<T>, const std::vector<std::remove_const_t<T>>, std::vector<T>>;

public:
  explicit column_view(VectorType&                  data_,
                       const std::vector<unsigned>& index_map_,
                       const std::vector<row_id>&   index_reverse_map_) :
    col_elems(&data_), index_map(&index_map_), index_reverse_map(&index_reverse_map_)
  {
  }

  [[nodiscard]] bool has_row_id(row_id rid) const
  {
    if (rid.value() >= index_map->size()) {
      return false;
    }
    unsigned offset = (*index_map)[rid.value()];
    if (offset >= index_reverse_map->size()) {
      return false;
    }
    return (*index_reverse_map)[offset] == rid;
  }

  T& operator[](row_id rid)
  {
    srsran_assert(has_row_id(rid), "Invalid row_id");
    return (*col_elems)[(*index_map)[rid.value()]];
  }

  const T& operator[](row_id rid) const
  {
    srsran_assert(has_row_id(rid), "Invalid row_id");
    return (*col_elems)[(*index_map)[rid.value()]];
  }

  /// Retrieves the number of rows of the table.
  std::size_t size() const { return index_reverse_map->size(); }

  /// Checks if the table contains any rows.
  bool     empty() const { return index_reverse_map->empty(); }
  T*       data() { return col_elems->data(); }
  const T* data() const { return col_elems->data(); }

  /// Unsorted iterator of the column.
  auto begin() { return col_elems->begin(); }
  auto begin() const { return col_elems->begin(); }
  auto cbegin() const { return col_elems->begin(); }
  auto end() { return col_elems->end(); }
  auto end() const { return col_elems->end(); }
  auto cend() const { return col_elems->end(); }

  span<const T> to_span() const { return span<const T>(col_elems->data(), col_elems->size()); }
  span<T>       to_span() { return span<T>(col_elems->data(), col_elems->size()); }

private:
  VectorType*                  col_elems;
  const std::vector<unsigned>* index_map;
  const std::vector<row_id>*   index_reverse_map;
};

/// \brief Struct-of-arrays representation of a list of objects.
/// Each row corresponds to an object, and each column to a field of the object.
/// Objects have a stable row ID, but their offset or address in the internal storage may change.
/// Each column of the table is represented as a vector. All elements are stored contiguously in memory, with no
/// holes.
template <typename... ColumnTypes>
class table
{
  struct sentinel {};

  /// Iterator of rows in the table. The iterator is not sorted by row ID.
  template <typename TableType>
  class iter_impl
  {
    struct arrow_proxy {
      row_view<const TableType>        r;
      const row_view<const TableType>* operator->() const { return &r; }
    };

  public:
    using value_type      = row_view<table>;
    using difference_type = std::ptrdiff_t;
    using reference       = row_view<TableType>;
    // No meaningful raw pointer available.
    using pointer = void;
    // forward iterator requires *it to be an actual reference to a stable object.
    using iterator_category = std::input_iterator_tag;

    iter_impl() = default;
    iter_impl(TableType& table_, size_t offset_) : parent(&table_), offset(offset_) {}

    operator iter_impl<const table>() const noexcept { return iter_impl<const table>{*parent, offset}; }

    row_view<TableType>       operator*() { return row_view<TableType>{*parent, offset}; }
    row_view<const TableType> operator*() const { return row_view<const TableType>{*parent, offset}; }
    arrow_proxy               operator->() const { return arrow_proxy{**this}; }

    iter_impl& operator++()
    {
      ++offset;
      return *this;
    }
    iter_impl operator++(int)
    {
      iter_impl tmp = *this;
      ++offset;
      return tmp;
    }

    bool        operator==(const iter_impl& other) const { return offset == other.offset and parent == other.parent; }
    bool        operator!=(const iter_impl& other) const { return not(*this == other); }
    bool        operator==(sentinel /* unused */) const { return offset == parent->size(); }
    bool        operator!=(sentinel /* unused */) const { return offset != parent->size(); }
    friend bool operator==(sentinel /* unused */, const iter_impl& rhs) { return rhs.offset == rhs.parent->size(); }
    friend bool operator!=(sentinel /* unused */, const iter_impl& rhs) { return rhs.offset != rhs.parent->size(); }

  private:
    friend class table<ColumnTypes...>;

    TableType* parent = nullptr;
    unsigned   offset = 0;
  };

public:
  template <std::size_t ColIndex>
  using column_type = std::tuple_element_t<ColIndex, std::tuple<ColumnTypes...>>;

  using iterator       = iter_impl<table>;
  using const_iterator = iter_impl<const table>;

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

  row_view<table> row(row_id rid)
  {
    srsran_assert(has_row_id(rid), "Invalid row_id");
    return row_view<table>{*this, index_map[rid.value()]};
  }
  row_view<const table> row(row_id rid) const
  {
    srsran_assert(has_row_id(rid), "Invalid row_id");
    return row_view<const table>{*this, index_map[rid.value()]};
  }

  /// Gets number of columns of the table.
  static constexpr size_t nof_columns() { return sizeof...(ColumnTypes); }

  /// Retrieves the number of rows of the table.
  [[nodiscard]] size_t size() const { return index_reverse_map.size(); }

  /// Checks if the table contains any rows.
  [[nodiscard]] bool empty() const { return index_reverse_map.empty(); }

  /// Pre-reserved space for the table.
  [[nodiscard]] size_t capacity() const { return index_reverse_map.capacity(); }

  /// Checks whether the table contains row with provided ID.
  [[nodiscard]] bool has_row_id(row_id rid) const
  {
    if (rid.value() >= index_map.size()) {
      return false;
    }
    const unsigned offset = index_map[rid.value()];
    if (offset >= index_reverse_map.size()) {
      return false;
    }
    return index_reverse_map[offset] == rid;
  }

  template <std::size_t I>
  auto& at(row_id rid)
  {
    static_assert(I < sizeof...(ColumnTypes), "Column index out of bounds");
    srsran_assert(has_row_id(rid), "Invalid row_id");
    return std::get<I>(columns)[index_map[rid.value()]];
  }
  template <std::size_t I>
  const auto& at(row_id rid) const
  {
    static_assert(I < sizeof...(ColumnTypes), "Column index out of bounds");
    srsran_assert(has_row_id(rid), "Invalid row_id");
    return std::get<I>(columns)[index_map[rid.value()]];
  }

  template <typename... Vals>
  row_id insert(Vals&&... vals)
  {
    static_assert(sizeof...(Vals) == sizeof...(ColumnTypes), "Invalid number of values to insert");
    static_assert((std::is_constructible_v<ColumnTypes, Vals&&> && ...),
                  "Argument types must be constructible into corresponding vector::value_type.");
    unsigned offset = index_reverse_map.size();
    unsigned rid    = free_list_head;
    if (rid < index_map.size()) {
      free_list_head = index_map[rid];
      index_map[rid] = offset;
    } else {
      srsran_sanity_check(rid == index_map.size(), "Invalid free_list_head");
      index_map.push_back(offset);
      free_list_head = rid + 1;
    }
    index_reverse_map.push_back(row_id{rid});
    push_back_impl(std::index_sequence_for<ColumnTypes...>{}, std::forward<Vals>(vals)...);

    return row_id{rid};
  }

  bool erase(row_id rid)
  {
    if (rid.value() >= index_map.size()) {
      return false;
    }
    const unsigned offset = index_map[rid.value()];
    if (offset >= index_reverse_map.size()) {
      return false;
    }
    srsran_sanity_check(index_reverse_map[offset] == rid, "Invalid row_id");

    if (offset != index_reverse_map.size() - 1) {
      std::apply([offset](auto&... cols) { (std::swap(cols[offset], cols.back()), ...); }, columns);
      index_reverse_map[offset]                    = index_reverse_map.back();
      index_map[index_reverse_map[offset].value()] = offset;
    }
    std::apply([](auto&... cols) { (cols.pop_back(), ...); }, columns);
    index_reverse_map.pop_back();
    index_map[rid.value()] = free_list_head;
    free_list_head         = rid.value();
    return true;
  }

  void erase(const_iterator it)
  {
    srsran_assert(it.parent == this, "Iterator does not belong to this table");
    srsran_assert(it.offset < size(), "Iterator out of range");
    erase(index_reverse_map[it.offset]);
  }

  void clear()
  {
    std::apply([](auto&... cols) { (cols.clear(), ...); }, columns);
    index_map.clear();
    index_reverse_map.clear();
    free_list_head = 0;
  }

  void reserve(unsigned size)
  {
    std::apply([size](auto&... cols) { (cols.reserve(size), ...); }, columns);
    index_map.reserve(size);
    index_reverse_map.reserve(size);
  }

  /// Get iterator of rows pointing to the beginning. Note: this iterator is not sorted by row_id.
  iterator       begin() { return iterator{*this, 0}; }
  const_iterator begin() const { return const_iterator{*this, 0}; }
  const_iterator cbegin() const { return const_iterator{*this, 0}; }
  sentinel       end() const { return {}; }
  sentinel       cend() const { return {}; }

private:
  friend class row_view<table>;
  friend class row_view<const table>;

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
  void push_back_impl(std::index_sequence<Is...> /* unused */, Vals&&... vals)
  {
    ((std::get<Is>(columns).emplace_back(std::forward<Vals>(vals))), ...);
  }

  /// Columns storage.
  std::tuple<std::vector<ColumnTypes>...> columns;
  /// Map from row_id to offset in the columns vectors.
  std::vector<unsigned> index_map;
  /// Map from offset in the columns vectors to row_id.
  std::vector<row_id> index_reverse_map;
  /// Linked List of free row_ids (using holes of index_map as next pointer).
  unsigned free_list_head{0};
};

} // namespace soa
} // namespace srsran

template <std::size_t ColIndex, typename Table>
struct std::tuple_element<ColIndex, srsran::soa::row_view<Table>> {
  using type = typename Table::template column_type<ColIndex>;
};

template <typename Table>
struct std::tuple_size<srsran::soa::row_view<Table>> : std::integral_constant<std::size_t, Table::nof_columns()> {};
