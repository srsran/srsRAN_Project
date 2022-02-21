
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
  using args        = type_list<Args...>;
};

/// Specialization for mutable class methods
template <typename Ret, typename Obj, typename... Args>
struct function_signature<Ret (Obj::*)(Args...)> {
  using return_type = Ret;
  using args        = type_list<Args...>;
};

/// Specialization for const class methods
template <typename Ret, typename Obj, typename... Args>
struct function_signature<Ret (Obj::*)(Args...) const> {
  using return_type = Ret;
  using args        = type_list<Args...>;
};

template <typename Sig>
using function_args_t = typename function_signature<Sig>::args;

template <typename Sig>
using function_return_t = typename function_signature<Sig>::return_type;

template <typename T>
auto callable_arguments() -> typename function_signature<decltype(&std::decay_t<T>::operator())>::type;

} // namespace detail

} // namespace srsgnb

#endif // SRSGNB_FUNCTION_SIGNATURE_H
