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

#include "srsran/support/executors/task_executor.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

namespace detail {

template <typename T, typename... Args>
class is_task_executor_ptr
{
  template <typename U>
  static auto test(U&& ptr) -> decltype((void)ptr->execute(std::declval<Args>()...), std::true_type{});
  template <typename U>
  static auto test(const U* ptr)
      -> decltype((void)std::declval<U*>()->execute(std::declval<Args>()...), std::true_type{});

  static std::false_type test(...);

public:
  static constexpr bool value = decltype(test(std::declval<T>()))::value;
};

template <typename T>
class is_task_executor_ptr<T>
{
  template <typename U>
  static auto test(U&& ptr) -> decltype((void)ptr->execute(std::declval<unique_task>()), std::true_type{});
  template <typename U>
  static auto test(const U* ptr)
      -> decltype((void)std::declval<U*>()->execute(std::declval<unique_task>()), std::true_type{});

  static std::false_type test(...);

public:
  static constexpr bool value = decltype(test(std::declval<T>()))::value;
};

/// Helper method to call execute for a task_executor that can be either a pointer, reference or value.
template <typename ExecType, typename... Args>
[[nodiscard]] bool invoke_execute(ExecType&& exec, Args&&... args)
{
  if constexpr (is_task_executor_ptr<ExecType, Args&&...>::value) {
    return exec->execute(std::forward<Args>(args)...);
  } else {
    return exec.execute(std::forward<Args>(args)...);
  }
}

/// Helper method to call defer for a task_executor that can be either a pointer, reference or value.
template <typename ExecType, typename... Args>
[[nodiscard]] bool invoke_defer(ExecType&& exec, Args&&... args)
{
  if constexpr (is_task_executor_ptr<ExecType, Args&&...>::value) {
    return exec->defer(std::forward<Args>(args)...);
  } else {
    return exec.defer(std::forward<Args>(args)...);
  }
}

// Helper function to convert a task executor object, that can be a raw pointer or unique_ptr to a reference.
template <typename T, std::enable_if_t<is_task_executor_ptr<T>::value, int> = 0>
auto& get_task_executor_ref(T&& exec)
{
  return *exec;
}
template <typename T, std::enable_if_t<not is_task_executor_ptr<T>::value, int> = 0>
auto get_task_executor_ref(T&& exec)
{
  return std::forward<T>(exec);
}

template <typename T>
class task_executor_has_can_run_task_inline
{
  template <typename U>
  static auto test(const U& u) -> decltype(get_task_executor_ref(u).can_run_task_inline(), std::true_type{});

  static std::false_type test(...);

public:
  static constexpr bool value = decltype(test(std::declval<T>()))::value;
};

// Helper function to determine whether an executor allows a task to be run inline, given the caller thread.
template <typename Executor, std::enable_if_t<task_executor_has_can_run_task_inline<Executor>::value, int> = 0>
bool executor_lets_run_task_inline(const Executor& exec)
{
  return get_task_executor_ref(exec).can_run_task_inline();
}
template <typename Executor, std::enable_if_t<not task_executor_has_can_run_task_inline<Executor>::value, int> = 0>
bool executor_lets_run_task_inline(const Executor& exec)
{
  return false;
}

} // namespace detail

} // namespace srsran
