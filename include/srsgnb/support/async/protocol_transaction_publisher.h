/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "manual_event.h"
#include "srsgnb/support/timers.h"

namespace srsgnb {

template <typename T>
class protocol_transaction_subscriber;

/// \brief Protocol transaction message publisher. Listeners of type \c protocol_transaction_subscriber will subscribe
/// to this class to listen for incoming messages.
template <typename T>
class protocol_transaction_publisher
{
public:
  explicit protocol_transaction_publisher(timer_manager& timer_db, const T& cancel_value_ = {}) :
    cancel_value(cancel_value_), running_timer(timer_db.create_unique_timer())
  {
  }
  protocol_transaction_publisher(const protocol_transaction_publisher&) = delete;
  protocol_transaction_publisher(protocol_transaction_publisher&&)      = delete;
  ~protocol_transaction_publisher()
  {
    if (has_subscriber()) {
      unsubscribe();
    }
  }

  /// \brief Checks if there is any pending transaction.
  bool has_subscriber() const { return sub != nullptr; }

  /// \brief Sets the result of the managed transaction. If no transaction is active, report a warning.
  template <typename U>
  void set(U&& u)
  {
    if (try_set(std::forward<U>(u))) {
      return;
    }
    if (not has_subscriber()) {
      srslog::fetch_basic_logger("ALL").debug("Setting transaction result, but no subscriber is listening");
    } else {
      srslog::fetch_basic_logger("ALL").warning("Transaction result is being overwritten");
    }
  }

  /// \brief Sets the result of the managed transaction if active. If no transaction is currently pending, returns
  /// false.
  template <typename U>
  bool try_set(U&& u)
  {
    if (not has_subscriber() or sub->event.is_set()) {
      return false;
    }
    running_timer.stop();
    auto old_sub = sub;
    unsubscribe();
    old_sub->event.set(std::forward<U>(u));
    return true;
  }

private:
  friend class protocol_transaction_subscriber<T>;

  void subscribe_(protocol_transaction_subscriber<T>& sub_)
  {
    srsgnb_assert(not has_subscriber(), "This class only allows one subscriber/listener per transaction");
    sub         = &sub_;
    sub->parent = this;
  }

  void subscribe_(protocol_transaction_subscriber<T>& sub_, unsigned time_to_cancel)
  {
    subscribe_(sub_);
    // Setup timeout.
    running_timer.set(time_to_cancel, [this](timer_id_t /**/) { set(cancel_value); });
    running_timer.run();
  }

  void unsubscribe()
  {
    srsgnb_assert(has_subscriber(), "Unsubscribe called but no subscriber is registered");
    sub->parent = nullptr;
    sub         = nullptr;
  }

  const T cancel_value;

  protocol_transaction_subscriber<T>* sub = nullptr;
  unique_timer                        running_timer;
};

/// \brief Protocol Transaction Awaitable. This awaitable is single-use, after which, the respective
/// transaction_manager class will reset its underlying event state.
template <typename T>
class protocol_transaction_subscriber
{
public:
  using result_type  = T;
  using awaiter_type = typename manual_event<result_type>::awaiter_type;

  protocol_transaction_subscriber() = default;
  protocol_transaction_subscriber(protocol_transaction_publisher<T>& publisher) { subscribe_to(publisher); }
  protocol_transaction_subscriber(protocol_transaction_subscriber&& other)                 = delete;
  protocol_transaction_subscriber(const protocol_transaction_subscriber& other)            = delete;
  protocol_transaction_subscriber& operator=(protocol_transaction_subscriber&& other)      = delete;
  protocol_transaction_subscriber& operator=(const protocol_transaction_subscriber& other) = delete;
  ~protocol_transaction_subscriber()
  {
    if (parent != nullptr) {
      parent->unsubscribe();
    }
  }

  /// \brief Subscribes a \c protocol_transaction_subscriber observer/listener/subscriber to this publisher. Only
  /// one simultaneous subscriber is allowed.
  void subscribe_to(protocol_transaction_publisher<T>& publisher) { publisher.subscribe_(*this); }

  /// \brief Subscribes a \c protocol_transaction_subscriber observer/listener/subscriber to this publisher and sets
  /// a timeout for automatic unsubscription. Only one simultaneous subscriber is allowed.
  void subscribe_to(protocol_transaction_publisher<T>& publisher, unsigned time_to_cancel)
  {
    publisher.subscribe_(*this, time_to_cancel);
  }

  /// \brief Checks whether the transaction was subscribed.
  bool has_publisher() const { return parent != nullptr; }

  /// \brief Checks if transaction result has been set.
  bool complete() const { return event.is_set(); }

  /// \brief Result of the transaction.
  const T& result() const&
  {
    srsgnb_assert(complete(), "Trying to fetch result of incomplete transaction");
    return event.get();
  }
  T result() &&
  {
    srsgnb_assert(complete(), "Trying to fetch result of incomplete transaction");
    return std::move(event).get();
  }

  /// Awaiter interface.
  awaiter_type get_awaiter() { return event.get_awaiter(); }

private:
  friend class protocol_transaction_publisher<T>;

  protocol_transaction_publisher<T>* parent = nullptr;
  manual_event<T>                    event;
};

} // namespace srsgnb