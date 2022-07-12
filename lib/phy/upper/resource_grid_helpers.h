/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_PHY_UPPER_RESOURCE_GRID_HELPERS_H
#define SRSGNB_LIB_PHY_UPPER_RESOURCE_GRID_HELPERS_H

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/constants.h"

namespace srsgnb {

/// \brief Helper function to calculate the subcarrier index reference for a resource grid allocation mask.
///
/// \return The lowest subcarrier index of the lowest selected PRB.
inline unsigned get_rg_subc_mask_reference(const bounded_bitset<MAX_RB>& prb_mask)
{
  unsigned prb_index_begin;
  {
    int ret = prb_mask.find_lowest();
    srsran_assert(ret != -1, "No RB found to transmit");
    prb_index_begin = static_cast<unsigned>(ret);
  }

  // Initial subcarrier index.
  return prb_index_begin * NRE;
}

/// \brief Helper function to create an allocation mask from a PRB mask and a RE mask.
///
/// Creates an allocation where PRB are selected from \c prb_mask and the RE within the PRB are selected using \c
/// prb_mask_re.
///
/// The resultant mask is relative to to the lowest subcarrier of the lowest selected PRB. In other words, the first
/// element of the resultant mask coincides with the lowest subcarrier of the first selected PRB in \c prb_mask.
///
/// \param[in] prb_mask    Selected PRB for the transmission.
/// \param[in] prb_mask_re DMRS pattern within a PRB.
/// \return The resultant mask.
/// \sa get_rg_subc_mask_reference
inline static_vector<bool, MAX_RB * NRE> get_rg_subc_mask(const bounded_bitset<MAX_RB>& prb_mask,
                                                          const std::array<bool, NRE>&  prb_mask_re)
{
  unsigned prb_index_begin;
  {
    int ret = prb_mask.find_lowest();
    srsran_assert(ret != -1, "No RB found to transmit");
    prb_index_begin = static_cast<unsigned>(ret);
  }

  unsigned prb_index_end;
  {
    int ret = prb_mask.find_highest();
    srsran_assert(ret != -1, "No RB found to transmit");
    prb_index_end = static_cast<unsigned>(ret + 1);
  }

  // Generate empty allocation mask for the all symbols. Relative to the first subcarrier used for this transmission.
  static_vector<bool, MAX_RB * NRE> mask((prb_index_end - prb_index_begin) * NRE);

  for (unsigned prb_index = prb_index_begin, subc_index = 0; prb_index != prb_index_end;
       ++prb_index, subc_index += NRE) {
    // Select RE mask for the given RB index.
    span<bool> re_mask_rb = span<bool>(mask).subspan(subc_index, NRE);

    // Skip if the RB is not used.
    if (!prb_mask.test(prb_index)) {
      continue;
    }

    // Otherwise, copy RE mask into the RB.
    std::copy(prb_mask_re.begin(), prb_mask_re.end(), re_mask_rb.begin());
  }

  return mask;
}

} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_RESOURCE_GRID_HELPERS_H
