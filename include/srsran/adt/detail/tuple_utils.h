/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  for_each_impl(t, f, std::make_index_sequence<std::tuple_size<std::decay_t<T>>::value>{});
}

template <typename Tuple, typename Pred>
constexpr bool any_of_impl(Tuple&& /*unused*/, Pred&& /*unused*/, std::index_sequence<>)
{
  return false;
}

template <typename Tuple, typename Pred, size_t FirstIs, size_t... Is>
constexpr bool any_of_impl(Tuple&& t, Pred&& pred, std::index_sequence<FirstIs, Is...>)
{
  return pred(std::get<FirstIs>(t)) || any_of_impl(t, std::forward<Pred>(pred), std::index_sequence<Is...>{});
}

template <typename... Elements, typename Pred, size_t... is>
constexpr bool any_of(std::tuple<Elements...>& t, Pred&& pred)
{
  return any_of_impl(t, std::forward<Pred>(pred), std::index_sequence_for<Elements...>{});
}

} // namespace detail

} // namespace srsran