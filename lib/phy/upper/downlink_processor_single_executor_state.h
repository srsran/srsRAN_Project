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

#include "srsran/phy/upper/channel_processors/pdsch/formatters.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

/// \brief Downlink processor state management class.
///
/// Represents the internal state of the upper PHY downlink processor single executor implementation.
class downlink_processor_single_executor_state
{
public:
  /// \brief Reserves the downlink processor upon the configuration of the resource grid.
  /// \return \c true if the downlink processor is idle. Otherwise, \c false.
  bool reserve_on_resource_grid_configure()
  {
    // Transition to accepting PDUs only if the current state is idle.
    uint32_t expected_idle = state_pending_pdus_idle;
    return state_pending_pdus.compare_exchange_weak(expected_idle, state_pending_pdus_mask_accepting_pdus);
  }

  /// \brief Notifies that all PDUs have already been queued and no more PDUs are accepted.
  /// \return \c true if the there are no pending PDUs to process, \c false otherwise.
  /// \remark An assertion is triggered if the current state does not contain the accepting PDUs mask.
  bool on_finish_requested()
  {
    // Remove the accepting PDU mask.
    uint32_t prev = state_pending_pdus.fetch_xor(state_pending_pdus_mask_accepting_pdus);

    // Assert that the accepting PDUs mask was there.
    srsran_assert((prev & state_pending_pdus_mask_accepting_pdus) != 0,
                  "The downlink processor is in an unexpected state state 0x{:08x}.",
                  prev);

    // There are no pending PDUs if the previous value is equal to the mask.
    return (prev == state_pending_pdus_mask_accepting_pdus);
  }

  /// \brief Notifies the resource grid has been sent over the resource grid gateway and the downlink processor.
  /// \remark An assertion is triggered if the current state is not finishing PDUs.
  void on_grid_sent()
  {
    // Transition to idle regardless of the state and trigger an assertion if the state is not expected.
    [[maybe_unused]] uint32_t prev = state_pending_pdus.exchange(state_pending_pdus_idle);
    srsran_assert(
        prev == state_pending_pdus_finishing, "The downlink processor is in an unexpected state state 0x{:08x}.", prev);
  }

  /// \brief Notifies the queuing of a new PDU processing task.
  /// \remark An assertion is triggered if the current state does not accept new processing PDUs.
  void on_task_creation()
  {
    [[maybe_unused]] uint32_t prev = state_pending_pdus.fetch_add(1);
    srsran_assert((prev & state_pending_pdus_mask_accepting_pdus) != 0,
                  "The downlink processor is in an unexpected state state 0x{:08x}.",
                  prev);
  }

  /// \brief Notifies the completion of a PDU processing task.
  /// \return \c true if the resource grid can be immediately sent, \c false otherwise.
  /// \remark An assertion is triggered if the current state is zero which means no task was pending.
  bool on_task_completion()
  {
    uint32_t prev = state_pending_pdus.fetch_sub(1);
    srsran_assert((prev & state_pending_pdus_mask_count) != 0,
                  "The downlink processor number of pending PDUs cannot be zero.");
    return prev == 1;
  }

private:
  /// Idle state - represents the state in which the downlink processor does not accept PDUs and does not have any
  /// pending PDU to process.
  static constexpr uint32_t state_pending_pdus_idle = 0x7fffffff;
  /// The downlink processor state that accepts PDUs.
  static constexpr uint32_t state_pending_pdus_mask_accepting_pdus = 0x80000000;
  /// The downlink processor pending PDUs counter mask.
  static constexpr uint32_t state_pending_pdus_mask_count = ~state_pending_pdus_mask_accepting_pdus;
  /// The downlink does not have any-more PDUs to process, and it is waiting to send the resource grid.
  static constexpr uint32_t state_pending_pdus_finishing = 0x00000000;

  /// Current state and number of pending PDUs to process.
  std::atomic<uint32_t> state_pending_pdus = state_pending_pdus_idle;
};
} // namespace srsran
