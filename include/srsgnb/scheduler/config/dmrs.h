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

/// \brief Configuration used for DL phase tracking reference signal.
///
/// \remark See TS38.331 Section as 6.3.2 (see field \e PTRS-DownlinkConfig).
struct ptrs_downlink_config {
  // TODO
};

/// Used to configure downlink demodulation reference signals for PDSCH.
/// \remark See TS 38.331, DMRS-DownlinkConfig.
struct dmrs_downlink_config {
  dmrs_config_type          type;
  dmrs_additional_positions additional_positions;
  dmrs_max_length           max_length;
  /// \brief DL DMRS scrambling initialization (see TS 38.211, clause 7.4.1.1.1).
  /// When the field is absent the UE applies the value Physical cell ID (physCellId) configured for this serving cell.
  optional<uint16_t> scrambling_id0;
  optional<uint16_t> scrambling_id1;
  /// Configures downlink PTRS. See TS 38.214, clause 5.1.6.3.
  optional<ptrs_downlink_config> phase_tracking_rs;
};

/// \brief Data type used to represent a DMRS symbol mask for PDSCH and PUSCH transmissions.
///
/// Each bit set in the bitset represents a symbol within the slot. The first bit corresponds to the first symbol.
using dmrs_symbol_mask = bounded_bitset<14>;

} // namespace srsgnb
