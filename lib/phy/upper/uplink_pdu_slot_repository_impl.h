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

#include "uplink_processor_fsm.h"
#include "srsran/adt/static_vector.h"
#include "srsran/phy/upper/uplink_pdu_slot_repository.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include <array>
#include <variant>

namespace srsran {

/// \brief Implements an the uplink slot repository.
///
/// It relies on a finite-state machine to decide whether new PDUs can be accepted and to know whether the registered
/// PDUs are being processed.
class uplink_pdu_slot_repository_impl : public unique_uplink_pdu_slot_repository::uplink_pdu_slot_repository_callback,
                                        private shared_resource_grid::pool_interface
{
public:
  /// Creates an uplink PDU slot repository.
  uplink_pdu_slot_repository_impl(resource_grid&                 grid_,
                                  std::atomic<unsigned>&         grid_ref_counter_,
                                  uplink_processor_fsm_notifier& fsm_) :
    grid(grid_), grid_ref_counter(grid_ref_counter_), fsm_notifier(fsm_)
  {
  }

  /// Uplink slot repository destructor.
  ~uplink_pdu_slot_repository_impl() override
  {
    // Wait for the resource grid to be returned to the pool.
    for (unsigned current_grid_ref_counter = grid_ref_counter.load(std::memory_order_acquire);
         current_grid_ref_counter != 0;
         current_grid_ref_counter = grid_ref_counter.load(std::memory_order_acquire)) {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    };
  }

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

  // See the uplink_pdu_slot_repository interface for documentation.
  void add_pusch_pdu(const pusch_pdu& pdu) override
  {
    unsigned end_symbol_index = pdu.pdu.start_symbol_index + pdu.pdu.nof_symbols - 1;
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index {}.", end_symbol_index);

    pusch_repository[end_symbol_index].push_back(pdu);
    fsm_notifier.increment_pending_pdu_count();
  }

  // See the uplink_pdu_slot_repository interface for documentation.
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
      fsm_notifier.increment_pending_pdu_count();
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
    fsm_notifier.increment_pending_pdu_count();
  }

  // See the uplink_pdu_slot_repository interface for documentation.
  void add_srs_pdu(const srs_pdu& pdu) override
  {
    unsigned end_symbol_index =
        pdu.config.resource.start_symbol.to_uint() + static_cast<unsigned>(pdu.config.resource.nof_symbols) - 1;
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index {}.", end_symbol_index);
    srs_repository[end_symbol_index].push_back(pdu);
    fsm_notifier.increment_pending_pdu_count();
  }

  /// \brief Clears all the PDU queues of the repository.
  void clear_queues()
  {
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
  }

  // See the unique_uplink_pdu_slot_repository::uplink_pdu_slot_repository_callback interface for documentation.
  shared_resource_grid finish_adding_pdus() override
  {
    fsm_notifier.stop_accepting_pdu();

    // Set grid reference counter to one.
    grid_ref_counter = 1;

    return {*this, grid_ref_counter};
  }

  /// Returns a span that contains the PUSCH PDUs for the given slot and symbol index.
  span<const pusch_pdu> get_pusch_pdus(unsigned end_symbol_index) const
  {
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index {}.", end_symbol_index);
    return pusch_repository[end_symbol_index];
  }

  /// Returns a span that contains the PUCCH PDUs for the given slot and symbol index.
  span<const pucch_pdu> get_pucch_pdus(unsigned end_symbol_index) const
  {
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index {}.", end_symbol_index);
    return pucch_repository[end_symbol_index];
  }

  /// Returns a span that contains the PUCCH PDUs for the given slot and symbol index.
  span<const pucch_f1_collection> get_pucch_f1_repository(unsigned end_symbol_index) const
  {
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index {}.", end_symbol_index);
    return pucch_f1_repository[end_symbol_index];
  }

  /// Returns a span that contains the SRS PDUs for the given slot and symbol index.
  span<const srs_pdu> get_srs_pdus(unsigned end_symbol_index) const
  {
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index {}.", end_symbol_index);
    return srs_repository[end_symbol_index];
  }

private:
  // See the shared_resource_grid::pool_interface interface for documentation.
  resource_grid& get() override { return grid; }

  // See the shared_resource_grid::pool_interface interface for documentation.
  void notify_release_scope() override {}

  /// Repository that contains PUSCH PDUs.
  std::array<static_vector<pusch_pdu, MAX_PUSCH_PDUS_PER_SLOT>, MAX_NSYMB_PER_SLOT> pusch_repository;
  /// Repository that contains PUCCH PDUs.
  std::array<static_vector<pucch_pdu, MAX_PUCCH_PDUS_PER_SLOT>, MAX_NSYMB_PER_SLOT> pucch_repository;
  /// Repository that contains collections of PUCCH Format 1.
  std::array<static_vector<pucch_f1_collection, MAX_PUCCH_PDUS_PER_SLOT>, MAX_NSYMB_PER_SLOT> pucch_f1_repository;
  /// Repository that contains SRS PDUs.
  std::array<static_vector<srs_pdu, MAX_SRS_PDUS_PER_SLOT>, MAX_NSYMB_PER_SLOT> srs_repository;
  /// Resource grid associated to the uplink slot.
  resource_grid& grid;
  /// Resource grid reference counter.
  std::atomic<unsigned>& grid_ref_counter;
  /// Notifier for the uplink processor finite-state machine.
  uplink_processor_fsm_notifier& fsm_notifier;
};
} // namespace srsran
