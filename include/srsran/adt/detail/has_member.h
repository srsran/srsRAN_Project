/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <type_traits>

namespace srsran {
namespace detail {

template <typename T, typename F>
constexpr auto has_member_impl(F&& f) -> decltype(f(std::declval<T>()), true)
{
  return true;
}

template <typename>
constexpr bool has_member_impl(...)
{
  return false;
}

} // namespace detail

/// Check if type T has the method/member EXPR.
#define has_member(T, EXPR) detail::has_member_impl<T>([](auto&& obj) -> decltype(obj.EXPR) {})

} // namespace srsran
