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
// TODO: Use builtin __builtin_type_pack_element if available.

template <std::size_t Index, typename List>
struct type_at;

template <std::size_t Index, typename Head, typename... Tail>
struct type_at<Index, type_list<Head, Tail...>> : type_at<Index - 1, type_list<Tail...>> {};

template <typename Head, typename... Tail>
struct type_at<0, type_list<Head, Tail...>> {
  using type = Head;
};

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
