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
class protocol_transaction_receiver;

/// \brief Protocol transaction manager for transactions without transaction ID.
template <typename T>
class protocol_transaction_channel
{
public:
  explicit protocol_transaction_channel(timer_manager& timer_db, const T& cancel_value_ = {}) :
    cancel_value(cancel_value_), running_timer(timer_db.create_unique_timer())
  {
  }

  /// \brief Creates a new protocol transaction and returns the receiver for the transaction result.
  /// \return Awaitable/Receiver of the transaction result.
  protocol_transaction_receiver<T> create_transaction() __attribute__((warn_unused_result));

  /// \brief Creates a new protocol transaction with a timeout value. After the timeout, the transaction is set
  /// as cancelled.
  /// \return Awaitable/Receiver of the transaction result.
  protocol_transaction_receiver<T> create_transaction(unsigned time_to_cancel) __attribute__((warn_unused_result))
  {
    // Setup timeout.
    running_timer.set(time_to_cancel, [this](timer_id_t /**/) { set(cancel_value); });
    running_timer.run();
    return create_transaction();
  }

  /// \brief Sets the result of the managed transaction.
  template <typename U>
  void set(U&& u)
  {
    if (not channel_active) {
      srslog::fetch_basic_logger("ALL").warning("Setting transaction result, but no transaction was created");
    }
    if (event.is_set()) {
      srslog::fetch_basic_logger("ALL").warning("Transaction result is being overwritten");
    }
    running_timer.stop();
    event.set(std::forward<U>(u));
  }

private:
  friend class protocol_transaction_receiver<T>;

  void destroy_receiver()
  {
    channel_active = false;
    event.reset();
  }

  const T cancel_value;

  bool            channel_active = false;
  unique_timer    running_timer;
  manual_event<T> event;
};

/// \brief Protocol Transaction Awaitable. This awaitable is single-use, after which, the respective
/// transaction_manager class will reset its underlying event state.
template <typename T>
class protocol_transaction_receiver
{
public:
  using result_type  = T;
  using awaiter_type = typename manual_event<result_type>::awaiter_type;

  protocol_transaction_receiver() = default;
  protocol_transaction_receiver(protocol_transaction_channel<T>* parent_) : parent(parent_) {}
  protocol_transaction_receiver(protocol_transaction_receiver&& other) noexcept :
    parent(std::exchange(other.parent, nullptr))
  {
  }
  protocol_transaction_receiver& operator=(protocol_transaction_receiver&& other)
  {
    parent = std::exchange(other.parent, nullptr);
    return *this;
  }
  ~protocol_transaction_receiver()
  {
    if (parent != nullptr) {
      parent->destroy_receiver();
    }
  }

  /// \brief Checks whether the transaction is active or detached from any channel.
  bool active() const { return parent != nullptr; }

  /// \brief Checks if transaction result has been set.
  bool complete() const { return parent->event.is_set(); }

  /// \brief Result of the transaction.
  const T& result() const& { return parent->event.get(); }
  T        result() && { return std::move(parent->event).get(); }

  /// Awaiter interface.
  awaiter_type get_awaiter() { return parent->event.get_awaiter(); }

private:
  protocol_transaction_channel<T>* parent = nullptr;
};

template <typename T>
protocol_transaction_receiver<T> protocol_transaction_channel<T>::create_transaction()
{
  srsgnb_assert(not channel_active, "Cannot create concurrent transactions with this type of transaction manager");
  channel_active = true;
  return protocol_transaction_receiver<T>{this};
}

} // namespace srsgnb