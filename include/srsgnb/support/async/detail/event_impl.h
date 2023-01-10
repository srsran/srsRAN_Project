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

#include "../coroutine.h"
#include <utility>

namespace srsgnb {

namespace detail {

/// Iterates over linked list of awaiters and calls resume for each awaiter stored coroutine handle
template <typename AwaiterType>
void flush_awaiter_list(AwaiterType* current)
{
  while (current != nullptr) {
    auto* next = current->next;
    current->suspended_handle.resume();
    current = next;
  }
}

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

} // namespace srsgnb
