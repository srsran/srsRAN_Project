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

#include "srsran/support/error_handling.h"
#include <cstdint>

namespace srsran {

/// Size of the small buffer embedded in "unique_function<R(Args...)>", used to store functors without calling "new"
constexpr size_t default_unique_function_buffer_size = 32;

/// unique_function<R(Args...)> declaration
/// @tparam Signature Functor signature with format: R(Args...)
/// @tparam Capacity Small buffer size embedded into the unique_function type
/// @tparam ForbidAlloc Determines whether malloc of the Functor is allowed or forbidden. In case of ForbidAlloc==true,
///                     the compiler forbids storing functors, such that "sizeof(Functor) > Capacity"
template <class Signature, size_t Capacity = default_unique_function_buffer_size, bool ForbidAlloc = false>
class unique_function;

namespace task_details {

/// Base vtable for move/call/destroy operations over the functor stored in "unique_function<R(Args...)"
template <typename R, typename... Args>
class oper_table_t
{
public:
  constexpr oper_table_t()                         = default;
  virtual ~oper_table_t()                          = default;
  virtual R    call(void* src, Args... args) const = 0;
  virtual void move(void* src, void* dest) const   = 0;
  virtual void dtor(void* src) const               = 0;
  virtual bool is_in_small_buffer() const          = 0;
};

/// Specialization of move/call/destroy operations for when the "unique_function<R(Args...)>" is empty
template <typename R, typename... Args>
class empty_table_t final : public oper_table_t<R, Args...>
{
public:
  constexpr empty_table_t() = default;
  R call(void* src, Args... args) const override
  {
    srsran_terminate("bad function call (cause: function ptr is empty)");
  }
  void move(void* src, void* dest) const override {}
  void dtor(void* src) const override {}
  bool is_in_small_buffer() const override { return true; }
};

/// specialization of move/call/destroy operations for when the functor is stored in "unique_function<R(Args...)>"
/// buffer
template <typename FunT, typename R, typename... Args>
class smallbuffer_table_t final : public oper_table_t<R, Args...>
{
public:
  constexpr smallbuffer_table_t() = default;
  R    call(void* src, Args... args) const override { return (*static_cast<FunT*>(src))(std::forward<Args>(args)...); }
  void move(void* src, void* dest) const override
  {
    ::new (dest) FunT(std::move(*static_cast<FunT*>(src)));
    static_cast<FunT*>(src)->~FunT();
  }
  void dtor(void* src) const override { static_cast<FunT*>(src)->~FunT(); }
  bool is_in_small_buffer() const override { return true; }
};

/// move/call/destroy operations for when the functor is stored outside of "unique_function<R(Args...)>" buffer
template <typename FunT, typename R, typename... Args>
class heap_table_t final : public oper_table_t<R, Args...>
{
public:
  constexpr heap_table_t() = default;
  R call(void* src, Args... args) const override { return (**static_cast<FunT**>(src))(std::forward<Args>(args)...); }
  void move(void* src, void* dest) const override
  {
    *static_cast<FunT**>(dest) = *static_cast<FunT**>(src);
    *static_cast<FunT**>(src)  = nullptr;
  }
  void dtor(void* src) const override { delete (*static_cast<FunT**>(src)); }
  bool is_in_small_buffer() const override { return false; }
};

/// Metafunction to check if a type is an instantiation of unique_function<R(Args...)>
template <class>
struct is_unique_function : std::false_type {
};
template <class Sig, size_t Capacity>
struct is_unique_function<unique_function<Sig, Capacity>> : std::true_type {
};

/// Metafunctions to enable different ctor implementations depending on whether the callback fits the small buffer
template <typename T, size_t Cap, typename FunT = typename std::decay<T>::type>
using enable_if_small_capture =
    typename std::enable_if<sizeof(FunT) <= Cap and not is_unique_function<FunT>::value, bool>::type;
template <typename T, size_t Cap, typename FunT = typename std::decay<T>::type>
using enable_if_big_capture =
    typename std::enable_if < Cap<sizeof(FunT) and not is_unique_function<FunT>::value, bool>::type;

} // namespace task_details

template <class R, class... Args, size_t Capacity, bool ForbidAlloc>
class unique_function<R(Args...), Capacity, ForbidAlloc>
{
  static_assert(Capacity >= sizeof(void*), "unique_function embedded buffer is too small");
  using storage_t    = std::aligned_storage_t<Capacity, alignof(std::max_align_t)>;
  using oper_table_t = task_details::oper_table_t<R, Args...>;
  static const task_details::empty_table_t<R, Args...> empty_table;

public:
  static constexpr size_t capacity = Capacity; ///< size of buffer

  constexpr unique_function() noexcept : oper_ptr(&empty_table) {}

  /// Called when T capture fits the unique_function small buffer
  template <typename T, task_details::enable_if_small_capture<T, capacity> = true>
  unique_function(T&& function) noexcept(std::is_nothrow_move_constructible<T>::value)
  {
    using FunT = typename std::decay<T>::type;
    static const task_details::smallbuffer_table_t<FunT, R, Args...> small_oper_table{};
    oper_ptr = &small_oper_table;
    ::new (&buffer) FunT(std::forward<T>(function));
  }

  /// Called when T capture does not fit the unique_function small buffer
  template <typename T, task_details::enable_if_big_capture<T, capacity> = true>
  unique_function(T&& function) noexcept(std::is_nothrow_move_constructible<T>::value)
  {
    static_assert(
        not ForbidAlloc,
        "Failed to store provided callback in std::unique_function specialization that forbids heap allocations.");
    using FunT = typename std::decay<T>::type;
    static const task_details::heap_table_t<FunT, R, Args...> heap_oper_table{};
    oper_ptr = &heap_oper_table;
    ptr      = static_cast<void*>(new FunT{std::forward<T>(function)});
  }

  unique_function(unique_function&& other) noexcept : oper_ptr(other.oper_ptr)
  {
    other.oper_ptr = &empty_table;
    oper_ptr->move(&other.buffer, &buffer);
  }

  ~unique_function() { oper_ptr->dtor(&buffer); }

  unique_function& operator=(unique_function&& other) noexcept
  {
    oper_ptr->dtor(&buffer);
    oper_ptr       = other.oper_ptr;
    other.oper_ptr = &empty_table;
    oper_ptr->move(&other.buffer, &buffer);
    return *this;
  }

  /// Call operator
  R operator()(Args... args) const noexcept { return oper_ptr->call(&buffer, std::forward<Args>(args)...); }

  bool is_empty() const noexcept { return oper_ptr == &empty_table; }
  bool is_in_small_buffer() const noexcept { return oper_ptr->is_in_small_buffer(); }

private:
  union {
    mutable storage_t buffer;
    void*             ptr;
  };
  const oper_table_t* oper_ptr;
};

template <typename R, typename... Args, size_t Capacity, bool ForbidAlloc>
const task_details::empty_table_t<R, Args...> unique_function<R(Args...), Capacity, ForbidAlloc>::empty_table{};

constexpr size_t default_unique_task_buffer_size = 64;

/// Generic moveable task
using unique_task = unique_function<void(), default_unique_task_buffer_size>;

} // namespace srsran
