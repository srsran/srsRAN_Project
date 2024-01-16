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

#include "coroutine.h"
#include "detail/event_impl.h"
#include <utility>

namespace srsran {

/// \brief Awaitable type that can be manually set to one of the states:
/// - "unset" state - Coroutines that await on an "unset" manual_event get suspended. They are only resumed when the
/// manual_event transitions to "set" state via the manual_event::set method.
/// - "set" state - Coroutines that await on a "set" manual_event do not get suspended and get automatically resumed.
/// Once "set", the awaiting coroutines have to option to extract the \c Data that was stored in the manual_event.
/// Multiple coroutines can simultaneously await this awaitable when in "unset" state. All the awaiting coroutines
/// will be resumed once the awaitable is set.
/// \tparam Data type of data passed to awaiting coroutine.
template <typename Data>
class manual_event : public detail::manual_event_common
{
public:
  using result_type = Data; ///< Stored value type

  /// Awaiter object type
  struct awaiter_type : public awaiter_common {
    using awaiter_common::awaiter_common;
    Data await_resume() const { return static_cast<manual_event*>(event)->data; }
  };

  /// Called to set event value, and trigger all awaiting coroutines
  template <typename U>
  void set(U&& u)
  {
    data = std::forward<U>(u);
    flush();
  }

  /// Called to object stored value when event is set
  const Data& get() const
  {
    srsran_sanity_check(is_set(), "trying to get manual_event<Data> data for non-set event");
    return data;
  }

  /// awaiter interface
  awaiter_type get_awaiter() { return awaiter_type{*this}; }

private:
  Data data; /// value set
};

/// \brief Specialization of manual_event<Data> for the case that the Data is void.
template <>
class manual_event<void> : public detail::manual_event_common
{
public:
  using result_type = void;
  struct awaiter_type : public awaiter_common {
    using awaiter_common::awaiter_common;
    void await_resume() {}
  };

  void set() { flush(); }

  /// awaiter interface
  awaiter_type get_awaiter() { return awaiter_type{*this}; }
};

using manual_event_flag = manual_event<void>;

} // namespace srsran
