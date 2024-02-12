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

#include "async_event_source.h"
#include "manual_event.h"
#include "srsran/adt/expected.h"
#include "srsran/adt/variant.h"
#include "srsran/support/compiler.h"
#include "srsran/support/timers.h"
#include <array>
#include <unordered_map>

namespace srsran {

/// Type of a transaction identifier.
using protocol_transaction_id_t = unsigned;

/// Invalid transaction id.
constexpr protocol_transaction_id_t invalid_protocol_transaction_id =
    std::numeric_limits<protocol_transaction_id_t>::max();

template <typename ResponseType>
class protocol_transaction_manager;

/// \brief Error causes for the case of transactions that timed out or were cancelled.
enum class protocol_transaction_failure { timeout, cancel, abnormal };

/// \brief Protocol Transaction Awaitable. This awaitable is single-use, after which, the respective
/// transaction_manager class will reset its underlying event state.
template <typename ResponseType>
class protocol_transaction
{
  using manager_type = protocol_transaction_manager<ResponseType>;

public:
  using response_type = ResponseType;
  using outcome_type  = expected<ResponseType, protocol_transaction_failure>;
  using event_type    = manual_event<outcome_type>;
  using awaiter_type  = typename event_type ::awaiter_type;

  protocol_transaction() = default;
  protocol_transaction(manager_type& parent_, unsigned transaction_id_, event_type& ev_) :
    parent(&parent_), transaction_id(transaction_id_), ev(&ev_)
  {
  }
  protocol_transaction(const protocol_transaction&) = delete;
  protocol_transaction(protocol_transaction&& other) noexcept :
    parent(std::exchange(other.parent, nullptr)),
    transaction_id(other.transaction_id),
    ev(std::exchange(other.ev, nullptr))
  {
  }
  ~protocol_transaction() { release(); }
  protocol_transaction& operator=(const protocol_transaction&) = delete;
  protocol_transaction& operator=(protocol_transaction&& other) noexcept
  {
    release();
    parent         = std::exchange(other.parent, nullptr);
    transaction_id = other.transaction_id;
    ev             = std::exchange(other.ev, nullptr);
    return *this;
  }

  /// Gets transaction id.
  unsigned id() const { return transaction_id; }

  /// Checks whether this transaction represents a valid, allocation transaction.
  bool valid() const { return parent != nullptr; }

  /// \brief Checks if transaction result has been set.
  bool complete() const
  {
    srsran_assert(valid(), "Trying to check completion of invalid transaction");
    return ev->is_set();
  }

  /// \brief Check whether transaction finished successfully with a response.
  bool has_response() const
  {
    srsran_assert(valid(), "Trying to check completion of invalid transaction");
    return complete() and ev->get().has_value();
  }

  /// \brief Check whether transaction was aborted (e.g. timeout or cancellation).
  bool aborted() const
  {
    srsran_assert(valid(), "Trying to check completion of invalid transaction");
    return complete() and ev->get().is_error();
  }

  /// \brief Get cause of transaction failure.
  const protocol_transaction_failure& failure_cause() const
  {
    srsran_assert(aborted(), "Trying to get result of invalid transaction");
    return ev->get().error();
  }

  /// \brief Result of the transaction.
  const response_type& response() const&
  {
    srsran_assert(has_response(), "Trying to get response of transaction without a response");
    return ev->get().value();
  }
  response_type response() &&
  {
    srsran_assert(has_response(), "Trying to get result of transaction without a response");
    return std::move(ev)->get();
  }

  const outcome_type& outcome() const&
  {
    srsran_assert(complete(), "Trying to fetch outcome of incomplete transaction");
    return ev->get();
  }
  outcome_type outcome() &&
  {
    srsran_assert(complete(), "Trying to fetch outcome of incomplete transaction");
    return std::move(ev)->get();
  }

  /// Awaiter interface.
  awaiter_type get_awaiter()
  {
    srsran_assert(valid(), "Trying to fetch awaiter of invalid transaction");
    return ev->get_awaiter();
  }

  /// Release transaction id, so it can be reused.
  void release()
  {
    if (parent != nullptr) {
      parent->remove_transaction(transaction_id);
      parent = nullptr;
      ev     = nullptr;
    }
  }

private:
  manager_type*             parent         = nullptr;
  protocol_transaction_id_t transaction_id = invalid_protocol_transaction_id;
  event_type*               ev             = nullptr;
};

/// \brief Manager of multiple concurrent protocol transactions. Each managed transaction can be uniquely identified by
/// a "transaction_id" integer value, which is automatically assigned by the \c protocol_transaction_manager.
/// Transactions are single-use, meaning that the user will have to create new transaction objects, with different
/// transaction_ids for every message exchange.
/// To create a new transaction, the user may call "create_transaction(...)", where a timeout for the transaction
/// can be optionally defined.
/// \tparam T type of the response to the transaction.
template <typename T>
class protocol_transaction_manager
{
  using outcome_type = typename protocol_transaction<T>::outcome_type;

public:
  /// Time limit for a transaction to complete.
  const static std::chrono::milliseconds max_timeout;

  explicit protocol_transaction_manager(protocol_transaction_id_t nof_transaction_ids_, timer_factory timer_service_) :
    nof_transaction_ids(nof_transaction_ids_), timer_service(timer_service_)
  {
  }
  ~protocol_transaction_manager()
  {
    if (not running_transactions.empty()) {
      srslog::fetch_basic_logger("ALL").error("Protocol transaction manager destroyed with {} running transactions",
                                              running_transactions.size());
    }
  }

  /// \brief Creates a new protocol transaction with automatically assigned transaction ID and with a timeout, after
  /// which the transaction gets cancelled.
  SRSRAN_NODISCARD protocol_transaction<T> create_transaction(std::chrono::milliseconds time_to_cancel = max_timeout)
  {
    // Allocate a new transaction id for the protocol transaction.
    unsigned transaction_id = invalid_protocol_transaction_id;
    for (unsigned count = 0; count < nof_transaction_ids and transaction_id == invalid_protocol_transaction_id;
         ++count) {
      if (running_transactions.count(next_transaction_id) == 0) {
        transaction_id = next_transaction_id;
      }
      next_transaction_id = (next_transaction_id + 1) % nof_transaction_ids;
    }
    if (transaction_id == invalid_protocol_transaction_id) {
      srslog::fetch_basic_logger("ALL").error("Failed to allocate new transaction id");
      return {};
    }

    return create_transaction(transaction_id, time_to_cancel);
  }

  /// \brief Creates a new protocol transaction with pre-defined transaction ID.
  SRSRAN_NODISCARD protocol_transaction<T> create_transaction(protocol_transaction_id_t transaction_id,
                                                              std::chrono::milliseconds time_to_cancel = max_timeout)
  {
    // Create new transaction instance.
    auto ret = running_transactions.emplace(
        std::piecewise_construct, std::forward_as_tuple(transaction_id), std::forward_as_tuple());
    if (not ret.second) {
      srslog::fetch_basic_logger("ALL").error("Failed to allocate transaction id={}", transaction_id);
      return {};
    }

    // Create timer, set timeout and callback, and start running it.
    unique_timer& timer = ret.first->second.timer;
    timer               = timer_service.create_timer();
    timer.set(time_to_cancel, [this, transaction_id](timer_id_t /*unused*/) {
      if (not set_transaction_outcome(transaction_id, protocol_transaction_failure::timeout)) {
        srslog::fetch_basic_logger("ALL").warning("Transaction id={} timeout but transaction is already completed",
                                                  transaction_id);
      }
    });
    timer.run();

    return {*this, ret.first->first, ret.first->second.event};
  }

  /// \brief Sets the result of a managed transaction with the provided transaction_id.
  /// \param[in] transaction_id Id of the transaction.
  /// \param[in] result Result of the transaction.
  /// \return True if result of the transaction was successfully set. False, if the transaction has already finished.
  template <typename U>
  SRSRAN_NODISCARD bool set_response(unsigned transaction_id, U&& result)
  {
    static_assert(std::is_convertible<U, T>::value, "Invalid transaction response type being set");
    return set_transaction_outcome(transaction_id, std::forward<U>(result));
  }

  /// \brief Cancel a running transaction.
  bool cancel_transaction(unsigned transaction_id)
  {
    return set_transaction_outcome(transaction_id, protocol_transaction_failure::cancel);
  }

private:
  friend class protocol_transaction<T>;

  template <typename U>
  SRSRAN_NODISCARD bool set_transaction_outcome(unsigned transaction_id, U&& result)
  {
    auto it = running_transactions.find(transaction_id);
    if (it == running_transactions.end()) {
      // No transaction with provided transaction id exists.
      return false;
    }

    // Stop running timer, so it doesn't try to overwrite result.
    it->second.timer.stop();

    // Store result.
    it->second.event.set(std::forward<U>(result));
    return true;
  }

  void remove_transaction(protocol_transaction_id_t transaction_id)
  {
    auto it = running_transactions.find(transaction_id);
    if (it == running_transactions.end()) {
      srslog::fetch_basic_logger("ALL").warning("Trying to remove non-existing transaction id={}", transaction_id);
      return;
    }
    running_transactions.erase(it);
  }

  const protocol_transaction_id_t nof_transaction_ids;
  timer_factory                   timer_service;

  struct transaction_context {
    unique_timer               timer;
    manual_event<outcome_type> event;
  };

  protocol_transaction_id_t next_transaction_id{0};

  std::unordered_map<protocol_transaction_id_t, transaction_context> running_transactions;
};

template <typename T>
constexpr std::chrono::milliseconds protocol_transaction_manager<T>::max_timeout{600000};

struct no_fail_response_path {};

template <typename SuccessResp, typename FailureResp>
class protocol_transaction_outcome_observer;

/// \brief Publisher of application protocol transaction outcomes to which an observer can subscribe to.
template <typename SuccessResp, typename FailureResp = no_fail_response_path>
class protocol_transaction_event_source
{
public:
  explicit protocol_transaction_event_source(timer_factory timer_db) : ev_source(timer_db) {}
  ~protocol_transaction_event_source()
  {
    // Automatically cancels any pending transaction.
    this->set(protocol_transaction_failure::cancel);
  }

  /// \brief Forwards a result to the registered listener/subscriber/observer.
  /// \return If no subscriber is registered, returns false.
  template <typename U>
  bool set(U&& u)
  {
    return ev_source.set(std::forward<U>(u));
  }

private:
  friend class protocol_transaction_outcome_observer<SuccessResp, FailureResp>;

  async_event_source<variant<SuccessResp, FailureResp, protocol_transaction_failure>> ev_source;
};

/// \brief Observer of application protocol transaction outcome.
template <typename SuccessResp, typename FailureResp = no_fail_response_path>
class protocol_transaction_outcome_observer
{
  static_assert(not std::is_same<SuccessResp, protocol_transaction_failure>::value, "Invalid Success Response");
  static_assert(not std::is_same<FailureResp, protocol_transaction_failure>::value, "Invalid Success Response");

  using observer_type = async_single_event_observer<variant<SuccessResp, FailureResp, protocol_transaction_failure>>;

public:
  using success_response_type = SuccessResp;
  using failure_response_type = FailureResp;
  using event_source_type     = protocol_transaction_event_source<success_response_type, failure_response_type>;
  using awaiter_type          = typename observer_type::awaiter_type;

  /// \brief Subscribes this observer to transaction event source of type \c protocol_transaction_event_source.
  /// Only one simultaneous subscriber is allowed.
  void subscribe_to(event_source_type& publisher) { observer.subscribe_to(publisher.ev_source); }

  /// \brief Subscribes this observer to transaction event source of type \c protocol_transaction_event_source and
  /// sets a timeout to get a response. Only one simultaneous subscriber is allowed.
  void subscribe_to(event_source_type& publisher, std::chrono::milliseconds time_to_cancel)
  {
    observer.subscribe_to(publisher.ev_source, time_to_cancel, protocol_transaction_failure::timeout);
  }

  /// \brief Checks whether this sink has been registered to an event source.
  bool connected() const { return observer.connected(); }

  /// \brief Checks if result has been set by the event source.
  bool complete() const { return observer.complete(); }

  /// \brief Checks whether the result of transaction was successful.
  bool successful() const { return complete() and variant_holds_alternative<success_response_type>(observer.result()); }

  /// \brief Checks whether the result of the transaction was a failure message.
  bool failed() const { return complete() and variant_holds_alternative<failure_response_type>(observer.result()); }

  /// \brief Checks if the protocol transaction could not be completed, due to abnormal conditions, cancellations or
  /// timeout.
  bool protocol_transaction_failed() const
  {
    return complete() and variant_holds_alternative<protocol_transaction_failure>(observer.result());
  }

  /// \brief Checks whether there was a transaction timeout.
  bool timeout_expired() const
  {
    return protocol_transaction_failed() and
           variant_get<protocol_transaction_failure>(observer.result()) == protocol_transaction_failure::timeout;
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
