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

#include "srsran/support/srsran_assert.h"
#include <memory>
#include <type_traits>

namespace srsran {

namespace views {

/// \brief This class provides a range (with begin and end) that filters out elements from another range for which
/// FilterFunc returns false.
/// \tparam Range
/// \tparam FilterFunc
template <typename Range, typename FilterFunc>
class filter_view
{
  template <typename Data>
  class iter_impl
  {
    using range_it_t = std::conditional_t<std::is_const<Data>::value,
                                          typename std::decay_t<Range>::const_iterator,
                                          typename std::decay_t<Range>::iterator>;

  public:
    using value_type        = std::remove_const_t<Data>;
    using reference         = Data&;
    using pointer           = Data*;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    iter_impl(filter_view<Range, FilterFunc>& parent_, range_it_t it_) : parent(&parent_), it(it_) { next_valid_it(); }

    iter_impl<Data>& operator++()
    {
      srsran_sanity_check(it != parent->r.end(), "Iterator incremented past end() point");
      ++it;
      next_valid_it();
      return *this;
    }

    value_type& operator*() { return *it; }
    value_type* operator->() { return &(*it); }

    bool operator==(const iter_impl<Data>& other) const { return it == other.it; }
    bool operator!=(const iter_impl<Data>& other) const { return it != other.it; }

  private:
    void next_valid_it()
    {
      while (it != parent->r.end() and not parent->pred(*it)) {
        ++it;
      }
    }

    filter_view<Range, FilterFunc>* parent;
    range_it_t                      it;
  };

public:
  using value_type     = typename std::decay_t<Range>::value_type;
  using iterator       = iter_impl<value_type>;
  using const_iterator = iter_impl<const value_type>;

  filter_view(Range r_, FilterFunc pred_) : r(std::forward<Range>(r_)), pred(std::forward<FilterFunc>(pred_)) {}

  iterator       begin() { return iterator{*this, r.begin()}; }
  iterator       end() { return iterator{*this, r.end()}; }
  const_iterator begin() const { return iterator{*this, r.begin()}; }
  const_iterator end() const { return iterator{*this, r.end()}; }

private:
  Range      r;
  FilterFunc pred;
};

template <typename Range, typename Pred>
filter_view<Range, Pred> filter(Range&& r, Pred&& p)
{
  return {std::forward<Range>(r), std::forward<Pred>(p)};
}

} // namespace views

} // namespace srsran
