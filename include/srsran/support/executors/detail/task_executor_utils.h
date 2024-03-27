/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../srsran_assert.h"
#include "../task_executor.h"

namespace srsran {

namespace detail {

template <typename T>
class is_task_executor_ptr
{
  template <typename U>
  static auto test(U&& ptr) -> decltype(ptr->execute(std::declval<unique_task>()), std::true_type{});
  template <typename U>
  static auto test(const U* ptr)
      -> decltype(std::declval<U*>()->execute(std::declval<unique_task>()), std::true_type{});

  static std::false_type test(...);

public:
  static constexpr bool value = decltype(test(std::declval<T>()))::value;
};

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