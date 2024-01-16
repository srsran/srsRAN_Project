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

#include "manual_event.h"
#include "srsran/adt/expected.h"
#include "srsran/support/timers.h"

namespace srsran {

template <typename T>
class async_single_event_observer;

/// \brief Publisher of async events. One single subscriber/listener/observer of type \c async_event_subscriber can
/// subscribe to this class to listen for incoming messages.
template <typename T>
class async_event_source
{
public:
  explicit async_event_source(timer_factory timer_db) : running_timer(timer_db.create_timer()) {}
  async_event_source(const async_event_source&)            = delete;
  async_event_source(async_event_source&&)                 = delete;
  async_event_source& operator=(const async_event_source&) = delete;
  async_event_source& operator=(async_event_source&&)      = delete;
  ~async_event_source() { srsran_assert(not has_subscriber(), "Observers must not outlive event sources"); }

  /// \brief Checks if there is any listener registered.
  bool has_subscriber() const { return sub != nullptr; }

  /// \brief Forwards a result to the registered listener/subscriber. If no subscriber is registered, returns false.
  template <typename U>
  bool set(U&& u)
  {
    if (has_subscriber()) {
      running_timer.stop();
      auto old_sub = sub;
      remove_observer();
      old_sub->event.set(std::forward<U>(u));
      return true;
    }
    srslog::fetch_basic_logger("ALL").debug("Setting transaction result, but no subscriber is listening");
    return false;
  }

private:
  friend class async_single_event_observer<T>;

  void set_observer(async_single_event_observer<T>& sub_)
  {
    srsran_assert(not has_subscriber(), "This class only allows one observer/sink/listener per transaction");
    srsran_assert(not sub_.connected(), "The observer cannot be already registered");
    if (sub_.complete()) {
      // observer finished the previous transaction. Its last result can be reset.
      sub_.reset();
    }
    sub         = &sub_;
    sub->parent = this;
  }

  template <typename U>
  void set_observer(async_single_event_observer<T>& sub_, std::chrono::milliseconds time_to_cancel, U&& cancelled_value)
  {
    set_observer(sub_);
    // Setup timeout.
    running_timer.set(time_to_cancel, [this, c = std::forward<U>(cancelled_value)](timer_id_t /**/) mutable {
      set(std::forward<U>(c));
    });
    running_timer.run();
  }

  void remove_observer()
  {
    srsran_assert(has_subscriber(), "Unsubscribe called but no subscriber is registered");
    sub->parent = nullptr;
    sub         = nullptr;
  }

  async_single_event_observer<T>* sub = nullptr;
  unique_timer                    running_timer;
};

/// \brief Awaitable type that implements a observer/subscriber/listener for a single async event. This awaitable
/// is single-use, meaning that after it auto-unsubscribes after receiving a message from the event source/publisher.
template <typename T>
class async_single_event_observer
{
public:
  using result_type  = T;
  using awaiter_type = typename manual_event<result_type>::awaiter_type;

  async_single_event_observer() = default;
  async_single_event_observer(async_event_source<T>& publisher) { subscribe_to(publisher); }
  async_single_event_observer(async_single_event_observer&& other)                 = delete;
  async_single_event_observer(const async_single_event_observer& other)            = delete;
  async_single_event_observer& operator=(async_single_event_observer&& other)      = delete;
  async_single_event_observer& operator=(const async_single_event_observer& other) = delete;
  ~async_single_event_observer()
  {
    if (parent != nullptr) {
      parent->remove_observer();
    }
  }

  /// \brief Subscribes this sink/observer/listener to an \c async_event_source. Only one simultaneous subscriber is
  /// allowed.
  void subscribe_to(async_event_source<T>& publisher) { publisher.set_observer(*this); }

  /// \brief Subscribes this observer/listener to an \c async_event_source and sets a timeout for automatic
  /// unsubscription. Only one simultaneous subscriber is allowed.
  template <typename U>
  void subscribe_to(async_event_source<T>& publisher, std::chrono::milliseconds time_to_cancel, U&& cancelled_value)
  {
    publisher.set_observer(*this, time_to_cancel, std::forward<U>(cancelled_value));
  }

  /// \brief Checks whether this sink has been registered to an event source.
  bool connected() const { return parent != nullptr; }

  /// \brief Checks if result has been set successfully by the event source.
  bool complete() const { return event.is_set(); }

  /// \brief Clears the stored result.
  void reset() { event.reset(); }

  /// \brief Result set by event source.
  const T& result() const&
  {
    srsran_assert(complete(), "Trying to fetch result of incomplete transaction");
    return event.get();
  }
  T result() &&
  {
    srsran_assert(complete(), "Trying to fetch result of incomplete transaction");
    return std::move(event).get();
  }

  /// Awaiter interface.
  awaiter_type get_awaiter() { return event.get_awaiter(); }

private:
  friend class async_event_source<T>;

  async_event_source<T>* parent = nullptr;
  manual_event<T>        event;
};

} // namespace srsran