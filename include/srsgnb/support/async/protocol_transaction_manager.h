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

#include "manual_event.h"
#include "srsgnb/adt/expected.h"
#include "srsgnb/support/timers.h"
#include <array>

namespace srsgnb {

/// \brief Protocol Transaction Awaitable. This awaitable is single-use, after which, the respective
/// transaction_manager class will reset its underlying event state.
template <typename T>
struct protocol_transaction {
public:
  using result_type  = T;
  using awaiter_type = typename manual_event<result_type>::awaiter_type;

  protocol_transaction() = default;
  protocol_transaction(unsigned transaction_id_, manual_event<result_type>& ev_) :
    transaction_id(transaction_id_), ev(&ev_)
  {
  }

  /// Gets transaction id.
  unsigned id() const { return transaction_id; }

  /// \brief Checks if transaction result has been set.
  bool complete() const { return ev->is_set(); }

  /// \brief Result of the transaction.
  const T& result() const& { return ev->get(); }
  T        result() && { return std::move(ev)->get(); }

  /// Awaiter interface.
  awaiter_type get_awaiter() { return ev->get_awaiter(); }

private:
  unsigned                   transaction_id = std::numeric_limits<unsigned>::max();
  manual_event<result_type>* ev             = nullptr;
};

/// \brief Manager of multiple protocol transactions. Each managed transaction can be uniquely identified by a
/// "transaction_id", which is automatically assigned by the \c protocol_transaction_manager.
/// Transactions are single-use, meaning that the user will have to create new transaction objects, with different
/// transaction_ids for every message exchange.
/// To create a new transaction, the user may call "create_transaction(...)", where a timeout for the transaction
/// can be optionally defined.
/// \tparam T type of each transaction message.
/// \tparam N Number of simultaneous transactions that the manager can handle.
template <typename T, size_t N>
class protocol_transaction_manager
{
public:
  explicit protocol_transaction_manager(timer_manager& timer_db_, const T& cancel_value_ = {}) :
    timer_db(timer_db_), cancel_value(cancel_value_)
  {
  }

  /// \brief Creates a new protocol transaction with automatically assigned transaction ID.
  protocol_transaction<T> create_transaction() __attribute__((warn_unused_result))
  {
    unsigned transaction_id = next_transaction_id.fetch_add(1, std::memory_order_relaxed);
    if (not transactions[transaction_id].is_set()) {
      // cancel any existing awaiter.
      set(transaction_id, cancel_value);
    }
    transactions[transaction_id].reset();
    return {transaction_id, transactions[transaction_id]};
  }

  /// \brief Creates a new protocol transaction with automatically assigned transaction ID and with a timeout, after
  /// which the transaction gets cancelled.
  protocol_transaction<T> create_transaction(unsigned time_to_cancel) __attribute__((warn_unused_result))
  {
    protocol_transaction<T> t = create_transaction();
    // Setup timeout.
    if (not running_timers[t.id()].is_valid()) {
      // Create a new timer if it doesn't exist yet.
      running_timers[t.id()] = timer_db.create_unique_timer();
    }
    running_timers[t.id()].set(time_to_cancel,
                               [this, transaction_id = t.id()](timer_id_t tid) { set(transaction_id, cancel_value); });
    running_timers[t.id()].run();
    return t;
  }

  template <typename U>
  void set(unsigned transaction_id, U&& u)
  {
    running_timers[transaction_id].stop();
    srsgnb_assert(not transactions[transaction_id].is_set(), "Transaction result cannot be overwritten.");
    transactions[transaction_id].set(std::forward<U>(u));
  }

private:
  timer_manager& timer_db;
  const T        cancel_value;

  std::atomic<unsigned>          next_transaction_id{0};
  std::array<unique_timer, N>    running_timers;
  std::array<manual_event<T>, N> transactions;
};

/// \brief Protocol transaction manager for transactions without transaction ID.
template <typename T>
class single_protocol_transaction_manager
{
public:
  explicit single_protocol_transaction_manager(timer_manager& timer_db_, const T& cancel_value_ = {}) :
    timer_db(timer_db_), cancel_value(cancel_value_)
  {
  }

  protocol_transaction<T> create_transaction() __attribute__((warn_unused_result))
  {
    if (not event.is_set()) {
      // cancel any existing awaiter.
      running_timer.stop();
      event.set(cancel_value);
    }
    event.reset();
    return {0, event};
  }

  protocol_transaction<T> create_transaction(unsigned time_to_cancel) __attribute__((warn_unused_result))
  {
    protocol_transaction<T> t = create_transaction();
    // Setup timeout.
    if (not running_timer.is_valid()) {
      // Create a new timer if it doesn't exist yet.
      running_timer = timer_db.create_unique_timer();
    }
    running_timer.set(time_to_cancel, [this](timer_id_t /**/) { event.set(cancel_value); });
    event.run();
    return t;
  }

  template <typename U>
  void set(U&& u)
  {
    running_timer.stop();
    event.set(std::forward<U>(u));
  }

private:
  timer_manager& timer_db;
  const T        cancel_value;

  unique_timer    running_timer;
  manual_event<T> event;
};

} // namespace srsgnb
