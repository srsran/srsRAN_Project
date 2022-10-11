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

#include "srsgnb/adt/circular_array.h"
#include "srsgnb/phy/upper/uplink_processor.h"

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
  /// \brief Maximum number of PDUs per slot.
  // :TODO: Use the RAN constants here when the PR is merged.
  static constexpr unsigned MAX_NUM_PDUS = 1024;

  /// \brief Maximum number of slots supported by the registry.
  ///
  /// In the worst case (subcarrier spacing of 240kHz), this number should be equivalent to 20ms.
  static constexpr unsigned MAX_NUM_SLOTS = 320;

  using slot_entry = static_vector<uplink_slot_pdu_entry, MAX_NUM_PDUS>;

public:
  /// Adds the given PUSCH PDU to the repository at the given slot.
  void add_pusch_pdu(slot_point slot, const uplink_processor::pusch_pdu& pdu)
  {
    uplink_slot_pdu_entry entry;
    entry.type  = uplink_slot_pdu_entry::pdu_type::PUSCH;
    entry.pusch = pdu;
    entry.pucch = {};

    repository[slot.to_uint()].push_back(entry);
  }

  /// Adds the given PUCCH PDU to the repository at the given slot.
  void add_pucch_pdu(slot_point slot, const uplink_processor::pucch_pdu& pdu)
  {
    uplink_slot_pdu_entry entry;
    entry.type  = uplink_slot_pdu_entry::pdu_type::PUCCH;
    entry.pucch = pdu;
    entry.pusch = {};

    repository[slot.to_uint()].push_back(entry);
  }

  /// Clears the given slot of the registry.
  void clear_slot(slot_point slot) { repository[slot.to_uint()].clear(); }

  /// Returns a span that contains the PDUs for the given slot.
  span<const uplink_slot_pdu_entry> get_pdus(slot_point slot) const { return repository[slot.to_uint()]; }

private:
  /// Repository that contains the PDUs.
  circular_array<slot_entry, MAX_NUM_SLOTS> repository;
};
} // namespace srsgnb
