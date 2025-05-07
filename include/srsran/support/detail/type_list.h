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

#include <type_traits>

namespace srsran {

/// List of types.
template <typename... Args>
struct type_list {};

/// Get number of Types of type_list.
template <typename... Args>
constexpr std::size_t type_list_size(type_list<Args...> t)
{
  return sizeof...(Args);
}

namespace type_list_helper {

namespace detail {

// type_at_t helpers

#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 14)

// Note: Version that leverages builtin __type_pack_element for faster compile times.

template <std::size_t I, typename TypeList>
struct type_at;

template <std::size_t I, typename... Ts>
struct type_at<I, type_list<Ts...>> {
  using type = __type_pack_element<I, Ts...>;
};

#else

template <std::size_t Index, typename List>
struct type_at;

template <std::size_t Index, typename Head, typename... Tail>
struct type_at<Index, type_list<Head, Tail...>> : type_at<Index - 1, type_list<Tail...>> {};

template <typename Head, typename... Tail>
struct type_at<0, type_list<Head, Tail...>> {
  using type = Head;
};

#endif

// concat_t helpers

template <typename... TypeLists>
struct concat;

template <typename... T>
struct concat<type_list<T...>> {
  using type = type_list<T...>;
};

template <typename... T1, typename... T2, typename... Rest>
struct concat<type_list<T1...>, type_list<T2...>, Rest...> {
  using type = typename concat<type_list<T1..., T2...>, Rest...>::type;
};

} // namespace detail

/// Get type relative to a provided Index from a type_list.
template <std::size_t Index, typename... Types>
using type_at_t = typename detail::type_at<Index, Types...>::type;

/// Metafunction that concatenates the types of multiple type_lists.
template <typename... Lists>
using concat_t = typename detail::concat<Lists...>::type;

} // namespace type_list_helper

} // namespace srsran
