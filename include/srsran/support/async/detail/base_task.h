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

#include "../coroutine.h"
#include "promise_data.h"
#include "unique_coroutine.h"

namespace srsran {

namespace detail {

struct task_promise_base {
  bool          has_value = false; ///< If result of task has been stored
  coro_handle<> continuation;      ///< Pending continuation task

  /// Check if return has been stored
  bool ready() const { return has_value; }
};

/// Common functionality to lazy and eager async tasks.
template <typename Derived, typename Result>
class common_task_crtp
{
public:
  using result_type = Result;

  /// Checks if async task has an handle.
  bool empty() const { return derived().handle.empty(); }

  /// Returns true if base_task is complete
  bool ready() const { return empty() or derived().handle.promise().ready(); }

  /// Called to get result of task once it is complete
  template <typename Res = std::decay_t<result_type>, std::enable_if_t<not std::is_same<Res, void>::value, bool> = true>
  const Res& get() const&
  {
    srsran_assert(not empty() and derived().handle.promise().ready(), "Called task::get() for task that is not ready");
    return derived().handle.promise().get();
  }
  template <typename Res = std::decay_t<result_type>, std::enable_if_t<not std::is_same<Res, void>::value, bool> = true>
  Res get() &&
  {
    srsran_assert(not empty() and derived().handle.promise().ready(), "Called task::get() for task that is not ready");
    return std::move(derived().handle.promise()).get();
  }

  /// Awaiter interface.
  bool await_ready() const noexcept { return derived().handle.empty() or derived().handle.promise().ready(); }
  template <typename Res = result_type>
  detail::enable_if_nonvoid<Res> await_resume()
  {
    srsran_sanity_check(not empty(), "Resuming an empty base_task");
    return derived().handle.promise().get();
  }
  template <typename Res = result_type>
  detail::enable_if_void<Res> await_resume()
  {
    srsran_sanity_check(not empty(), "Resuming an empty base_task");
  }

private:
  Derived&       derived() { return static_cast<Derived&>(*this); }
  const Derived& derived() const { return static_cast<const Derived&>(*this); }
};

} // namespace detail

} // namespace srsran
