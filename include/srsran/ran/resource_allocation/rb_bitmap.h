/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/bounded_bitset.h"
#include "srsran/ran/resource_allocation/rb_interval.h"
#include "srsran/ran/resource_block.h"

namespace srsran {

/// \brief Common Resource Block (CRB) bitmap.
///
/// Describes a frequency allocation mask applied to the entire channel bandwidth. The lowest
/// information bit represents the lowest resource block in the channel, which contains the
/// PointA in the lowest subcarrier.
///
/// A CRB bitmap size must be lower than or equal to the channel bandwidth to avoid allocating
/// resources outside of the physical resource grid.
using crb_bitmap = bounded_bitset<MAX_NOF_PRBS, false, crb_tag>;

inline crb_bitmap& operator|=(crb_bitmap& crb_bits, const crb_interval& grant)
{
  crb_bits.fill(grant.start(), grant.stop());
  return crb_bits;
}

/// \brief Physical Resource Block (PRB) bitmap.
///
/// Describes a frequency allocation mask applied to a certain bandwidth part (BWP). The
/// size of the map must be lower than or equal to the BWP size. The lowest information bit
/// represents the lowest PRB contained within the BWP.
///
/// The conversion from a PRB bitmap to a CRB bitmap requires the BWP description.
using prb_bitmap = bounded_bitset<MAX_NOF_PRBS, false, prb_tag>;

inline prb_bitmap& operator|=(prb_bitmap& prb_bits, const prb_interval& grant)
{
  prb_bits.fill(grant.start(), grant.stop());
  return prb_bits;
}

/// \brief Virtual Resource Block (VRB) bitmap.
///
/// Describes a frequency allocation mask of scheduled VRBs, to later be mapped to Physical Resource Blocks (PRBs) using
/// VRB-to-PRB mapping rules (either interleaved or non-interleaved), as defined in TS 38.214. The lowest information
/// bit corresponds to the lowest indexed VRB defined in the mapping, i.e., VRB 0.
///
/// A VRB bitmap size must be lower than or equal to the BWP where the mapping is defined.
using vrb_bitmap = bounded_bitset<MAX_NOF_PRBS, false, vrb_tag>;

inline vrb_bitmap& operator|=(vrb_bitmap& vrb_bits, const vrb_interval& grant)
{
  vrb_bits.fill(grant.start(), grant.stop());
  return vrb_bits;
}

inline crb_bitmap prb_to_crb(const crb_interval bwp_crb_lims, const prb_bitmap& prb_bits)
{
  // TODO.
  return prb_bits.convert_to<crb_bitmap>();
}

} // namespace srsran
