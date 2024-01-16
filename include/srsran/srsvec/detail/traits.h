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

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"

namespace srsran {
namespace srsvec {
namespace detail {

/// Extract the first template argument inside the argument pack.
template <typename... T>
struct get_first_type_in_pack {
  using type = std::tuple_element_t<0, std::tuple<T...>>;
};

/// Remove const, volatile qualifiers and references.
template <typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

/// Default template left undefined to avoid pages of compiler template errors when substitution fails.
template <typename T>
struct value_type_of_container;

template <template <typename...> class T, typename... U>
struct value_type_of_container<T<U...>> {
  using type = const typename get_first_type_in_pack<U...>::type;
};

template <template <typename, size_t> class T, typename U, size_t N>
struct value_type_of_container<T<U, N>> {
  using type = const U;
};

template <typename T>
struct value_type_of_impl {
  using type = typename value_type_of_container<remove_cvref_t<T>>::type;
};

template <typename T, size_t N>
struct value_type_of_impl<T (&)[N]> {
  using type = T;
};

/// Deduces the value type of different containers.
template <typename T>
using value_type_of_t = typename value_type_of_impl<T>::type;

/// Checks if T is compatible with a span of integer types.
template <typename T, typename = void>
struct is_integral_span_compatible : std::false_type {
};

template <typename T>
struct is_integral_span_compatible<T,
                                   std::enable_if_t<std::is_convertible<T, span<value_type_of_t<T>>>::value &&
                                                    std::is_integral<value_type_of_t<T>>::value>> : std::true_type {
};

/// Checks if T is compatible with a span of arithmetic types.
template <typename T, typename = void>
struct is_arithmetic_span_compatible : std::false_type {
};

template <typename T>
struct is_arithmetic_span_compatible<T,
                                     std::enable_if_t<std::is_convertible<T, span<value_type_of_t<T>>>::value &&
                                                      std::is_arithmetic<value_type_of_t<T>>::value>> : std::true_type {
};

/// Checks if T is compatible with a span of complex floating points (which are not arithmetic types).
template <typename T, typename = void>
struct is_complex_span_compatible : std::false_type {
};

template <typename T>
struct is_complex_span_compatible<
    T,
    std::enable_if_t<std::is_convertible<T, span<value_type_of_t<T>>>::value && is_complex<value_type_of_t<T>>::value>>
  : std::true_type {
};

/// Checks if T is compatible with a span.
template <typename T, typename = void>
struct is_span_compatible : std::false_type {
};

template <typename T>
struct is_span_compatible<T, std::enable_if_t<std::is_convertible<T, span<value_type_of_t<T>>>::value>>
  : std::true_type {
};

} // namespace detail
} // namespace srsvec
} // namespace srsran
