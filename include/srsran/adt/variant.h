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

#include "variant/variant.hpp"

namespace srsran {

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

/// \brief Reads the value of the variant given the index.
/// \param[in] v Variant to examine.
/// \return Reference to the value stored in the variant.
template <size_t Idx, typename... Types>
inline constexpr const auto& variant_get(const variant<Types...>& v)
{
  return mpark::get<Idx, Types...>(v);
}

/// Apply a visitor to the variant.
template <typename Visitor, typename... Vs>
inline constexpr auto variant_visit(Visitor&& visitor, Vs&&... vs)
{
  return mpark::visit(std::forward<Visitor>(visitor), std::forward<Vs>(vs)...);
}

} // namespace srsran
