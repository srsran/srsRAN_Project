/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_FUNCTION_SIGNATURE_H
#define SRSGNB_FUNCTION_SIGNATURE_H

#include "srsgnb/support/detail/type_list.h"

namespace srsgnb {

namespace detail {

/// Type to store arguments and return of callable type Sig
template <typename Sig>
struct function_signature;

/// Specialization for free functions
template <typename Ret, typename... Args>
struct function_signature<Ret(Args...)> {
  using return_type = Ret;
  using arg_types   = type_list<Args...>;
};

/// Specialization for mutable class methods
template <typename Ret, typename Obj, typename... Args>
struct function_signature<Ret (Obj::*)(Args...)> {
  using return_type = Ret;
  using arg_types   = type_list<Args...>;
};

/// Specialization for const class methods
template <typename Ret, typename Obj, typename... Args>
struct function_signature<Ret (Obj::*)(Args...) const> {
  using return_type = Ret;
  using arg_types   = type_list<Args...>;
};

template <typename Sig>
using function_args_t = typename function_signature<Sig>::arg_types;

template <typename Sig>
using function_return_t = typename function_signature<Sig>::return_type;

template <typename T>
auto callable_arguments() -> typename function_signature<decltype(&std::decay_t<T>::operator())>::arg_types;

} // namespace detail

} // namespace srsgnb

#endif // SRSGNB_FUNCTION_SIGNATURE_H
