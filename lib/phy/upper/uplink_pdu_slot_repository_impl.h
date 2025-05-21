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

#include "srsran/adt/static_vector.h"
#include "srsran/phy/upper/uplink_pdu_slot_repository.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include <array>
#include <thread>
#include <variant>

namespace srsran {

/// \brief Implements an the uplink slot repository.
///
/// The implementation is build with an internal finite state machine with the following states:
/// - Idle: the repository does not contain any PDU and it is not processing;
/// - Accepting: the repository accepts the enqueueing of new PDUs;
/// - Waiting: the repository contains PDUs but it has not processed any yet;
/// - Processing: the repository contains PDUs and it is processing PDUs;
/// - Discarding slot: the repository is in a state of discarding the collected PDUs; and
/// - Stopped: the repository does not accept transitioning to any other state.
class uplink_pdu_slot_repository_impl : public unique_uplink_pdu_slot_repository::uplink_pdu_slot_repository_callback
{
public:
  /// PUCCH Format 1 aggregated configuration.
  struct pucch_f1_collection {
    /// Pairs the reception context with the UE dedicated parameters.
    struct ue_entry {
      /// UE reception context.
      ul_pucch_context context;
      /// Transmission initial cyclic shift.
      unsigned initial_cyclic_shift;
      /// Transmission time domain OCC.
      unsigned time_domain_occ;
    };
    /// Common configuration.
    pucch_processor::format1_batch_configuration config;
    /// UE common configurations.
    static_vector<ue_entry, MAX_PUCCH_PDUS_PER_SLOT> ue_contexts;
  };

  // See interface for documentation.
  void add_pusch_pdu(const pusch_pdu& pdu) override
  {
    unsigned end_symbol_index = pdu.pdu.start_symbol_index + pdu.pdu.nof_symbols - 1;
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index {}.", end_symbol_index);

    pusch_repository[end_symbol_index].push_back(pdu);
    increment_pending_pdu_count();
  }

  // See interface for documentation.
  void add_pucch_pdu(const pucch_pdu& pdu) override
  {
    auto fetch_end_symbol_index = [](const pucch_pdu& _pdu) {
      return std::visit([](const auto& config) { return config.start_symbol_index + config.nof_symbols - 1; },
                        _pdu.config);
    };

    unsigned end_symbol_index = fetch_end_symbol_index(pdu);
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index {}.", end_symbol_index);

    if (!std::holds_alternative<pucch_processor::format1_configuration>(pdu.config)) {
      pucch_repository[end_symbol_index].push_back(pdu);
      increment_pending_pdu_count();
      return;
    }

    const auto&                                   config = std::get<pucch_processor::format1_configuration>(pdu.config);
    pucch_processor::format1_common_configuration common_config(config);
    pucch_f1_collection::ue_entry                 ue_entry = {.context              = pdu.context,
                                                              .initial_cyclic_shift = config.initial_cyclic_shift,
                                                              .time_domain_occ      = config.time_domain_occ};

    // Select Format 1 PUCCH configuration collection.
    span<pucch_f1_collection> f1_collections = pucch_f1_repository[end_symbol_index];

    // Find a compatible Format 1 common configuration.
    auto* it =
        std::find_if(f1_collections.begin(), f1_collections.end(), [&common_config](const pucch_f1_collection& entry) {
          return common_config == entry.config.common_config;
        });

    // If the common configuration was found.
    if (it != f1_collections.end()) {
      // Push back the UE dedicated entry in the existing collection.
      it->config.entries.insert(config.initial_cyclic_shift,
                                config.time_domain_occ,
                                {.context = config.context, .nof_harq_ack = config.nof_harq_ack});
      it->ue_contexts.emplace_back(ue_entry);
      return;
    }

    // If the common configuration is not yet in the list, push back a complete new collection.
    pucch_f1_repository[end_symbol_index].emplace_back(
        pucch_f1_collection{.config = pucch_processor::format1_batch_configuration(config), .ue_contexts = {ue_entry}});

    // Only increment per collection.
    increment_pending_pdu_count();
  }

  // See interface for documentation.
  void add_srs_pdu(const srs_pdu& pdu) override
  {
    unsigned end_symbol_index =
        pdu.config.resource.start_symbol.to_uint() + static_cast<unsigned>(pdu.config.resource.nof_symbols) - 1;
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index {}.", end_symbol_index);
    srs_repository[end_symbol_index].push_back(pdu);
    increment_pending_pdu_count();
  }

  /// \brief Reserves the repository for a new slot context.
  ///
  /// A reservation is not successful if the current state is not idle.
  ///
  /// \return \c true if the reservation is successful, otherwise \c false.
  bool reserve_on_new_slot(slot_point new_slot)
  {
    // Try transitioning state from idle to accepting PDU.
    uint32_t expected_pending_pdu_count = pending_pdu_count_idle;
    if (!pending_pdu_count.compare_exchange_weak(expected_pending_pdu_count, accepting_pdu_mask)) {
      return false;
    }

    // Overwrite configured slot.
    configured_slot = new_slot;

    // Clear all entries.
    for (auto& entry : pusch_repository) {
      entry.clear();
    }
    for (auto& entry : pucch_repository) {
      entry.clear();
    }
    for (auto& entry : pucch_f1_repository) {
      entry.clear();
    }
    for (auto& entry : srs_repository) {
      entry.clear();
    }

    // Success.
    return true;
  }

  // See interface for documentation.
  void finish_adding_pdus() override
  {
    // Remove accepting PDU mask and verify the previous state was accepting PDU.
    [[maybe_unused]] uint32_t prev = pending_pdu_count.fetch_xor(accepting_pdu_mask);
    srsran_assert(is_state_accepting_pdu(prev), "Unexpected prev={:08x} finishing PDUs.", prev);
  }

  /// \brief Notifies the start of a slot discard.
  ///
  /// The current state does not allow discarding the slot if:
  /// - is stopped;
  /// - is discarding (to avoid discarding twice); or
  /// - there are no pending PDUs to discard.
  ///
  /// \return \c true if the current state allows to discard the slot, otherwise \c false.
  bool start_discard_slot()
  {
    // Get current state.
    uint32_t current_state = pending_pdu_count.load();

    uint32_t nof_pending_pdu_in_queue = current_state & 0xfff;

    // Skip function - returns true if the slot discard is not necessary.
    auto skip_function = [&current_state]() {
      // Skip if the state is stopped, already discarding or no pending PDU.
      return is_state_stopped(current_state) || is_state_discarding(current_state) ||
             !has_state_pending_pdu_in_queue(current_state);
    };

    // Try to set the number of pending execute PDU equal to the number of enqueued tasks and set the discarding slot
    // mask.
    bool success = false;
    while (!skip_function() && !success) {
      success = pending_pdu_count.compare_exchange_weak(
          current_state, current_state | (nof_pending_pdu_in_queue * pending_pdu_inc_exec) | discarding_slot_mask);
    }

    return success;
  }

  /// \brief Notifies the completion of the slot discard.
  ///
  /// Transitions the state to idle.
  ///
  /// \remark An assertion is triggered if there are still PDUs to notify.
  void finish_discard_slot()
  {
    [[maybe_unused]] uint32_t prev_state = pending_pdu_count.exchange(pending_pdu_count_idle);
    srsran_assert(!has_state_pending_pdu_in_queue(prev_state) && !has_state_pending_pdu_in_exec(prev_state) &&
                      is_state_discarding(prev_state),
                  "Unexpected state after discarding slot 0x{:08x}",
                  prev_state);
  }

  /// \brief Returns \c true if the repository is configured with the given slot and ready to process PDUs.
  ///
  /// The slot is considered invalid if :
  /// - the current state is accepting PDUs, idle, discarding a slot, or stopped; or
  /// - the given slot is not equal to the configured one.
  bool is_slot_valid(slot_point slot) const
  {
    // Verify that the repository is in a valid state to process PDUs.
    uint32_t current_state = pending_pdu_count.load();
    if (is_state_idle(current_state) || is_state_accepting_pdu(current_state) || is_state_discarding(current_state) ||
        is_state_stopped(current_state)) {
      return false;
    }

    // Checks the slot validity.
    return configured_slot == slot;
  }

  /// \brief Notifies the event of creating a new asynchronous execution task. It increments the PDU being executed
  /// count.
  /// \return \c true if the internal state allows the creation of the task, otherwise \c false.
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

      srsran_assert(!is_state_accepting_pdu(current_state) && has_state_pending_pdu_in_queue(current_state),
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
    srsran_assert(!is_state_accepting_pdu(prev) && !is_state_stopped(prev) && has_state_pending_pdu_in_exec(prev) &&
                      has_state_pending_pdu_in_queue(prev),
                  "The slot repository is in an unexpected state 0x{:08x}.",
                  prev);
  }

  /// Returns a span that contains the PUSCH PDUs for the given slot and symbol index.
  span<const pusch_pdu> get_pusch_pdus(unsigned end_symbol_index) const
  {
    // Skip if there are no pending PDUs. It could transition to idle and queue new PDUs any time, resulting in a race
    // condition.
    if (!has_state_pending_pdu_in_queue(pending_pdu_count.load())) {
      return {};
    }

    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index {}.", end_symbol_index);
    return pusch_repository[end_symbol_index];
  }

  /// Returns a span that contains the PUCCH PDUs for the given slot and symbol index.
  span<const pucch_pdu> get_pucch_pdus(unsigned end_symbol_index) const
  {
    // Skip if there are no pending PDUs. It could transition to idle and queue new PDUs any time, resulting in a race
    // condition.
    if (!has_state_pending_pdu_in_queue(pending_pdu_count.load())) {
      return {};
    }

    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index {}.", end_symbol_index);
    return pucch_repository[end_symbol_index];
  }

  /// Returns a span that contains the PUCCH PDUs for the given slot and symbol index.
  span<const pucch_f1_collection> get_pucch_f1_repository(unsigned end_symbol_index) const
  {
    // Skip if there are no pending PDUs. It could transition to idle and queue new PDUs any time, resulting in a race
    // condition.
    if (!has_state_pending_pdu_in_queue(pending_pdu_count.load())) {
      return {};
    }

    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index {}.", end_symbol_index);
    return pucch_f1_repository[end_symbol_index];
  }

  /// Returns a span that contains the SRS PDUs for the given slot and symbol index.
  span<const srs_pdu> get_srs_pdus(unsigned end_symbol_index) const
  {
    // Skip if there are no pending PDUs. It could transition to idle and queue new PDUs any time, resulting in a race
    // condition.
    if (!has_state_pending_pdu_in_queue(pending_pdu_count.load())) {
      return {};
    }

    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index {}.", end_symbol_index);
    return srs_repository[end_symbol_index];
  }

  /// \brief Stops the uplink PDU slot repository.
  ///
  /// It waits as long as:
  /// - the repository is active accepting PDUs;
  /// - there are asynchronous tasks being executed; or
  /// - it is in the process of discarding a slot.
  void stop()
  {
    // As long as there are pending asynchronous tasks, wait for them to finish.
    for (uint32_t current_state = pending_pdu_count.load();
         is_state_accepting_pdu(current_state) || has_state_pending_pdu_in_exec(current_state) ||
         is_state_discarding(current_state) ||
         !pending_pdu_count.compare_exchange_weak(current_state, pending_pdu_count_stopped);
         current_state = pending_pdu_count.load()) {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }

private:
  /// Accepting PDU state mask in the pending PDU count.
  static constexpr uint32_t accepting_pdu_mask = 0x80000000;
  /// The current state is discarding the slot.
  static constexpr uint32_t discarding_slot_mask = 0x40000000;
  /// Pending PDU value when the processor is idle.
  static constexpr uint32_t pending_pdu_count_idle = 0x0;
  /// Pending PDU value when the processor is stopped.
  static constexpr uint32_t pending_pdu_count_stopped = 0x7fffffff;
  /// PDU pending increment.
  static constexpr uint32_t pending_pdu_inc_queue = 0x1;
  /// PDU execution increment.
  static constexpr uint32_t pending_pdu_inc_exec = 0x1000;

  /// Returns \c true if a state is idle.
  static constexpr bool is_state_idle(uint32_t state) { return state == pending_pdu_count_idle; }

  /// Returns \c true if a state is stopped.
  static constexpr bool is_state_stopped(uint32_t state) { return state == pending_pdu_count_stopped; }

  /// Returns \c true if a state is in process of discarding the slot.
  static constexpr bool is_state_discarding(uint32_t state) { return state & discarding_slot_mask; }

  /// Returns \c true if a state is accepting PDU.
  static constexpr bool is_state_accepting_pdu(uint32_t state) { return state & accepting_pdu_mask; }

  /// Returns \c true if a state contains pending PDUs in queues.
  static constexpr bool has_state_pending_pdu_in_queue(uint32_t state) { return state & 0xfff; }

  /// Returns \c true if a state contains pending PDUs in execution.
  static constexpr bool has_state_pending_pdu_in_exec(uint32_t state) { return state & 0xfff000; }

  /// \brief Increment the pending PDU count.
  /// \remark An assertion is triggered if the pending PDU count contains the accepting PDU mask.
  void increment_pending_pdu_count()
  {
    [[maybe_unused]] uint32_t prev = pending_pdu_count.fetch_add(pending_pdu_inc_queue);
    srsran_assert((prev & accepting_pdu_mask) != 0, "The slot repository is the invalid state of NOT accepting PDUs.");
  }

  /// Repository that contains PUSCH PDUs.
  std::array<static_vector<pusch_pdu, MAX_PUSCH_PDUS_PER_SLOT>, MAX_NSYMB_PER_SLOT> pusch_repository;
  /// Repository that contains PUCCH PDUs.
  std::array<static_vector<pucch_pdu, MAX_PUCCH_PDUS_PER_SLOT>, MAX_NSYMB_PER_SLOT> pucch_repository;
  /// Repository that contains collections of PUCCH Format 1.
  std::array<static_vector<pucch_f1_collection, MAX_PUCCH_PDUS_PER_SLOT>, MAX_NSYMB_PER_SLOT> pucch_f1_repository;
  /// Repository that contains SRS PDUs.
  std::array<static_vector<srs_pdu, MAX_SRS_PDUS_PER_SLOT>, MAX_NSYMB_PER_SLOT> srs_repository;
  /// Counts the number of pending PDUs.
  std::atomic<uint32_t> pending_pdu_count = {};
  /// Current configured slot.
  slot_point configured_slot;
};
} // namespace srsran
