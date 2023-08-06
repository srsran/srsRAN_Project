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

#include "async_event_source.h"
#include "manual_event.h"
#include "srsran/adt/expected.h"
#include "srsran/adt/variant.h"
#include "srsran/support/compiler.h"
#include "srsran/support/timers.h"
#include <array>

namespace srsran {

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

/// \brief Manager of multiple concurrent protocol transactions. Each managed transaction can be uniquely identified by
/// a "transaction_id" integer value, which is automatically assigned by the \c protocol_transaction_manager.
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
  explicit protocol_transaction_manager(timer_factory timer_service_, const T& cancel_value_ = {}) :
    timer_service(timer_service_), cancel_value(cancel_value_)
  {
  }

  /// \brief Creates a new protocol transaction with automatically assigned transaction ID.
  SRSRAN_NODISCARD protocol_transaction<T> create_transaction()
  {
    unsigned transaction_id = next_transaction_id.fetch_add(1, std::memory_order_relaxed) % N;
    if (not transactions[transaction_id].is_set()) {
      // cancel any existing awaiter.
      bool ignore = set(transaction_id, cancel_value);
      (void)ignore;
    }
    transactions[transaction_id].reset();
    return {transaction_id, transactions[transaction_id]};
  }

  /// \brief Creates a new protocol transaction with automatically assigned transaction ID and with a timeout, after
  /// which the transaction gets cancelled.
  SRSRAN_NODISCARD protocol_transaction<T> create_transaction(std::chrono::milliseconds time_to_cancel)
  {
    protocol_transaction<T> t = create_transaction();
    // Setup timeout.
    if (not running_timers[t.id()].is_valid()) {
      // Create a new timer if it doesn't exist yet.
      running_timers[t.id()] = timer_service.create_timer();
    }
    running_timers[t.id()].set(time_to_cancel, [this, transaction_id = t.id()](timer_id_t tid) {
      if (not set(transaction_id, cancel_value)) {
        srslog::fetch_basic_logger("ALL").warning("Transaction id={} timeout but transaction is already completed",
                                                  transaction_id);
      }
    });
    running_timers[t.id()].run();
    return t;
  }

  /// \brief Creates a new protocol transaction for known transactionID and with a timeout, after
  /// which the transaction gets cancelled.
  SRSRAN_NODISCARD protocol_transaction<T> create_transaction(unsigned                  transaction_id,
                                                              std::chrono::milliseconds time_to_cancel)
  {
    if (not transactions[transaction_id].is_set()) {
      // cancel any existing awaiter.
      bool ignore = set(transaction_id, cancel_value);
      (void)ignore;
    }
    transactions[transaction_id].reset();

    protocol_transaction<T> t = {transaction_id, transactions[transaction_id]};
    // Setup timeout.
    if (not running_timers[t.id()].is_valid()) {
      // Create a new timer if it doesn't exist yet.
      running_timers[t.id()] = timer_service.create_timer();
    }
    running_timers[t.id()].set(time_to_cancel, [this, transaction_id = t.id()](timer_id_t tid) {
      if (not set(transaction_id, cancel_value)) {
        srslog::fetch_basic_logger("ALL").warning("Transaction id={} timeout but transaction is already completed",
                                                  transaction_id);
      }
    });
    running_timers[t.id()].run();
    return t;
  }

  /// \brief Sets the result of a managed transaction with the provided transaction_id.
  /// \param[in] transaction_id Id of the transaction.
  /// \param[in] result Result of the transaction.
  /// \return True if result of the transaction was successfully set. False, if the transaction has already finished.
  template <typename U>
  SRSRAN_NODISCARD bool set(unsigned transaction_id, U&& result)
  {
    if (transactions[transaction_id].is_set()) {
      return false;
    }
    running_timers[transaction_id].stop();
    transactions[transaction_id].set(std::forward<U>(result));
    return true;
  }

private:
  timer_factory timer_service;
  const T       cancel_value;

  std::atomic<unsigned>          next_transaction_id{0};
  std::array<unique_timer, N>    running_timers;
  std::array<manual_event<T>, N> transactions;
};

struct no_fail_response_path {};

/// \brief Type used to represent a transaction timeout.
struct transaction_timeout {};

/// \brief Publisher of application protocol transaction outcomes to which an observer can subscribe to.
template <typename SuccessResp, typename FailureResp = no_fail_response_path>
using protocol_transaction_event_source = async_event_source<variant<SuccessResp, FailureResp, transaction_timeout>>;

/// \brief Observer of application protocol transaction outcome.
template <typename SuccessResp, typename FailureResp = no_fail_response_path>
class protocol_transaction_outcome_observer
{
  static_assert(not std::is_same<SuccessResp, transaction_timeout>::value, "Invalid Success Response");
  static_assert(not std::is_same<FailureResp, transaction_timeout>::value, "Invalid Success Response");

  using observer_type = async_single_event_observer<variant<SuccessResp, FailureResp, transaction_timeout>>;

public:
  using success_response_type = SuccessResp;
  using failure_response_type = FailureResp;
  using event_source_type     = protocol_transaction_event_source<success_response_type, failure_response_type>;
  using awaiter_type          = typename observer_type::awaiter_type;

  /// \brief Subscribes this observer to transaction event source of type \c protocol_transaction_event_source.
  /// Only one simultaneous subscriber is allowed.
  void subscribe_to(event_source_type& publisher) { observer.subscribe_to(publisher); }

  /// \brief Subscribes this observer to transaction event source of type \c protocol_transaction_event_source and
  /// sets a timeout to get a response. Only one simultaneous subscriber is allowed.
  void subscribe_to(event_source_type& publisher, std::chrono::milliseconds time_to_cancel)
  {
    observer.subscribe_to(publisher, time_to_cancel, transaction_timeout{});
  }

  /// \brief Checks whether this sink has been registered to an event source.
  bool connected() const { return observer.connected(); }

  /// \brief Checks if result has been set by the event source.
  bool complete() const { return observer.complete(); }

  /// \brief Checks whether the result of transaction was successful.
  bool successful() const { return complete() and variant_holds_alternative<success_response_type>(observer.result()); }

  /// \brief Checks whether the result of the transaction was a failure message.
  bool failed() const { return complete() and variant_holds_alternative<failure_response_type>(observer.result()); }

  /// \brief Checks whether there was a transaction timeout.
  bool timeout_expired() const
  {
    return complete() and variant_holds_alternative<transaction_timeout>(observer.result());
  }

  /// \brief Result set by event source.
  const success_response_type& response() const
  {
    srsran_assert(successful(), "Trying to fetch incorrect transaction result");
    return variant_get<success_response_type>(observer.result());
  }
  const failure_response_type& failure() const
  {
    srsran_assert(failed(), "Trying to fetch incorrect transaction result");
    return variant_get<failure_response_type>(observer.result());
  }

  /// Awaiter interface.
  awaiter_type get_awaiter() { return observer.get_awaiter(); }

private:
  observer_type observer;
};

} // namespace srsran
