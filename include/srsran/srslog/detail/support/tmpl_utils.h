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

#include <cstddef>
#include <cstdint>
#include <tuple>
#include <type_traits>

namespace srslog {

namespace detail {

///
/// Implementation of the std::index_sequence C++14 library utility.
///

template <std::size_t...>
struct index_sequence {
};

template <std::size_t N, std::size_t... Next>
struct index_sequence_helper : public index_sequence_helper<N - 1U, N - 1U, Next...> {
};

template <std::size_t... Next>
struct index_sequence_helper<0U, Next...> {
  using type = index_sequence<Next...>;
};

template <std::size_t N>
using make_index_sequence = typename index_sequence_helper<N>::type;

///
/// Implementation of the std::get<T> C++14 library utility.
///

template <typename T, typename Tuple>
struct tuple_index;

template <typename T, typename... Ts>
struct tuple_index<T, std::tuple<T, Ts...>> {
  static constexpr std::size_t value = 0;
};

template <typename T, typename U, typename... Ts>
struct tuple_index<T, std::tuple<U, Ts...>> {
  static constexpr std::size_t value = 1 + tuple_index<T, std::tuple<Ts...>>::value;
};

template <typename T, typename... Ts>
constexpr std::size_t get_type_index_in_tuple()
{
  return tuple_index<T, std::tuple<Ts...>>::value;
}

///
/// Traits to restrict template parameter to iterators over uint8_t, e.g. for byte_buffer and friends.
///

template <typename...>
using void_t = void;

template <typename, typename = void>
struct is_byte_iterable : public std::false_type {
};

template <typename T>
struct is_byte_iterable<
    T,
    void_t<decltype(++std::declval<T>()),
           typename std::enable_if<std::is_same<typename T::value_type, uint8_t>::value, int>::type>>
  : public std::true_type {
};

} // namespace detail

} // namespace srslog
