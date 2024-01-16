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

#include <type_traits>

namespace srsran {

namespace detail {
// To allow ADL with custom begin/end
using std::begin;
using std::end;

template <typename T>
auto is_iterable_impl(int)
    -> decltype(begin(std::declval<T&>()) != end(std::declval<T&>()),   // begin/end and operator !=
                void(),                                                 // Handle evil operator ,
                ++std::declval<decltype(begin(std::declval<T&>()))&>(), // operator ++
                void(*begin(std::declval<T&>())),                       // operator*
                std::true_type{});

template <typename T>
std::false_type is_iterable_impl(...);

} // namespace detail

template <typename T>
using is_iterable = decltype(detail::is_iterable_impl<T>(0));

} // namespace srsran