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

#include "srsran/adt/static_vector.h"
#include "srsran/phy/upper/uplink_pdu_slot_repository.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include <array>
#include <thread>
#include <variant>

namespace srsran {
/// Defines an entry of the uplink slot PDU repository.
using uplink_slot_pdu_entry = std::variant<uplink_pdu_slot_repository::pusch_pdu,
                                           uplink_pdu_slot_repository::pucch_pdu,
                                           uplink_pdu_slot_repository::srs_pdu>;

class uplink_pdu_slot_repository_impl : public unique_uplink_pdu_slot_repository::uplink_pdu_slot_repository_callback
{
public:
  // See interface for documentation.
  void add_pusch_pdu(const pusch_pdu& pdu) override
  {
    unsigned end_symbol_index = pdu.pdu.start_symbol_index + pdu.pdu.nof_symbols - 1;
    get_entry(end_symbol_index).push_back(pdu);
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
    get_entry(end_symbol_index).push_back(pdu);
    increment_pending_pdu_count();
  }

  // See interface for documentation.
  void add_srs_pdu(const srs_pdu& pdu) override
  {
    unsigned end_symbol_index =
        pdu.config.resource.start_symbol.to_uint() + static_cast<unsigned>(pdu.config.resource.nof_symbols) - 1;
    get_entry(end_symbol_index).push_back(pdu);
    increment_pending_pdu_count();
  }

  /// \brief Reserves the repository for a new slot context.
  /// \return \c true if the reservation is successful, otherwise \c false.
  bool reserve_on_new_slot()
  {
    // Try to write the accepting PDU mask, it is expected that the number of pending PDU is zero.
    uint32_t expected_pending_pdu_count = pending_pdu_count_idle;
    if (!pending_pdu_count.compare_exchange_weak(expected_pending_pdu_count, accepting_pdu_mask)) {
      return false;
    }

    // Clear all entries.
    for (auto& entry : repository) {
      entry.clear();
    }

    // Success.
    return true;
  }

  // See interface for documentation.
  void finish_adding_pdus() override
  {
    [[maybe_unused]] uint32_t prev = pending_pdu_count.fetch_xor(accepting_pdu_mask);
    srsran_assert((prev & accepting_pdu_mask) != 0, "Unexpected prev={:0x016} finishing PDUs.", prev);
  }

  /// \brief Notifies the event of creating a new asynchronous execution task. It increments the PDU being executed
  /// count.
  /// \return \c true if the internal state allows the creation of the task, otherwise \c false.
  bool on_create_pdu_task()
  {
    // Get current state.
    uint32_t current_state = pending_pdu_count.load();

    // Try to increment the number of PDUs to execute.
    while (!pending_pdu_count.compare_exchange_weak(current_state, current_state + pending_pdu_inc_exec)) {
      // Return false if the execution has already stopped.
      if (current_state == pending_pdu_count_stopped) {
        return false;
      }
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }

    // The exchange was successful, indicate the task can be created.
    return true;
  }

  /// \brief Notifies the completion of a PDU processing.
  ///
  /// Decrements the pending PDU counter.
  ///
  /// \remark An assertion is triggered if the pending PDU count contains the accepting PDU mask.
  void on_finish_processing_pdu()
  {
    [[maybe_unused]] uint32_t prev = pending_pdu_count.fetch_sub(pending_pdu_inc_queue + pending_pdu_inc_exec);
    srsran_assert((prev & accepting_pdu_mask) == 0, "The slot repository is in an unexpected state 0x{:016x}.", prev);
  }

  /// Stops the uplink PDU slot repository.
  void stop()
  {
    // As long as there are pending asynchronous tasks, wait for them to finish.
    for (uint32_t current_state = pending_pdu_count.load();
         ((current_state & 0xffff0000) != 0) ||
         !pending_pdu_count.compare_exchange_weak(current_state, pending_pdu_count_stopped);
         current_state = pending_pdu_count.load()) {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }

  /// Returns a span that contains the PDUs for the given slot and symbol index.
  span<const uplink_slot_pdu_entry> get_pdus(unsigned end_symbol_index) const { return get_entry(end_symbol_index); }

private:
  /// Accepting PDU state mask in the pending PDU count.
  static constexpr uint32_t accepting_pdu_mask = 1UL << 31U;
  /// Pending PDU value when the processor is idle.
  static constexpr uint32_t pending_pdu_count_idle = 0UL;
  /// Pending PDU value when the processor is stopped.
  static constexpr uint32_t pending_pdu_count_stopped = std::numeric_limits<int32_t>::max();
  /// PDU pending increment.
  static constexpr uint32_t pending_pdu_inc_queue = 1;
  /// PDU execution increment.
  static constexpr uint32_t pending_pdu_inc_exec = 1UL << 12;

  /// \brief Increment the pending PDU count.
  /// \remark An assertion is triggered if the pending PDU count contains the accepting PDU mask.
  void increment_pending_pdu_count()
  {
    [[maybe_unused]] uint32_t prev = pending_pdu_count.fetch_add(pending_pdu_inc_queue);
    srsran_assert((prev & accepting_pdu_mask) != 0, "The slot repository is the invalid state of NOT accepting PDUs.");
  }

  /// Repository that contains the PDUs.
  std::array<static_vector<uplink_slot_pdu_entry, MAX_UL_PDUS_PER_SLOT>, MAX_NSYMB_PER_SLOT> repository;
  /// Internal state, it counts the number of pending and executing PDUs using masks.
  std::atomic<uint32_t> pending_pdu_count = {};

  /// Returns a reference to the list of PDUs for the given slot and end symbol index.
  const static_vector<uplink_slot_pdu_entry, MAX_UL_PDUS_PER_SLOT>& get_entry(unsigned end_symbol_index) const
  {
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index.");
    return repository[end_symbol_index];
  }

  /// Returns a reference to the list of PDUs for the given slot and end symbol index.
  static_vector<uplink_slot_pdu_entry, MAX_UL_PDUS_PER_SLOT>& get_entry(unsigned end_symbol_index)
  {
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index.");
    return repository[end_symbol_index];
  }
};
} // namespace srsran
