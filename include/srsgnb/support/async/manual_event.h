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

namespace detail {

/// Base class implementation with functionality that is common to manual_event<R> and manual_event_flag
/// Awaiters for the event are stored in an intrusive linked list, where each awaiter is stored in the embedded
/// memory buffer of the coroutine frame
class manual_event_common
{
public:
  struct awaiter_common {
    explicit awaiter_common(manual_event_common& event_) : event(&event_) {}
    bool await_ready() const { return event->is_set(); }
    void await_suspend(coro_handle<> c)
    {
      // Store suspending coroutine.
      suspended_handle = c;

      // enqueue awaiter
      next = static_cast<awaiter_common*>(std::exchange(event->state, static_cast<void*>(this)));
    }

    manual_event_common* event;
    awaiter_common*      next;
    coro_handle<>        suspended_handle;
  };

  manual_event_common()                                             = default;
  manual_event_common(const manual_event_common& event_)            = delete;
  manual_event_common& operator=(const manual_event_common& event_) = delete;
  ~manual_event_common() {}

  /// Resets event state
  void reset()
  {
    if (is_set()) {
      state = nullptr;
    }
  }

  /// Check if event value is set
  bool is_set() const { return state == this; }

protected:
  // Event state. Can be in 3 states:
  // - this - event is set
  // - nullptr - event is not set and with no awaiters pending
  // - other - state is not set. Points to first element in linked list of awaiters
  void* state = nullptr;

  /// triggers all awaiting coroutines after event is set
  void flush()
  {
    if (is_set()) {
      return;
    }
    auto old_state = std::exchange(state, this);
    flush_awaiter_list(static_cast<awaiter_common*>(old_state));
  }
};

} // namespace detail

/// @brief Awaitable type that can be manually set to one of the states: "set" or "unset"
/// Coroutines will automatically resume when they await on a manual_event_flag that is in "set" state
class manual_event_flag : public detail::manual_event_common
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

/// @brief Similar to "manual_event_flag", but the data passed to .set() is propagated to awaiting coroutine
/// @tparam Data type of data passed to awaiting coroutine
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

} // namespace srsgnb
