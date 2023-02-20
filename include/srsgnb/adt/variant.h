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

#include "variant/variant.hpp"

namespace srsgnb {

template <typename... Types>
using variant = mpark::variant<Types...>;

/// \brief Reads the value of the variant given the index or the type (if the type is unique).
/// \param[in] v Variant to examine.
/// \return Reference to the value stored in the variant.
template <typename T, typename... Types>
inline constexpr const T& variant_get(const variant<Types...>& v)
{
  return mpark::get<T, Types...>(v);
}

/// \brief Reads the value of the variant given the index or the type (if the type is unique).
/// \param[in] v Variant to examine.
/// \return Non-const Reference to the value stored in the variant.
template <typename T, typename... Types>
inline constexpr T& variant_get(variant<Types...>& v)
{
  return mpark::get<T, Types...>(v);
}

/// \brief Checks if the variant v holds the alternative T.
/// \param[in] v Variant to examine.
/// \return True if the variant currently holds the alternative T, false otherwise.
template <typename T, typename... Types>
inline constexpr bool variant_holds_alternative(const variant<Types...>& v) noexcept
{
  return mpark::holds_alternative<T, Types...>(v);
}

} // namespace srsgnb
