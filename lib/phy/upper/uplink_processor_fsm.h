/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/ran/slot_point.h"
#include "srsran/support/srsran_assert.h"
#include <atomic>
#include <thread>

namespace srsran {

/// \brief Notifier interface for uplink_processr_fsm.
///
/// Allows the uplink PDU slot repository to notify the finite-state machine about
class uplink_processor_fsm_notifier
{
public:
  /// Default destructor.
  virtual ~uplink_processor_fsm_notifier() = default;

  /// Notifies the FSM that a new PDU has been added to repository.
  /// \remark An assertion is triggered if the FSM is not currently accepting PDUs.
  virtual void increment_pending_pdu_count() = 0;

  /// Notifies the FSM that the repository is not accepting more PDUs.
  /// \remark An assertion is triggered if the FSM is not currently accepting PDUs.
  virtual void stop_accepting_pdu() = 0;
};

/// \brief Finite-state machine for the uplink processor.
///
/// The interaction between the uplink processor and the PDU slot repository evolves according to the following states.
/// - Idle: the repository does not contain any PDU and it is not processing.
/// - Accepting: the repository accepts the enqueueing of new PDUs.
/// - Waiting: the repository contains PDUs but it has not processed any yet.
/// - Processing: the repository contains PDUs and it is processing PDUs.
/// - Locked: the repository is executing a sequential procedure and it does not accept any transitions.
/// - Stopped: the repository does not accept transitioning to any other state.
class uplink_processor_fsm : public uplink_processor_fsm_notifier
{
public:
  /// \brief Starts accepting PDUs for the given slot.
  /// \return True if the transition from \e idle to <em>accepting PDUs</em> was successful, false otherwise.
  bool start_new_slot(slot_point slot)
  {
    uint32_t expected_pending_pdu_count = pending_pdu_count_idle;
    if (!pending_pdu_count.compare_exchange_weak(expected_pending_pdu_count, accepting_pdu_mask)) {
      return false;
    }
    configured_slot = slot;
    return true;
  }

  /// \brief Returns true if the repository is configured with the given slot and ready to process PDUs.
  ///
  /// The slot is considered invalid if:
  /// - the current state is accepting PDUs, idle, locked, or stopped; or
  /// - the given slot is not equal to the configured one.
  bool is_slot_valid(slot_point slot) const
  {
    uint32_t current_state = pending_pdu_count.load();
    // Verify that the repository is in a valid state to process PDUs.
    if (is_state_idle(current_state) || is_state_accepting_pdu(current_state) || is_state_locked(current_state) ||
        is_state_stopped(current_state)) {
      return false;
    }

    // Checks the slot validity.
    return (configured_slot == slot);
  }

  // See the uplink_processor_fsm_notifier interface for the documentation.
  void stop_accepting_pdu() override
  {
    // Remove accepting PDU mask and verify the previous state was accepting PDU.
    [[maybe_unused]] uint32_t prev = pending_pdu_count.fetch_xor(accepting_pdu_mask);
    srsran_assert(is_state_accepting_pdu(prev), "Unexpected prev={:08x} finishing PDUs.", prev);
  }

  // See the uplink_processor_fsm_notifier interface for the documentation.
  void increment_pending_pdu_count() override
  {
    [[maybe_unused]] uint32_t prev = pending_pdu_count.fetch_add(pending_pdu_inc_queue);
    srsran_assert(is_state_accepting_pdu(prev), "Cannot accept PDUs.");
  }

  /// \brief Notifies the beginning of a slot discarding process.
  ///
  /// The current state does not allow discarding the slot if:
  /// - the state is stopped;
  /// - the state is locked (handling receive symbols or discarding);
  /// - there are no pending PDUs to discard; or
  /// - there are pending asynchronous tasks.
  ///
  /// \return True if the current state allows discarding the slot, false otherwise.
  bool start_discard_slot()
  {
    // Get current state.
    uint32_t current_state = pending_pdu_count.load();

    // Skip function - returns true if the slot discard is not necessary.
    auto skip_function = [&current_state]() {
      return is_state_stopped(current_state) || is_state_locked(current_state) ||
             (get_nof_pending_pdu_in_queue(current_state) == 0) || (get_nof_pending_pdu_in_exec(current_state) > 0);
    };

    // Try to set the number of pending execute PDU equal to the number of enqueued tasks and set the discarding slot
    // mask.
    bool success = false;
    while (!skip_function() && !success) {
      success = pending_pdu_count.compare_exchange_weak(
          current_state,
          current_state | (get_nof_pending_pdu_in_queue(current_state) * pending_pdu_inc_exec) | locked_mask);
    }

    return success;
  }

  /// \brief Notifies the completion of a slot discarding process.
  ///
  /// Transitions the state to idle.
  ///
  /// \remark An assertion is triggered if there are still PDUs to notify or the state is not locked.
  void finish_discard_slot()
  {
    [[maybe_unused]] uint32_t prev_state = pending_pdu_count.exchange(pending_pdu_count_idle);
    srsran_assert((get_nof_pending_pdu_in_queue(prev_state) == 0) && (get_nof_pending_pdu_in_exec(prev_state) == 0) &&
                      is_state_locked(prev_state),
                  "Unexpected state after discarding slot 0x{:08x}",
                  prev_state);
  }

  /// \brief Notifies the start of handling a receive symbol.
  ///
  /// The current state does not allow discarding the slot if:
  /// - is stopped;
  /// - state is locked (a different procedure is running); or
  /// - there are no pending PDUs to discard.
  ///
  /// \return True if the current state allows discarding the slot, false otherwise.
  bool start_handle_rx_symbol()
  {
    // Get current state.
    uint32_t current_state = pending_pdu_count.load();

    // Skip function - returns true if the slot processing lock is not possible.
    auto skip_function = [&current_state]() {
      // It is not possible to lock if the state is stopped, locked or there are no pending PDU.
      return (is_state_stopped(current_state) || is_state_locked(current_state) ||
              (get_nof_pending_pdu_in_queue(current_state) == 0));
    };

    // Try to set the number of pending execute PDUs equal to the number of enqueued tasks and set the discarding slot
    // mask.
    bool success = false;
    while (!skip_function() && !success) {
      success = pending_pdu_count.compare_exchange_weak(current_state, current_state | locked_mask);
    }

    return success;
  }

  /// \brief Notifies the completion of handling a receive symbol.
  ///
  /// Removes the locking mask.
  ///
  /// \remark An assertion is triggered if the current state is not locked.
  void finish_handle_rx_symbol()
  {
    [[maybe_unused]] uint32_t prev_state = pending_pdu_count.fetch_xor(locked_mask);
    srsran_assert(is_state_locked(prev_state), "Unexpected state after discarding slot 0x{:08x}", prev_state);
  }

  /// \brief Notifies the event of creating a new asynchronous execution task. It increments the PDU being executed
  /// count.
  /// \return True if the internal state allows the creation of the task, false otherwise.
  /// \remark An assertion is triggered if the state is accepting PDUs or there are no pending PDUs to process.
  bool on_create_pdu_task()
  {
    // Get current state.
    uint32_t current_state = pending_pdu_count.load();

    // Stop function - returns true if the current state is stopped, and it triggers an assertion if the current state
    // is unexpected.
    auto stop_function = [&current_state]() {
      if (is_state_stopped(current_state)) {
        return true;
      }

      srsran_assert(!is_state_accepting_pdu(current_state) && (get_nof_pending_pdu_in_queue(current_state) > 0),
                    "The slot repository is in an unexpected state 0x{:08x}.",
                    current_state);
      return false;
    };

    // Try to increment the number of PDUs to execute.
    bool success = false;
    while (!stop_function() && !success) {
      success = pending_pdu_count.compare_exchange_weak(current_state, current_state + pending_pdu_inc_exec);
    }

    // Return true if the exchange was successful.
    return success;
  }

  /// \brief Notifies the completion of a PDU processing.
  ///
  /// Decrements the pending PDU counter.
  ///
  /// \remark An assertion is triggered if:
  /// - the current state is accepting PDUs; or
  /// - the current state is stopped; or
  /// - any of the pending counters are not zero.
  void on_finish_processing_pdu()
  {
    [[maybe_unused]] uint32_t prev = pending_pdu_count.fetch_sub(pending_pdu_inc_queue + pending_pdu_inc_exec);

    // Assert previous state.
    srsran_assert(!is_state_accepting_pdu(prev) && !is_state_stopped(prev) && (get_nof_pending_pdu_in_exec(prev) > 0) &&
                      (get_nof_pending_pdu_in_queue(prev) > 0),
                  "The slot repository is in an unexpected state 0x{:08x}.",
                  prev);
  }

  /// \brief Notifies a new PRACH detection request.
  ///
  /// It tries to increment the number of pending PRACH detection task counter by one if the internal state allows the
  /// creation of the PRACH detection task.
  ///
  /// The creation of the task is allowed if the internal state is not stopped. See is_state_stopped() for more details.
  ///
  /// \return True if the internal state allows the creation of the PRACH detection task. False, otherwise.
  bool on_prach_detection()
  {
    // Get current count.
    uint32_t current_pending_prach_count = pending_prach_count.load();

    // Try to increment the count.
    bool success = false;
    while (!is_state_stopped(current_pending_prach_count) && !success) {
      success = pending_prach_count.compare_exchange_weak(current_pending_prach_count, current_pending_prach_count + 1);
    }

    // Return true if the exchange was successful.
    return success;
  }

  /// \brief Notifies the completion of a PRACH detection task.
  /// \remark An assertion is triggered if the PRACH detection task counter was zero or stopped before the decrement.
  void on_end_prach_detection()
  {
    [[maybe_unused]] uint32_t prev = pending_prach_count.fetch_sub(1);

    // Assert previous count is valid.
    srsran_assert(
        !is_state_stopped(prev) && (prev != 0), "The PRACH detection task count 0x{:08x} is unexpected.", prev);
  }

  /// \brief Stops the uplink PDU slot repository.
  ///
  /// It waits as long as:
  /// - the repository is active accepting PDUs;
  /// - there are asynchronous tasks being executed; or
  /// - the state is locked.
  void stop()
  {
    // As long as there are pending asynchronous tasks, wait for them to finish.
    for (uint32_t current_state = pending_pdu_count.load();
         is_state_accepting_pdu(current_state) || (get_nof_pending_pdu_in_exec(current_state) > 0) ||
         is_state_locked(current_state) ||
         !pending_pdu_count.compare_exchange_weak(current_state, pending_pdu_count_stopped);
         current_state = pending_pdu_count.load()) {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }

    // As long as there are pending PRACH detection tasks, wait for them to finish.
    for (uint32_t current_state = pending_prach_count.load();
         (current_state != 0) || !pending_prach_count.compare_exchange_weak(current_state, pending_pdu_count_stopped);
         current_state = pending_prach_count.load()) {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }

private:
  /// Accepting PDU state mask in the pending PDU count.
  static constexpr uint32_t accepting_pdu_mask = 0x80000000;
  /// The current state is locked. The state is locked if a certain sequential procedure is executed (i.e., discarding
  /// slot).
  static constexpr uint32_t locked_mask = 0x40000000;
  /// Pending PDU value when the processor is idle.
  static constexpr uint32_t pending_pdu_count_idle = 0x0;
  /// Pending PDU value when the processor is stopped.
  static constexpr uint32_t pending_pdu_count_stopped = 0x7fffffff;
  /// PDU pending increment.
  static constexpr uint32_t pending_pdu_inc_queue = 0x1;
  /// PDU execution increment.
  static constexpr uint32_t pending_pdu_inc_exec = 0x1000;

  /// Counts the number of pending PDUs.
  std::atomic<uint32_t> pending_pdu_count = {};
  /// Current configured slot.
  slot_point configured_slot;
  /// \brief Counts the current number of pending PRACH detection tasks.
  ///
  /// This counter is mainly used during the stop process of the processor for guaranteeing all PRACH detection tasks
  /// are completed before returning from the stop() method.
  ///
  /// The counter is set to zero when no tasks are executed. It is set to \ref pending_pdu_count_stopped when no more
  /// PRACH detection tasks are allowed. Use is_state_stopped() for determining if the state is stopped.
  std::atomic<uint32_t> pending_prach_count = 0;

  /// Returns true if a state is idle.
  static bool is_state_idle(uint32_t state) { return (state == pending_pdu_count_idle); }
  /// Returns true if a state is accepting PDU.
  static bool is_state_accepting_pdu(uint32_t state) { return (state & accepting_pdu_mask); }
  /// Returns true if a state is locked.
  static bool is_state_locked(uint32_t state) { return (state & locked_mask); }
  /// \brief Returns true if a state is stopped.
  ///
  /// An internal state is considered stopped if it is equal to \ref pending_pdu_count_stopped.
  static bool is_state_stopped(uint32_t state) { return (state == pending_pdu_count_stopped); }
  /// Returns the total number of pending PDUs in execution for the given state.
  static unsigned get_nof_pending_pdu_in_exec(uint32_t state) { return (state & 0xfff000); }
  /// Returns the total number of pending PDUs in the queues.
  static unsigned get_nof_pending_pdu_in_queue(uint32_t state) { return (state & 0xfff); }
};

} // namespace srsran
