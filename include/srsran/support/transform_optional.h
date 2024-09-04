/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  return std::invoke(fn, in.value(), std::forward<Args>(args)...);
}
} // namespace srsran
