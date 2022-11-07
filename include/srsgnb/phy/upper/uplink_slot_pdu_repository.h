/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/upper/uplink_processor.h"
#include "srsgnb/ran/slot_pdu_capacity_contants.h"
#include <vector>

namespace srsgnb {

/// Defines an entry of the uplink slot PDU repository.
struct uplink_slot_pdu_entry {
  /// Labels for the supported PDU types.
  enum class pdu_type { PUSCH, PUCCH };

  /// PDU type.
  pdu_type type;
  // :TODO: convert this to variant.
  /// PUSCH PDU.
  uplink_processor::pusch_pdu pusch;
  /// PUCCH PDU.
  uplink_processor::pucch_pdu pucch;
};

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
  /// \param nof_slots[in] Number of slots supported by the repository.
  explicit uplink_slot_pdu_repository(unsigned nof_slots) : nof_slots(nof_slots), repository(nof_slots) {}

  /// Adds the given PUSCH PDU to the repository at the given slot.
  void add_pusch_pdu(slot_point slot, const uplink_processor::pusch_pdu& pdu)
  {
    uplink_slot_pdu_entry entry;
    entry.type  = uplink_slot_pdu_entry::pdu_type::PUSCH;
    entry.pusch = pdu;
    entry.pucch = {};

    repository[slot.to_uint() % nof_slots].push_back(entry);
  }

  /// Adds the given PUCCH PDU to the repository at the given slot.
  void add_pucch_pdu(slot_point slot, const uplink_processor::pucch_pdu& pdu)
  {
    uplink_slot_pdu_entry entry;
    entry.type  = uplink_slot_pdu_entry::pdu_type::PUCCH;
    entry.pucch = pdu;
    entry.pusch = {};

    repository[slot.to_uint() % nof_slots].push_back(entry);
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
} // namespace srsgnb
