/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CORESET_H
#define SRSGNB_CORESET_H

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/pdcch/pdcch_constants.h"

namespace srsgnb {

/// Max number of CORESETs per BWP per cell (including UE-specific and common CORESETs).
/// \remark See TS 38.331, "PDCCH-Config".
const size_t MAX_NOF_CORESETS_PER_BWP = 3;

/// Describes a frequency resource bitmap for CORESET allocation. Each resource represents to a group of 6 PRBs.
/// \remark See TS 38.331, "ControlResourceSet"
using freq_resource_bitmap = bounded_bitset<pdcch_constants::MAX_NOF_FREQ_RESOUCES>;

/// \brief CORESET identifier. This value is UE-specific, which means that a UE can have up to
/// "maxNrofControlResourceSets" CORESETS configured.
/// \remark See TS 38.331, "maxNrofControlResourceSets".
enum coreset_id : uint8_t { MIN_CORESET_ID = 0, MAX_CORESET_ID = 11, MAX_NOF_CORESETS = 12 };

constexpr inline coreset_id to_coreset_id(unsigned cs_id)
{
  return static_cast<coreset_id>(cs_id);
}

/// CORESET configuration.
/// \remark See TS 38.331, "ControlResourceSet".
struct coreset_configuration {
  struct interleaved_mapping_type {
    /// Values: (2, 3, 6).
    unsigned reg_bundle_sz;
    /// Values: (2, 3, 6).
    unsigned interleaver_sz;
    /// Values: (0..MAX_NOF_PRBS-1).
    optional<unsigned> shift_index;
  };
  enum class precoder_granularity_type { same_as_reg_bundle, all_contiguous_rbs };

  coreset_id           id;
  freq_resource_bitmap freq_domain_resources;
  /// Duration in number of symbols. Values: (1..maxCORESETDuration).
  unsigned                           duration;
  optional<interleaved_mapping_type> interleaved;
  precoder_granularity_type          precoder_granurality;
  /// Integer offset in RBs from the pointA to the lowest RB of CORESET#0. Only used when id == 0.
  unsigned coreset0_rb_start;
};

} // namespace srsgnb

#endif // SRSGNB_CORESET_H
