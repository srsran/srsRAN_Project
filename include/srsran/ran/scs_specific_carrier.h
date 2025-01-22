/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/subcarrier_spacing.h"
#include <optional>

namespace srsran {

/// \brief It provides parameters determining the location and width of the actual carrier.
/// \remark See TS 38.331, "SCS-SpecificCarrier".
struct scs_specific_carrier {
  /// Offset between Point A (lowest subcarrier of common RB 0) and the lowest usable subcarrier in this carrier in
  /// number of PRBs. Values: (0..2199).
  unsigned           offset_to_carrier;
  subcarrier_spacing scs;
  /// Width of this carrier in number of PRBs. Values: (0..MAX_NOF_PRBS).
  unsigned carrier_bandwidth;
  /// Indicates the downlink Tx Direct Current location for the carrier. A value in the range 0..3299 indicates the
  /// subcarrier index within the carrier. The values in the value range 3301..4095 are reserved and ignored by the UE.
  /// If this field is absent, the UE assumes the default value of 3300 (i.e. "Outside the carrier").
  std::optional<unsigned> tx_direct_current_location;
};

} // namespace srsran
