/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_MOVE_CALLBACK_H
#define SRSRAN_MOVE_CALLBACK_H

#include "srsgnb/adt/detail/type_storage.h"
#include "srsgnb/support/srsran_assert.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <stdint.h>
#include <type_traits>

namespace srsgnb {

/// Size of the small buffer embedded in "move_callback<R(Args...)>", used to store functors without calling "new"
constexpr size_t default_move_callback_buffer_size = 32;

/**
 * move_callback<R(Args...)> declaration
 * @tparam Signature Functor signature with format: R(Args...)
 * @tparam Capacity Small buffer size embedded into the move_callback type
 * @tparam ForbidAlloc Determines whether malloc of the Functor is allowed or forbidden. In case of ForbidAlloc==true,
 *                     the compiler forbids storing functors, such that "sizeof(Functor) > Capacity"
 */
template <class Signature, size_t Capacity = default_move_callback_buffer_size, bool ForbidAlloc = false>
class move_callback;

namespace task_details {

/// Base vtable for move/call/destroy operations over the functor stored in "move_callback<R(Args...)"
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

//! specialization of move/call/destroy operations for when the "move_callback<R(Args...)>" is empty
template <typename R, typename... Args>
class empty_table_t final : public oper_table_t<R, Args...>
{
public:
  constexpr empty_table_t() = default;
  R         call(void* src, Args... args) const final
  {
    srsran_assertion_failure("bad function call (cause: function ptr is empty)");
  }
  void move(void* src, void* dest) const final {}
  void dtor(void* src) const final {}
  bool is_in_small_buffer() const final { return true; }
};

//! specialization of move/call/destroy operations for when the functor is stored in "move_callback<R(Args...)>" buffer
template <typename FunT, typename R, typename... Args>
class smallbuffer_table_t final : public oper_table_t<R, Args...>
{
public:
  constexpr smallbuffer_table_t() = default;
  R    call(void* src, Args... args) const final { return (*static_cast<FunT*>(src))(std::forward<Args>(args)...); }
  void move(void* src, void* dest) const final
  {
    ::new (dest) FunT(std::move(*static_cast<FunT*>(src)));
    static_cast<FunT*>(src)->~FunT();
  }
  void dtor(void* src) const final { static_cast<FunT*>(src)->~FunT(); }
  bool is_in_small_buffer() const final { return true; }
};

//! move/call/destroy operations for when the functor is stored outside of "move_callback<R(Args...)>" buffer
template <typename FunT, typename R, typename... Args>
class heap_table_t final : public oper_table_t<R, Args...>
{
public:
  constexpr heap_table_t() = default;
  R    call(void* src, Args... args) const final { return (**static_cast<FunT**>(src))(std::forward<Args>(args)...); }
  void move(void* src, void* dest) const final
  {
    *static_cast<FunT**>(dest) = *static_cast<FunT**>(src);
    *static_cast<FunT**>(src)  = nullptr;
  }
  void dtor(void* src) const final { delete (*static_cast<FunT**>(src)); }
  bool is_in_small_buffer() const final { return false; }
};

//! Metafunction to check if a type is an instantiation of move_callback<R(Args...)>
template <class>
struct is_move_callback : std::false_type {};
template <class Sig, size_t Capacity>
struct is_move_callback<move_callback<Sig, Capacity> > : std::true_type {};

//! metafunctions to enable different ctor implementations depending on whether the callback fits the small buffer
template <typename T, size_t Cap, typename FunT = typename std::decay<T>::type>
using enable_if_small_capture =
    typename std::enable_if<sizeof(FunT) <= Cap and not is_move_callback<FunT>::value, bool>::type;
template <typename T, size_t Cap, typename FunT = typename std::decay<T>::type>
using enable_if_big_capture =
    typename std::enable_if < Cap<sizeof(FunT) and not is_move_callback<FunT>::value, bool>::type;

} // namespace task_details

template <class R, class... Args, size_t Capacity, bool ForbidAlloc>
class move_callback<R(Args...), Capacity, ForbidAlloc>
{
  static constexpr size_t capacity = Capacity >= sizeof(void*) ? Capacity : sizeof(void*); ///< size of buffer
  using storage_t                  = typename std::aligned_storage<capacity, alignof(detail::max_alignment_t)>::type;
  using oper_table_t               = task_details::oper_table_t<R, Args...>;
  static const task_details::empty_table_t<R, Args...> empty_table;

public:
  move_callback() noexcept : oper_ptr(&empty_table) {}

  //! Called when T capture fits the move_callback buffer
  template <typename T, task_details::enable_if_small_capture<T, capacity> = true>
  move_callback(T&& function) noexcept
  {
    using FunT = typename std::decay<T>::type;
    static const task_details::smallbuffer_table_t<FunT, R, Args...> small_oper_table{};
    oper_ptr = &small_oper_table;
    ::new (&buffer) FunT(std::forward<T>(function));
  }

  //! Called when T capture does not fit the move_callback buffer
  template <typename T, task_details::enable_if_big_capture<T, capacity> = true>
  move_callback(T&& function)
  {
    static_assert(
        not ForbidAlloc,
        "Failed to store provided callback in std::move_callback specialization that forbids heap allocations.");
    using FunT = typename std::decay<T>::type;
    static const task_details::heap_table_t<FunT, R, Args...> heap_oper_table{};
    oper_ptr = &heap_oper_table;
    ptr      = static_cast<void*>(new FunT{std::forward<T>(function)});
  }

  move_callback(move_callback&& other) noexcept : oper_ptr(other.oper_ptr)
  {
    other.oper_ptr = &empty_table;
    oper_ptr->move(&other.buffer, &buffer);
  }

  ~move_callback() { oper_ptr->dtor(&buffer); }

  move_callback& operator=(move_callback&& other) noexcept
  {
    oper_ptr->dtor(&buffer);
    oper_ptr       = other.oper_ptr;
    other.oper_ptr = &empty_table;
    oper_ptr->move(&other.buffer, &buffer);
    return *this;
  }

  R operator()(Args... args) const noexcept { return oper_ptr->call(&buffer, std::forward<Args>(args)...); }

  bool is_empty() const { return oper_ptr == &empty_table; }
  bool is_in_small_buffer() const { return oper_ptr->is_in_small_buffer(); }

private:
  union {
    mutable storage_t buffer;
    void*             ptr;
  };
  const oper_table_t* oper_ptr;
};

template <typename R, typename... Args, size_t Capacity, bool ForbidAlloc>
const task_details::empty_table_t<R, Args...> move_callback<R(Args...), Capacity, ForbidAlloc>::empty_table{};

/// Generic moveable task
using move_task_t = move_callback<void(), 64>;

} // namespace srsgnb

#endif // SRSRAN_MOVE_CALLBACK_H
