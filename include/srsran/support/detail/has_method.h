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

#include <type_traits>

namespace srsran {
namespace detail {

/// Helper metafunction to determine if a type T can be dereferenced (i.e., have an operator* defined).
template <typename T, typename = void>
struct is_dereferenceable_impl : std::false_type {};

/// Specialization for types that can be dereferenced.
template <typename T>
struct is_dereferenceable_impl<T, std::void_t<decltype(*std::declval<T>())>> : std::true_type {};

} // namespace detail

/// Determines whether the type T can be dereferenced (i.e., has an operator* defined).
template <typename T>
using is_dereferenceable = detail::is_dereferenceable_impl<T>;

/// Creates a trait that checks if a provided type T has a method with the specified name and signature.
#define CREATE_TRAIT_HAS_METHOD(trait_name, method_name)                                                               \
  template <typename, typename T, typename = void>                                                                     \
  struct trait_name : std::false_type {};                                                                              \
                                                                                                                       \
  template <typename T, typename Ret, typename... Args>                                                                \
  struct trait_name<T, Ret(Args...), std::void_t<decltype(std::declval<T>().method_name(std::declval<Args>()...))>>    \
    : std::is_same<decltype(std::declval<T>().method_name(std::declval<Args>()...)), Ret> {}

} // namespace srsran
