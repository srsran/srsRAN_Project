/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/adt/optional.h"
#include "srsran/support/async/coroutine.h"

namespace srsran {

template <typename Data>
class event_receiver;

template <typename Data>
class event_sender
{
  event_sender(event_receiver<Data>& receiver_) : receiver(&receiver_) {}

public:
  event_sender(const event_sender<Data>&) = delete;
  event_sender(event_sender<Data>&& other) noexcept : receiver(std::exchange(other.receiver, nullptr)) {}
  event_sender& operator=(const event_sender<Data>&) = delete;
  event_sender& operator=(event_sender<Data>&& other) noexcept
  {
    receiver = std::exchange(other.receiver, nullptr);
    return *this;
  }
  ~event_sender()
  {
    if (receiver != nullptr) {
      receiver->cancel();
    }
  }

  template <typename Result>
  void set(Result&& data)
  {
    static_assert(std::is_convertible<Result, Data>::value, "Unable to convert Result to Data type");
    srsran_assert(receiver != nullptr, "Sender not connected to receiver");
    receiver->set_result(std::forward<Result>(data));
    receiver = nullptr;
  }

private:
  friend class event_receiver<Data>;

  event_receiver<Data>* receiver = nullptr;
};

template <>
class event_sender<void>
{
  event_sender(event_receiver<void>& receiver_) : receiver(&receiver_) {}

public:
  event_sender(const event_sender<void>&) = delete;
  event_sender(event_sender<void>&& other) noexcept : receiver(std::exchange(other.receiver, nullptr)) {}
  event_sender& operator=(const event_sender<void>&) = delete;
  event_sender& operator=(event_sender<void>&& other) noexcept
  {
    receiver = std::exchange(other.receiver, nullptr);
    return *this;
  }
  ~event_sender();

  void set();

private:
  friend class event_receiver<void>;

  event_receiver<void>* receiver = nullptr;
};

template <typename Data>
class event_receiver
{
  enum class state_t : int8_t { uninit, unset, cancelled, set };

public:
  event_receiver()                                 = default;
  event_receiver(const event_receiver&)            = delete;
  event_receiver(event_receiver&&)                 = delete;
  event_receiver& operator=(const event_receiver&) = delete;
  event_receiver& operator=(event_receiver&&)      = delete;
  ~event_receiver()
  {
    srsran_assert(state != state_t::unset, "Event receiver destroyed without being set or cancelled");
  }

  event_sender<Data> get_sender()
  {
    srsran_assert(state == state_t::uninit, "Only one sender can be connected to an event receiver");
    state = state_t::unset;
    return event_sender<Data>(*this);
  }

  bool completed() const { return state == state_t::set or state == state_t::cancelled; }
  bool successful() const { return state == state_t::set; }
  bool aborted() const { return state == state_t::cancelled; }

  const Data& result() const
  {
    srsran_assert(successful(), "The event result has not be set");
    return data.value();
  }

  // Awaiter/Awaitable interface.
  event_receiver<Data>& get_awaiter() { return *this; }
  bool                  await_ready() const { return completed(); }
  void                  await_suspend(coro_handle<> c)
  {
    // Store suspending coroutine.
    suspended_handle = c;
  }
  const optional<Data>& await_resume() { return data; }

private:
  friend class event_sender<Data>;

  void set_result(const Data& result)
  {
    srsran_assert(state == state_t::unset, "The event cannot be set multiple times");
    state      = state_t::set;
    this->data = result;

    // Awake the awaiting coroutine.
    if (not suspended_handle.empty()) {
      suspended_handle.resume();
    }
  }
  void cancel()
  {
    srsran_assert(state == state_t::unset, "The event cannot be set multiple times");
    state = state_t::cancelled;

    // Awake the awaiting coroutine.
    if (not suspended_handle.empty()) {
      suspended_handle.resume();
    }
  }

  state_t        state = state_t::uninit;
  coro_handle<>  suspended_handle;
  optional<Data> data;
};

template <>
class event_receiver<void>
{
  enum class state_t : int8_t { uninit, unset, cancelled, set };

public:
  event_receiver()                                 = default;
  event_receiver(const event_receiver&)            = delete;
  event_receiver(event_receiver&&)                 = delete;
  event_receiver& operator=(const event_receiver&) = delete;
  event_receiver& operator=(event_receiver&&)      = delete;
  ~event_receiver()
  {
    srsran_assert(state != state_t::unset, "Event receiver destroyed without being set or cancelled");
  }

  event_sender<void> get_sender()
  {
    srsran_assert(state == state_t::uninit, "Only one sender can be connected to an event receiver");
    state = state_t::unset;
    return event_sender<void>(*this);
  }

  bool completed() const { return state == state_t::set or state == state_t::cancelled; }
  bool successful() const { return state == state_t::set; }
  bool aborted() const { return state == state_t::cancelled; }

  // Awaiter/Awaitable interface.
  event_receiver<void>& get_awaiter() { return *this; }
  bool                  await_ready() const { return completed(); }
  void                  await_suspend(coro_handle<> c)
  {
    // Store suspending coroutine.
    suspended_handle = c;
  }
  bool await_resume() { return state == state_t::set; }

private:
  friend class event_sender<void>;

  void set_result()
  {
    srsran_assert(state == state_t::unset, "The event cannot be set multiple times");
    state = state_t::set;

    // Awake the awaiting coroutine.
    if (not suspended_handle.empty()) {
      suspended_handle.resume();
    }
  }
  void cancel()
  {
    srsran_assert(state == state_t::unset, "The event cannot be set multiple times");
    state = state_t::cancelled;

    // Awake the awaiting coroutine.
    if (not suspended_handle.empty()) {
      suspended_handle.resume();
    }
  }

  state_t       state = state_t::uninit;
  coro_handle<> suspended_handle;
};

inline event_sender<void>::~event_sender()
{
  if (receiver != nullptr) {
    receiver->cancel();
  }
}

inline void event_sender<void>::set()
{
  srsran_assert(receiver != nullptr, "Sender not connected to receiver");
  receiver->set_result();
  receiver = nullptr;
}

} // namespace srsran