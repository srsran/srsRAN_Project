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

#include <cstdint>
#include <limits>
#include <type_traits>

namespace srsran {
namespace detail {

/// Checks if a value V fits in the integer type T.
template <typename T, auto V>
constexpr bool fits_in_integer_type()
{
  static_assert(std::is_integral_v<T>, "T must be an integer type");
  using VType = decltype(V);
  static_assert(std::is_integral_v<VType>, "V must be of an integer type");

  // Compare in a common type to avoid issues with signed/unsigned promotions.
  using CommonT = std::common_type_t<T, VType>;
  return static_cast<CommonT>(V) >= static_cast<CommonT>(std::numeric_limits<T>::min()) and
         static_cast<CommonT>(V) <= static_cast<CommonT>(std::numeric_limits<T>::max());
}

template <auto V>
struct smallest_signed_fit {
  // clang-format off
  using type = std::conditional_t<fits_in_integer_type<int8_t, V>(), int8_t,
        std::conditional_t<fits_in_integer_type<int16_t, V>(), int16_t,
          std::conditional_t<fits_in_integer_type<int32_t, V>(), int32_t,
            std::conditional_t<fits_in_integer_type<int64_t, V>(), int64_t,
              void>>>>;
  // clang-format on
};

} // namespace detail

/// \brief Determines the smallest signed integer type that can fit the value V.
template <auto V>
using smallest_signed_fit_t = typename detail::smallest_signed_fit<V>::type;

/// \brief Determines a signed integer type that can fit the value V, preferring the type of V if it is large enough.
template <auto V>
using expand_signed_if_not_fit_t =
    std::conditional_t<(sizeof(std::make_signed_t<decltype(V)>) >= sizeof(smallest_signed_fit_t<V>)),
                       std::make_signed_t<decltype(V)>,
                       smallest_signed_fit_t<V>>;

} // namespace srsran
