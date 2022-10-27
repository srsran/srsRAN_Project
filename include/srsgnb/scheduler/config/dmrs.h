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

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/dmrs.h"

namespace srsgnb {

/// Used to configure downlink demodulation reference signals for PDSCH.
/// \remark See TS 38.331, DMRS-DownlinkConfig.
struct dmrs_downlink_config {
  bool operator==(const dmrs_downlink_config& rhs) const
  {
    return type == rhs.type && additional_positions == rhs.additional_positions && max_length == rhs.max_length &&
           scrambling_id0 == rhs.scrambling_id0 && scrambling_id1 == rhs.scrambling_id1;
  }
  bool operator!=(const dmrs_downlink_config& rhs) const { return !(rhs == *this); }

  /// Selection of the DMRS type to be used for DL (see TS 38.211, clause 7.4.1.1.1). If the field is absent, the
  /// UE uses DMRS type 1.
  optional<dmrs_config_type> type;
  /// Position for additional DM-RS in DL, see Tables 7.4.1.1.2-3 and 7.4.1.1.2-4 in TS 38.211. If the field is absent,
  /// the UE applies the value pos2.
  optional<dmrs_additional_positions> additional_positions;
  /// The maximum number of OFDM symbols for DL front loaded DMRS. If the field is absent, the UE applies value len1. If
  /// set to len2, the UE determines the actual number of DM-RS symbols by the associated DCI.
  optional<dmrs_max_length> max_length;
  /// \brief DL DMRS scrambling initialization (see TS 38.211, clause 7.4.1.1.1).
  /// When the field is absent the UE applies the value Physical cell ID (physCellId) configured for this serving cell.
  optional<uint16_t> scrambling_id0;
  optional<uint16_t> scrambling_id1;
  // TODO: Remaining
};

/// \brief Data type used to represent a DMRS symbol mask for PDSCH and PUSCH transmissions.
///
/// Each bit set in the bitset represents a symbol within the slot. The first bit corresponds to the first symbol.
using dmrs_symbol_mask = bounded_bitset<14>;

} // namespace srsgnb
