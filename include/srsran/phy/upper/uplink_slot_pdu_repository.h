/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/adt/variant.h"
#include "srsran/phy/upper/uplink_processor.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include <vector>

namespace srsran {

/// Defines an entry of the uplink slot PDU repository.
using uplink_slot_pdu_entry = variant<uplink_processor::pusch_pdu, uplink_processor::pucch_pdu>;

/// \brief Uplink slot PDU repository.
///
/// This class registers PUSCH and PUCCH PDUs. PDUs are indexed by the uplink slot they will be processed in.
// :TODO: move this class to private when the uplink processor gets refactorized.
class uplink_slot_pdu_repository
{
  using slot_entry = static_vector<uplink_slot_pdu_entry, MAX_UL_PDUS_PER_SLOT>;

public:
  /// \brief Constructs an uplink slot pdu repository that supports the given number of slots.
  ///
  /// \param nof_slots_[in] Number of slots supported by the repository.
  explicit uplink_slot_pdu_repository(unsigned nof_slots_) : nof_slots(nof_slots_), repository(nof_slots_) {}

  /// Adds the given PUSCH PDU to the repository at the given slot.
  void add_pusch_pdu(slot_point slot, const uplink_processor::pusch_pdu& pdu)
  {
    repository[slot.to_uint() % nof_slots].push_back(pdu);
  }

  /// Adds the given PUCCH PDU to the repository at the given slot.
  void add_pucch_pdu(slot_point slot, const uplink_processor::pucch_pdu& pdu)
  {
    repository[slot.to_uint() % nof_slots].push_back(pdu);
  }

  /// Clears the given slot of the registry.
  void clear_slot(slot_point slot) { repository[slot.to_uint() % nof_slots].clear(); }

  /// Returns a span that contains the PDUs for the given slot.
  span<const uplink_slot_pdu_entry> get_pdus(slot_point slot) const { return repository[slot.to_uint() % nof_slots]; }

private:
  /// Number of slots.
  const size_t nof_slots;
  /// Repository that contains the PDUs.
  std::vector<slot_entry> repository;
};
} // namespace srsran
