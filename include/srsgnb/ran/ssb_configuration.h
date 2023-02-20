/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/ran/ssb_properties.h"
#include "srsgnb/ran/subcarrier_spacing.h"

namespace srsgnb {

/// SSB-Index identifies an SS-Block within an SS-Burst.
/// \remark See TS 38.331, "SSB-Index" and "maxNrofSSBs". See also, TS 38.213, clause 4.1.
enum ssb_id_t : uint8_t { MIN_SSB_ID = 0, MAX_SSB_ID = 63, MAX_NOF_SSB_RESOURCES = 64 };

/// Maximum number of SS/PBCH beams in a SS/PBCH period as per TS38.213 Section 4.1.
constexpr size_t NOF_BEAMS = 64;

/// SSB Configuration.
struct ssb_configuration {
  /// SSB subcarrier spacing.
  subcarrier_spacing scs;
  /// Represents the offset to Point A in PRBs as per TS38.331 Section 6.3.2 IE offsetToPointA.
  /// Possible values: {0, ..., 2199}.
  ssb_offset_to_pointA offset_to_point_A;
  /// SSB periodicity.
  ssb_periodicity ssb_period;
  /// k_ssb or SSB SubcarrierOffest, as per TS38.211 Section 7.4.3.1. Possible values: {0, ..., 23}.
  ssb_subcarrier_offset k_ssb;
  /// Each bit in this bitmap represents if a beam is active or not as per TS38.331 Section 6.3.2 IE
  /// ssb-PositionsInBurst.
  uint64_t ssb_bitmap;
  /// The n-th element of the array indicates what Beam ID to use for the n-th SSB occasion in \c ssb_bitmap. Only
  /// relevant if the n-th bit of \c ssb_bitmap is set.
  std::array<uint8_t, NOF_BEAMS> beam_ids;
  /// PSS EPRE to SSS EPRE for SSB. TS 38.213, Section 4.1 gives an explanation of this measure, but doesn't provide a
  /// name for this parameter. Nor is there a name in the TS 38.331.
  ssb_pss_to_sss_epre pss_to_sss_epre;
  /// Average EPRE of the resources elements that carry secondary synchronization signals in dBm, as per
  /// ServingCellConfigCommonSIB, TS 38.331. Possible values: {-60, ..., 50}
  int ssb_block_power;
};

} // namespace srsgnb
