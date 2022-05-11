/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PROMISE_DATA_H
#define SRSGNB_PROMISE_DATA_H

#include "srsgnb/support/srsran_assert.h"
#include <memory>

namespace srsgnb {

namespace detail {

/// Class that holds promise result value
template <typename R, typename Base>
struct promise_data : public Base {
  promise_data()                          = default;
  promise_data(const promise_data& other) = delete;
  promise_data(promise_data&& other)      = delete;
  promise_data& operator=(const promise_data& other) = delete;
  promise_data& operator=(promise_data&& other) = delete;
  ~promise_data()
  {
    if (Base::has_value) {
      reinterpret_cast<R*>(&mem)->~R();
    }
  }

  /// Set Return value
  template <typename U>
  void return_value(U&& u)
  {
    srsran_sanity_check(not Base::has_value, "Return called more than once.");
    new (&mem) R(std::forward<U>(u));
    Base::has_value = true;
  }

  /// Get Return value
  const R& get() const&
  {
    srsran_assert(Base::has_value, "Trying to extract result from unset Promise");
    return *reinterpret_cast<const R*>(&mem);
  }
  R get() &&
  {
    srsran_assert(Base::has_value, "Trying to extract result from unset Promise");
    return std::move(*reinterpret_cast<R*>(&mem));
  }

private:
  /// result storage.
  std::aligned_storage_t<sizeof(R), alignof(R)> mem;
};

/// Specialization for when Return type is void
template <typename Base>
struct promise_data<void, Base> : public Base {
  void return_value() { Base::has_value = true; }
};

} // namespace detail

} // namespace srsgnb

#endif // SRSGNB_PROMISE_DATA_H
