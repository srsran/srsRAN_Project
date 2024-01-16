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

#include <type_traits>
#include <utility>

namespace srsran {

namespace detail {

template <typename Callable>
struct scope_exit {
  template <typename C>
  explicit scope_exit(C&& f_) : exit_function(std::forward<C>(f_))
  {
  }
  scope_exit(scope_exit&& rhs) noexcept : exit_function(std::move(rhs.exit_function)), active(rhs.active)
  {
    rhs.release();
  }
  scope_exit(const scope_exit&)                = delete;
  scope_exit& operator=(const scope_exit&)     = delete;
  scope_exit& operator=(scope_exit&&) noexcept = delete;
  ~scope_exit()
  {
    if (active) {
      exit_function();
    }
  }

  void release() { active = false; }

private:
  Callable exit_function;
  bool     active = true;
};

} // namespace detail

/// \brief Defers callable call to scope exit
/// \tparam Callable Any type with a call operator
/// \param callable function that is called at scope exit
/// \return object that has to be stored in a local variable
template <typename Callable>
detail::scope_exit<std::decay_t<Callable>> make_scope_exit(Callable&& callable)
{
  return detail::scope_exit<std::decay_t<Callable>>{std::forward<Callable>(callable)};
}

#define DEFER(FUNC) auto on_exit_call##__LINE__ = srsran::make_scope_exit([&]() { FUNC })

} // namespace srsran
