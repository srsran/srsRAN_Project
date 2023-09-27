/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  }

  state_t        state = state_t::uninit;
  coro_handle<>  suspended_handle;
  optional<Data> data;
};

} // namespace srsran