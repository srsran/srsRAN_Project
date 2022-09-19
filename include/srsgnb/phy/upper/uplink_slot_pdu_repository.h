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
  /// Defines the PDU type supported.
  enum class pdu_type { PUSCH, PUCCH };

  pdu_type type;
  // :TODO: convert this to variant.
  uplink_processor::pusch_pdu pusch;
  // :TOOD: add the PUCCH PDU.
};

/// Uplink slot PDU registry.
///
/// This class registers the PUSCH and PUCCH PDUs, indexing them by slot that will be processed in an uplink slot.
class uplink_slot_pdu_repository
{
  /// Maximum number of PDUs per slot.
  /// Note: Worst case is 65535.
  static constexpr unsigned MAX_NUM_PDUS = 1024;

  /// Maximum number of slots that the registry supports. In worst case (scs 240kHz) this number should be able to store
  /// 20ms.
  static constexpr unsigned MAX_NUM_SLOTS = 320;

  using slot_entry = static_vector<uplink_slot_pdu_entry, MAX_NUM_PDUS>;

public:
  /// Adds the given PUSCH PDU to the repository at the given slot.
  void add_pusch_pdu(slot_point slot, const uplink_processor::pusch_pdu& pdu)
  {
    uplink_slot_pdu_entry entry;
    entry.type  = uplink_slot_pdu_entry::pdu_type::PUSCH;
    entry.pusch = pdu;

    registry[slot.to_uint()].push_back(entry);
  }

  // :TODO: extend with PUCCH.

  /// Clears the given slot of the registry.
  void clear_slot(slot_point slot) { registry[slot.to_uint()].clear(); }

  /// Returns a span that contains the PDUs for the given slot.
  span<const uplink_slot_pdu_entry> get_pdus(slot_point slot) const { return registry[slot.to_uint()]; }

private:
  circular_array<slot_entry, MAX_NUM_SLOTS> registry;
};
} // namespace srsgnb
