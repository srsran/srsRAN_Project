/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_DMRS_H
#define SRSGNB_DMRS_H

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/optional.h"

namespace srsgnb {

/// \brief Position of first DM-RS for Downlink (see TS 38.211, clause 7.4.1.1.1) and Uplink (see TS 38.211,
/// clause 6.4.1.1.3).
enum class dmrs_typeA_position { pos2 = 2, pos3 = 3 };

inline unsigned to_symbol_index(dmrs_typeA_position pos)
{
  return static_cast<unsigned>(pos);
}

/// Type of DMRS to be used in DL and UL.
/// \remark See TS 38.331, DMRS-DownlinkConfig and DMRS-UplinkConfig.
enum class dmrs_type { type1 = 1, type2 };

/// \brief Position for additional DM-RS in DL (see TS 38.211, Tables 7.4.1.1.2-3 and 7.4.1.1.2-4).
/// \remark See TS 38.331, DMRS-DownlinkConfig and DMRS-UplinkConfig.
enum class dmrs_additional_positions { pos0 = 0, pos1, pos2, pos3 };

/// \brief The maximum number of OFDM symbols for DL front loaded DMRS. If set to len2, the UE determines the actual
/// number of DM-RS symbols by the associated DCI. (see TS 38.214, clause 7.4.1.1.2).
/// \remark See TS 38.214, DMRS-DownlinkConfig.
enum class dmrs_max_length { len1 = 1, len2 };

struct ptrs_downlink_config {
  // TODO
};

/// Used to configure downlink demodulation reference signals for PDSCH.
/// \remark See TS 38.331, DMRS-DownlinkConfig.
struct dmrs_downlink_config {
  dmrs_type                 type;
  dmrs_additional_positions additional_positions;
  dmrs_max_length           max_length;
  /// \brief DL DMRS scrambling initialization (see TS 38.211, clause 7.4.1.1.1).
  /// When the field is absent the UE applies the value Physical cell ID (physCellId) configured for this serving cell.
  optional<uint16_t> scrambling_id0;
  optional<uint16_t> scrambling_id1;
  /// Configures downlink PTRS. See TS 38.214, clause 5.1.6.3.
  optional<ptrs_downlink_config> phase_tracking_rs;
};

/// \brief Data type used to represent a DMRS for PDSCH symbol mask.
///
/// Each bit set in the bitset represents a symbol within the slot. The first bit corresponds to the first symbol.
using pdsch_dmrs_symbol_mask = bounded_bitset<14>;

} // namespace srsgnb

#endif // SRSGNB_DMRS_H
