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
  /// Selection of the DMRS type to be used for DL (see TS 38.211, clause 7.4.1.1.1). If the field is absent, the
  /// UE uses DMRS type 1.
  dmrs_config_type type{dmrs_config_type::type1};
  /// Position for additional DM-RS in DL, see Tables 7.4.1.1.2-3 and 7.4.1.1.2-4 in TS 38.211. If the field is absent,
  /// the UE applies the value pos2.
  dmrs_additional_positions additional_positions{dmrs_additional_positions::pos2};
  /// The maximum number of OFDM symbols for DL front loaded DMRS. If the field is absent, the UE applies value len1.
  dmrs_max_length max_length{dmrs_max_length::len1};
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
