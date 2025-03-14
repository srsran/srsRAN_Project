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
#include "srsran/phy/upper/uplink_processor.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include <array>
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

  /// \brief Prepares the slot repository for a new slot context.
  /// \return \c true if the new slot is configured successfully.
  bool new_slot()
  {
    // Try to write the accepting PDU mask, it is expected that the number of pending PDU is zero.
    uint64_t expected_pending_pdu_count = 0;
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
    [[maybe_unused]] uint64_t prev = pending_pdu_count.fetch_xor(accepting_pdu_mask);
    srsran_assert((prev & accepting_pdu_mask) != 0, "Unexpected prev={:0x016} finishing PDUs.", prev);
  }

  /// \brief Notifies the completion of a PDU processing.
  ///
  /// Decrements the pending PDU counter.
  ///
  /// \remark An assertion is triggered if the pending PDU count contains the accepting PDU mask.
  void on_finish_processing_pdu()
  {
    [[maybe_unused]] uint64_t prev = pending_pdu_count.fetch_sub(1);
    srsran_assert((prev & accepting_pdu_mask) == 0, "The slot repository is the invalid state of accepting PDUs.");
  }

  /// Returns a span that contains the PDUs for the given slot and symbol index.
  span<const uplink_slot_pdu_entry> get_pdus(unsigned end_symbol_index) const { return get_entry(end_symbol_index); }

private:
  /// Accepting PDU state mask in the pending PDU count.
  static constexpr uint64_t accepting_pdu_mask = 1UL << 63U;

  /// \brief Increment the pending PDU count.
  /// \remark An assertion is triggered if the pending PDU count contains the accepting PDU mask.
  void increment_pending_pdu_count()
  {
    [[maybe_unused]] uint64_t prev = pending_pdu_count.fetch_add(1);
    srsran_assert((prev & accepting_pdu_mask) != 0, "The slot repository is the invalid state of NOT accepting PDUs.");
  }

  /// Repository that contains the PDUs.
  std::array<static_vector<uplink_slot_pdu_entry, MAX_UL_PDUS_PER_SLOT>, MAX_NSYMB_PER_SLOT> repository;
  /// Counts the number of pending PDUs.
  std::atomic<uint64_t> pending_pdu_count = {};

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
