/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "variant/variant.hpp"

namespace srsgnb {

template <typename... Types>
using variant = mpark::variant<Types...>;

template <typename T, typename... Types>
inline constexpr T& variant_get(variant<Types...>& v)
{
  return mpark::get<T, Types...>(v);
}

template <typename T, typename... Types>
inline constexpr const T& variant_get(const variant<Types...>& v)
{
  return mpark::get<T, Types...>(v);
}

} // namespace srsgnb
