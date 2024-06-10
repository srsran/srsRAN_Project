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

#include "srsran/phy/upper/uplink_processor.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include <variant>
#include <vector>

namespace srsran {

/// Defines an entry of the uplink slot PDU repository.
using uplink_slot_pdu_entry =
    std::variant<uplink_processor::pusch_pdu, uplink_processor::pucch_pdu, uplink_processor::srs_pdu>;

/// \brief Uplink slot PDU repository.
///
/// This class registers PUSCH and PUCCH PDUs. PDUs are indexed by the uplink slot they will be processed in.
// :TODO: move this class to private when the uplink processor gets refactorized.
class uplink_slot_pdu_repository
{
  using slot_entry = std::array<static_vector<uplink_slot_pdu_entry, MAX_UL_PDUS_PER_SLOT>, MAX_NSYMB_PER_SLOT>;

  /// Number of slots.
  const size_t nof_slots;
  /// Repository that contains the PDUs.
  std::vector<slot_entry> repository;

public:
  /// \brief Constructs an uplink slot pdu repository that supports the given number of slots.
  ///
  /// \param nof_slots_[in] Number of slots supported by the repository.
  explicit uplink_slot_pdu_repository(unsigned nof_slots_) : nof_slots(nof_slots_), repository(nof_slots_) {}

  /// Adds the given PUSCH PDU to the repository at the given slot.
  void add_pusch_pdu(slot_point slot, const uplink_processor::pusch_pdu& pdu)
  {
    assert_slot(slot);

    unsigned end_symbol_index = pdu.pdu.start_symbol_index + pdu.pdu.nof_symbols - 1;
    get_entry(slot, end_symbol_index).push_back(pdu);
  }

  /// Adds the given PUCCH PDU to the repository at the given slot.
  void add_pucch_pdu(slot_point slot, const uplink_processor::pucch_pdu& pdu)
  {
    assert_slot(slot);

    auto fetch_end_symbol_index = [](const uplink_processor::pucch_pdu& _pdu) {
      switch (_pdu.context.format) {
        case pucch_format::FORMAT_0:
          return _pdu.format0.start_symbol_index + _pdu.format0.nof_symbols - 1;
        case pucch_format::FORMAT_1:
          return _pdu.format1.start_symbol_index + _pdu.format1.nof_symbols - 1;
        case pucch_format::FORMAT_2:
          return _pdu.format2.start_symbol_index + _pdu.format2.nof_symbols - 1;
        default:
          srsran_assert(false, "Unsupported PUCCH format");
          return 0U;
      }
    };

    unsigned end_symbol_index = fetch_end_symbol_index(pdu);
    get_entry(slot, end_symbol_index).push_back(pdu);
  }

  /// Adds the given SRS PDU to the repository at the given slot.
  void add_srs_pdu(slot_point slot, const uplink_processor::srs_pdu& pdu)
  {
    assert_slot(slot);

    unsigned end_symbol_index =
        pdu.config.resource.start_symbol.to_uint() + static_cast<unsigned>(pdu.config.resource.nof_symbols) - 1;
    get_entry(slot, end_symbol_index).push_back(pdu);
  }

  /// Clears the given slot of the registry.
  void clear_slot(slot_point slot)
  {
    assert_slot(slot);

    for (auto& entry : repository[slot.to_uint() % nof_slots]) {
      entry = {};
    }
  }

  /// Returns a span that contains the PDUs for the given slot and symbol index.
  span<const uplink_slot_pdu_entry> get_pdus(slot_point slot, unsigned end_symbol_index) const
  {
    assert_slot(slot);

    return get_entry(slot, end_symbol_index);
  }

private:
  /// Asserts the slot numerology with the pool dimensions.
  void assert_slot(slot_point slot) const
  {
    srsran_assert(slot.nof_slots_per_system_frame() % nof_slots == 0,
                  "The minimum supported number of slots per system frame (i.e., {}) must be divisible by the number "
                  "of slots (i.e., {})",
                  slot.nof_slots_per_system_frame(),
                  nof_slots);
  }

  /// Returns a reference to the list of PDUs for the given slot and end symbol index.
  const static_vector<uplink_slot_pdu_entry, MAX_UL_PDUS_PER_SLOT>& get_entry(slot_point slot,
                                                                              unsigned   end_symbol_index) const
  {
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index");
    return repository[slot.to_uint() % nof_slots][end_symbol_index];
  }

  /// Returns a reference to the list of PDUs for the given slot and end symbol index.
  static_vector<uplink_slot_pdu_entry, MAX_UL_PDUS_PER_SLOT>& get_entry(slot_point slot, unsigned end_symbol_index)
  {
    srsran_assert(end_symbol_index < MAX_NSYMB_PER_SLOT, "Invalid end symbol index");
    return repository[slot.to_uint() % nof_slots][end_symbol_index];
  }
};

} // namespace srsran
