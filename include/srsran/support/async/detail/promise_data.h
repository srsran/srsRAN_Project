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

#include "srsran/support/srsran_assert.h"
#include <memory>

namespace srsran {

namespace detail {

/// Class that holds promise result value
template <typename R, typename Base>
struct promise_data : public Base {
  promise_data()                                     = default;
  promise_data(const promise_data& other)            = delete;
  promise_data(promise_data&& other)                 = delete;
  promise_data& operator=(const promise_data& other) = delete;
  promise_data& operator=(promise_data&& other)      = delete;
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

} // namespace srsran
