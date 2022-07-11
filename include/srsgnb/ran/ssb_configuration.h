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

#include "srsgnb/ran/ssb_properties.h"
#include "srsgnb/ran/subcarrier_spacing.h"

namespace srsgnb {

/// Maximum number of SS/PBCH beams in a SS/PBCH period as per TS38.213 Section 4.1.
constexpr size_t NOF_BEAMS = 64;

/// SSB Configuration.
struct ssb_configuration {
  /// SSB subcarrier spacing.
  subcarrier_spacing scs;
  /// Represents the offset to Point A in PRBs as per TS38.331 Section 6.3.2 IE offsetToPointA.
  /// Possible values: {0, ..., 2199}.
  uint16_t ssb_offset_to_point_A;
  /// SSB periodicity.
  ssb_periodicity ssb_period;
  /// SSB SubcarrierOffest or k_ssb as per TS38.211 Section 7.4.3.1. Possible values: {0, ..., 23}.
  uint8_t ssb_subcarrier_offset;
  /// Each bit in this bitmap represents if a beam is active or not as per TS38.331 Section 6.3.2 IE
  /// ssb-PositionsInBurst.
  uint64_t ssb_bitmap;
  /// The n-th element of the array indicates what Beam ID to use for the n-th SSB occasion in \c ssb_bitmap. Only
  /// relevant if the n-th bit of \c ssb_bitmap is set.
  std::array<uint8_t, NOF_BEAMS> beam_ids;
  /// PSS power level allocation in dB, relative to SSS.
  ssb_beta_pss beta_pss;
};

} // namespace srsgnb
