/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/detail/intrusive_ptr.h"
#include "srsran/support/synchronization/futex_util.h"
#include <utility>

namespace srsran {

namespace scoped_sync_detail {

/// \brief Shared sync state.
///
/// We rely on intrusive_ptr to manage the lifetime of this object. This is necessary because it hard to make futex
/// wake/wait operations happen before the state object is destroyed.
class state : public intrusive_ptr_atomic_ref_counter
{
public:
  friend void intrusive_ptr_inc_ref(state* ptr) { ptr->ref_count.inc_ref(); }
  friend void intrusive_ptr_dec_ref(state* ptr)
  {
    if (ptr->ref_count.dec_ref()) {
      delete ptr;
    }
  }

  /// Count of number of current scoped_sync_token.
  intrusive_ptr_atomic_ref_counter ref_count;
  /// Number of non-reset scoped_sync_token instances.
  std::atomic<uint32_t> token_count{0};
};

} // namespace scoped_sync_detail

/// \brief Scoped token that notifies the associated sync_event when it gets destroyed or reset.
class scoped_sync_token
{
public:
  scoped_sync_token() = default;
  scoped_sync_token(const intrusive_ptr<scoped_sync_detail::state>& state_) : state_ptr(state_)
  {
    if (state_ptr != nullptr) {
      state_ptr->token_count.fetch_add(1, std::memory_order_relaxed);
    }
  }
  scoped_sync_token(const scoped_sync_token& other) : state_ptr(other.state_ptr)
  {
    if (state_ptr != nullptr) {
      state_ptr->token_count.fetch_add(1, std::memory_order_relaxed);
    }
  }
  scoped_sync_token(scoped_sync_token&& other) noexcept : state_ptr(std::move(other.state_ptr)) {}
  ~scoped_sync_token() { reset(); }
  scoped_sync_token& operator=(const scoped_sync_token& other)
  {
    if (this != &other) {
      reset();
      state_ptr = other.state_ptr;
      if (state_ptr != nullptr) {
        state_ptr->token_count.fetch_add(1, std::memory_order_relaxed);
      }
    }
    return *this;
  }
  scoped_sync_token& operator=(scoped_sync_token&& other) noexcept
  {
    reset();
    state_ptr = std::move(other.state_ptr);
    return *this;
  }

  /// Destroys the token and potentially unlocks sync_event::wait().
  void reset()
  {
    if (state_ptr != nullptr) {
      auto cur = state_ptr->token_count.fetch_sub(1, std::memory_order_acq_rel) - 1;
      if (cur == 0) {
        // Count is zero. Wake all waiters.
        // Note: The intrusive_ptr ref_count protects the use of state_ptr->token_count here, even if the waiter
        // already called destroy on state_ptr in its dtor.
        futex_util::wake_all(state_ptr->token_count);
      }
      // Potentially destroys the state object.
      state_ptr.reset();
    }
  }

private:
  intrusive_ptr<scoped_sync_detail::state> state_ptr;
};

/// \brief Synchronization event to wait until all tokens to be reset.
///
/// This class operates like a std::barrier, where the "arrive" call is performed by "tokens" in an RAII manner and
/// "wait" is performed by the \c sync_event. Contrarily to the std::barrier, the number of participants is not fixed.
class sync_event
{
public:
  ~sync_event() { wait(); }

  /// Creates a new observer of stop() requests.
  scoped_sync_token get_token() { return scoped_sync_token{state_ptr}; }

  /// Waits for all tokens to be reset. At the end of this call,
  void wait()
  {
    // Block waiting until all observers are gone.
    auto cur = state_ptr->token_count.load(std::memory_order_acquire);
    while (cur > 0) {
      futex_util::wait(state_ptr->token_count, cur);
      cur = state_ptr->token_count.load(std::memory_order_acquire);
    }
  }

  [[nodiscard]] uint32_t nof_tokens_approx() const { return state_ptr->token_count.load(std::memory_order_relaxed); }

private:
  /// \brief Ref-counted pointer to the shared state.
  intrusive_ptr<scoped_sync_detail::state> state_ptr{new scoped_sync_detail::state()};
};

} // namespace srsran
