/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "coroutine.h"
#include "detail/event_impl.h"
#include <utility>

namespace srsgnb {

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
  const Data& get()
  {
    srsgnb_sanity_check(is_set(), "trying to get manual_event<Data> data for non-set event");
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

} // namespace srsgnb
