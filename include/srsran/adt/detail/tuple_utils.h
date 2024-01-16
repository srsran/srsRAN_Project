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

#include <array>

namespace srsran {

namespace detail {

template <size_t N, size_t... Is>
constexpr auto as_tuple(const std::array<unsigned, N>& arr, std::index_sequence<Is...> /*unused*/)
{
  return std::make_tuple(arr[Is]...);
}

/// Convert an std::array<T, N> to a tuple<T, T, ...>.
template <typename T, size_t N>
constexpr auto as_tuple(const std::array<T, N>& arr)
{
  return as_tuple(arr, std::make_index_sequence<N>{});
}

template <typename T, typename F, size_t... Is>
constexpr void for_each_impl(T&& t, const F& f, std::index_sequence<Is...>)
{
  (void)std::initializer_list<int>{(f(std::get<Is>(t)), 0)...};
}

template <typename T, typename F>
constexpr void for_each(T&& t, const F& f)
{
  for_each_impl(std::forward<T>(t), f, std::make_index_sequence<std::tuple_size<std::decay_t<T>>::value>{});
}

template <typename Tuple, typename Pred>
constexpr bool any_of_impl(Tuple&& /*unused*/, Pred&& /*unused*/, std::index_sequence<>)
{
  return false;
}

template <typename Tuple, typename Pred, size_t FirstIs, size_t... Is>
constexpr bool any_of_impl(Tuple&& t, Pred&& pred, std::index_sequence<FirstIs, Is...>)
{
  return pred(std::get<FirstIs>(t)) ||
         any_of_impl(std::forward<Tuple>(t), std::forward<Pred>(pred), std::index_sequence<Is...>{});
}

template <typename... Elements, typename Pred, size_t... is>
constexpr bool any_of(std::tuple<Elements...>& t, Pred&& pred)
{
  return any_of_impl(t, std::forward<Pred>(pred), std::index_sequence_for<Elements...>{});
}

template <typename... Elements, typename Pred, size_t... is>
constexpr bool any_of(const std::tuple<Elements...>& t, Pred&& pred)
{
  return any_of_impl(t, std::forward<Pred>(pred), std::index_sequence_for<Elements...>{});
}

} // namespace detail

} // namespace srsran