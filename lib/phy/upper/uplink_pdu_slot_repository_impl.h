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
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index.");

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
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index.");

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
    auto it =
        std::find_if(f1_collections.begin(), f1_collections.end(), [&common_config](const pucch_f1_collection& entry) {
          return common_config == entry.config.common_config;
        });

    // If the common configuration was found.
    if (it != f1_collections.end()) {
      // Push back the UE dedicated entry in the existing collection.
      it->config.entries.emplace(config.initial_cyclic_shift,
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
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index.");
    srs_repository[end_symbol_index].push_back(pdu);
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
    [[maybe_unused]] uint32_t prev = pending_pdu_count.fetch_xor(accepting_pdu_mask);
    srsran_assert((prev & accepting_pdu_mask) != 0, "Unexpected prev={:08x} finishing PDUs.", prev);
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
      if (current_state == pending_pdu_count_stopped) {
        return true;
      }

      srsran_assert(((current_state & accepting_pdu_mask) == 0) && ((current_state & 0xfff) != 0),
                    "The slot repository is in an unexpected state 0x{:08x}.",
                    current_state);

      return false;
    };

    // Try to increment the number of PDUs to execute.
    bool success = false;
    while (!stop_function() &&
           !(success = pending_pdu_count.compare_exchange_weak(current_state, current_state + pending_pdu_inc_exec))) {
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

    srsran_assert(((prev & accepting_pdu_mask) == 0) && (prev != pending_pdu_count_stopped) &&
                      ((prev & 0xfff000) != 0) && ((prev & 0xfff) != 0),
                  "The slot repository is in an unexpected state 0x{:08x}.",
                  prev);
  }

  /// Returns a span that contains the PUSCH PDUs for the given slot and symbol index.
  span<const pusch_pdu> get_pusch_pdus(unsigned end_symbol_index) const
  {
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index.");
    return pusch_repository[end_symbol_index];
  }

  /// Returns a span that contains the PUCCH PDUs for the given slot and symbol index.
  span<const pucch_pdu> get_pucch_pdus(unsigned end_symbol_index) const
  {
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index.");
    return pucch_repository[end_symbol_index];
  }

  /// Returns a span that contains the PUCCH PDUs for the given slot and symbol index.
  span<const pucch_f1_collection> get_pucch_f1_repository(unsigned end_symbol_index) const
  {
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index.");
    return pucch_f1_repository[end_symbol_index];
  }

  /// Returns a span that contains the SRS PDUs for the given slot and symbol index.
  span<const srs_pdu> get_srs_pdus(unsigned end_symbol_index) const
  {
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index.");
    return srs_repository[end_symbol_index];
  }

  /// \brief Stops the uplink PDU slot repository.
  ///
  /// It waits as long as:
  /// - the repository is active accepting PDUs; and
  /// - there are asynchronous tasks being executed.
  void stop()
  {
    // As long as there are pending asynchronous tasks, wait for them to finish.
    for (uint32_t current_state = pending_pdu_count.load();
         (current_state & accepting_pdu_mask) || (current_state & 0xfff000) ||
         !pending_pdu_count.compare_exchange_weak(current_state, pending_pdu_count_stopped);
         current_state = pending_pdu_count.load()) {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }

private:
  /// Accepting PDU state mask in the pending PDU count.
  static constexpr uint32_t accepting_pdu_mask = 0x80000000;
  /// Pending PDU value when the processor is idle.
  static constexpr uint32_t pending_pdu_count_idle = 0x0;
  /// Pending PDU value when the processor is stopped.
  static constexpr uint32_t pending_pdu_count_stopped = 0x7fffffff;
  /// PDU pending increment.
  static constexpr uint32_t pending_pdu_inc_queue = 0x1;
  /// PDU execution increment.
  static constexpr uint32_t pending_pdu_inc_exec = 0x1000;

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
};
} // namespace srsran
