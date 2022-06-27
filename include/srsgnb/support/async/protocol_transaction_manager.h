/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_TRANSACTION_RING_H
#define SRSGNB_TRANSACTION_RING_H

#include "manual_event.h"
#include "srsgnb/adt/expected.h"
#include "srsgnb/support/timers.h"
#include <array>

namespace srsgnb {

template <typename T>
struct protocol_transaction {
public:
  using result_type  = T;
  using awaiter_type = typename manual_event<result_type>::awaiter_type;

  protocol_transaction() : transaction_id(std::numeric_limits<unsigned>::max()), ev(nullptr) {}
  protocol_transaction(unsigned transaction_id_, manual_event<result_type>& ev_) :
    transaction_id(transaction_id_), ev(&ev_)
  {
  }

  /// Gets transaction id.
  unsigned id() const { return transaction_id; }

  bool complete() const { return ev->is_set(); }

  const T& result() const& { return ev->get(); }
  T        result() && { return std::move(ev)->get(); }

  /// awaiter interface
  awaiter_type get_awaiter()
  {
    srsran_assert(not ev->is_set(), "Transaction cannot be reused");
    return ev->get_awaiter();
  }

private:
  template <typename U, size_t N>
  friend class protocol_transaction_manager;

  template <typename U>
  void set(U&& u)
  {
    srsran_assert(not ev->is_set(), "Transaction result cannot be overwritten.");
    ev->set(std::forward<U>(u));
  }

  unsigned                   transaction_id;
  manual_event<result_type>* ev;
};

template <typename T, size_t N>
class protocol_transaction_manager
{
public:
  explicit protocol_transaction_manager(timer_manager& timer_db_, const T& cancel_value_ = {}) :
    timer_db(timer_db_), cancel_value(cancel_value_)
  {
  }

  protocol_transaction<T> create_transaction()
  {
    unsigned transaction_id = next_transaction_id.fetch_add(1, std::memory_order_relaxed);
    if (not slot_ar[transaction_id].is_set()) {
      // cancel any existing awaiter.
      running_timers[transaction_id].stop();
      slot_ar[transaction_id].set(cancel_value);
    }
    slot_ar[transaction_id].reset();
    return {transaction_id, slot_ar[transaction_id]};
  }

  protocol_transaction<T> create_transaction(unsigned time_to_cancel)
  {
    protocol_transaction<T> t = create_transaction();
    // Setup timeout.
    if (not running_timers[t.id()].is_valid()) {
      // Create a new timer if it doesn't exist yet.
      running_timers[t.id()] = timer_db.create_unique_timer();
    }
    running_timers[t.id()].set(
        time_to_cancel, [this, transaction_id = t.id()](timer_id_t tid) { slot_ar[transaction_id].set(cancel_value); });
    return t;
  }

  template <typename U>
  void set(unsigned transaction_id, U&& u)
  {
    running_timers[transaction_id].stop();
    slot_ar[transaction_id].set(std::forward<U>(u));
  }

private:
  timer_manager& timer_db;
  const T        cancel_value;

  std::atomic<unsigned>          next_transaction_id{0};
  std::array<unique_timer, N>    running_timers;
  std::array<manual_event<T>, N> slot_ar;
};

} // namespace srsgnb

#endif // SRSGNB_TRANSACTION_RING_H
