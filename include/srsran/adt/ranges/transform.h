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

#include "srsran/support/srsran_assert.h"
#include <iterator>
#include <type_traits>
#include <utility>

namespace srsran {

namespace views {

/// Converts a Range into another via the \c TransformFunc predicate.
template <typename Range, typename TransformFunc>
class transform_view
{
  using range_value_type     = typename std::decay_t<Range>::value_type;
  using transform_value_type = std::invoke_result_t<TransformFunc, const range_value_type&>;

  template <typename Data>
  class iter_impl
  {
    using range_it_t  = decltype(std::begin(std::declval<Range>()));
    using parent_type = std::conditional_t<std::is_const_v<Data>,
                                           const transform_view<Range, TransformFunc>,
                                           transform_view<Range, TransformFunc>>;

  public:
    using value_type        = std::remove_const_t<transform_value_type>;
    using reference         = typename std::iterator_traits<range_it_t>::reference;
    using pointer           = Data*;
    using difference_type   = typename std::iterator_traits<range_it_t>::difference_type;
    using iterator_category = typename std::iterator_traits<range_it_t>::iterator_category;

    iter_impl(parent_type& parent_, range_it_t it_) : parent(&parent_), it(it_) {}
    iter_impl(const iter_impl<std::remove_const_t<Data>>& other) : parent(other.parent), it(other.it) {}

    iter_impl<Data>& operator++()
    {
      srsran_sanity_check(it != parent->r.end(), "Iterator incremented past end() point");
      ++it;
      return *this;
    }
    iter_impl<Data> operator++(int)
    {
      srsran_sanity_check(it != parent->r.end(), "Iterator incremented past end() point");
      auto ret{*this};
      ++it;
      return ret;
    }

    template <typename Iter                  = iter_impl,
              typename std::enable_if_t<std::is_same_v<typename std::iterator_traits<Iter>::iterator_category,
                                                       std::random_access_iterator_tag>,
                                        int> = 0>
    difference_type operator-(const Iter& other) const
    {
      srsran_sanity_check(other.parent == parent, "Invalid subtraction of iterators of different ranges");
      return it - other.it;
    }

    auto operator*() { return parent->pred(*it); }
    auto operator*() const { return parent->pred(*it); }

    bool operator==(const iter_impl<Data>& other) const { return it == other.it; }
    bool operator!=(const iter_impl<Data>& other) const { return it != other.it; }

  private:
    parent_type* parent;
    range_it_t   it;
  };

public:
  using value_type     = transform_value_type;
  using iterator       = iter_impl<value_type>;
  using const_iterator = iter_impl<const value_type>;

  template <typename R, typename T>
  transform_view(R&& r_, T&& pred_) : r(std::forward<R>(r_)), pred(std::forward<T>(pred_))
  {
  }

  iterator       begin() { return iterator{*this, r.begin()}; }
  iterator       end() { return iterator{*this, r.end()}; }
  const_iterator begin() const { return const_iterator{*this, r.begin()}; }
  const_iterator end() const { return const_iterator{*this, r.end()}; }

private:
  Range         r;
  TransformFunc pred;
};

/// Apply a transformation over an existing range.
/// \param[in] r Range to transform
/// \param[in] p Predicate that transforms each value, one-by-one.
template <typename Range, typename Pred>
transform_view<Range, Pred> transform(Range&& r, Pred&& p)
{
  return {std::forward<Range>(r), std::forward<Pred>(p)};
}

} // namespace views

} // namespace srsran
