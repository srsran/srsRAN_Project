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

#include "srsran/support/detail/type_list.h"

namespace srsran {

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

} // namespace srsran
