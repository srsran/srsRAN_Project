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

/// \file
/// \brief Monad-inspired std::optional transformation.

#pragma once

#include <functional>
#include <optional>
#include <type_traits>

namespace srsran {

/// \brief Transforms an std::optional into another std::optional of possibly a different type.
///
/// Applies a function to the value of an optional, if it exists, and returns a new optional with the result. If the
/// original optional is uninitialized, then returns an empty optional.
/// \tparam T        Base type of the input optional.
/// \tparam F        Callable type.
/// \tparam Args     Type of the extra (not mandatory) arguments of the callable function.
/// \param[in] in    Optional object to be transformed, if it has value. The base type should be compatible with the
///                  type of the first input of \c fn.
/// \param[in] fn    Invoked function. It must take at least one argument of the same type as the base type of \c in. It
///                  can possibly take other arguments.
/// \param[in] args  Non-mandatory extra arguments of \c fn.
/// \return An optional initialized to <tt>fn(in.value(), args) if \c in has a value, a null optional otherwise. The
/// base type of the output optional is inferred from the result type of \c fn.
template <typename T, typename F, typename... Args>
std::optional<std::invoke_result_t<F, T, Args&&...>>
transform_optional(const std::optional<T>& in, F&& fn, Args&&... args)
{
  static_assert(std::is_invocable_v<F, T, Args&&...>);
  if (!in.has_value()) {
    return std::nullopt;
  }

  return std::invoke(fn, *in, std::forward<Args>(args)...);
}

/// \brief Applies a function to the value of an std::optional.
///
/// Applies a function to the value of an optional, if it exists, and returns the result. If the
/// original optional is uninitialized, then returns a given default value.
/// \tparam T         Base type of the input optional.
/// \tparam U         Type of the default output.
/// \tparam F         Callable type.
/// \tparam Args      Type of the extra (not mandatory) arguments of the callable function.
/// \param[in] in     Optional object the function is applied to, if it has value. The base type should be compatible
///                   with the type of the first input of \c fn.
/// \param[in] d_out  Default value to be returned when \c in is empty. It's type must be compatible with the return
///                   type of \c fn.
/// \param[in] fn     Invoked function. It must take at least one argument of the same type as the base type of \c in.
///                   It can possibly take other arguments.
/// \param[in] args   Non-mandatory extra arguments of \c fn.
/// \return The result of <tt>fn(in.value(), args) if \c in has a value, a \c d_out otherwise. The output type is
/// inferred from the result type of \c fn.
template <typename T, typename U, typename F, typename... Args>
std::invoke_result_t<F, T, Args&&...> evaluate_or(const std::optional<T>& in, const U& d_out, F&& fn, Args&&... args)
{
  static_assert(std::is_invocable_v<F, T, Args&&...>);
  using UF = std::invoke_result_t<F, T, Args&&...>;
  static_assert(std::is_convertible_v<U, UF>);

  if (!in.has_value()) {
    return d_out;
  }

  return std::invoke(fn, *in, std::forward<Args>(args)...);
}
} // namespace srsran
