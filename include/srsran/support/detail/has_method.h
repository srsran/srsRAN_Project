/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

template <typename T, typename = void>
struct is_dereferenceable_impl : std::false_type {};

// Specialization for types that can be dereferenced (i.e., have an operator* defined).
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
    : std::is_same<decltype(std::declval<T>().method_name(std::declval<Args>()...)), Ret> {};

} // namespace srsran
