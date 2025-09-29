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
#include "srsran/ran/pdcch/dci_format.h"
#include "srsran/ran/resource_allocation/rb_interval.h"
#include "srsran/ran/resource_block.h"

namespace srsran::rb_helper {

/// \brief Conversion of CRBs to VRBs as per TS38.211, clause 7.3.1.6.
/// \param crbs CRB interval to be converted to VRB interval.
/// \param bwp_crb_start Start of the BWP CRBs.
/// \param coreset_crb_start Start of the coreset CRBs.
/// \param dci_fmt DCI DL format.
/// \param is_common_ss True if the SearchSpace is a Common SearchSpace(CSS), false otherwise.
/// \return VRB interval.
inline vrb_interval crb_to_vrb_dl_non_interleaved(crb_interval  crbs,
                                                  unsigned      bwp_crb_start,
                                                  unsigned      coreset_crb_start,
                                                  dci_dl_format dci_fmt,
                                                  bool          is_common_ss)
{
  if (dci_fmt == dci_dl_format::f1_0 and is_common_ss) {
    return crb_to_vrb_f1_0_common_ss_non_interleaved(crbs, coreset_crb_start);
  }
  return vrb_interval{crbs.start() - bwp_crb_start, crbs.stop() - bwp_crb_start};
}

/// \brief Conversion of CRBs to VRBs as per TS38.211, clause 7.3.1.6.
/// \param crbs CRB interval to be converted to VRB interval.
/// \param bwp_crb_start Start of the BWP CRBs.
/// \return VRB interval.
inline vrb_interval crb_to_vrb_ul_non_interleaved(crb_interval crbs, unsigned bwp_crb_start)
{
  srsran_sanity_check(crbs.start() >= bwp_crb_start, "Invalid CRB start");
  return vrb_interval{crbs.start() - bwp_crb_start, crbs.stop() - bwp_crb_start};
}

inline crb_interval vrb_to_crb_ul_non_interleaved(vrb_interval vrbs, unsigned bwp_crb_start)
{
  return crb_interval{vrbs.start() + bwp_crb_start, vrbs.stop() + bwp_crb_start};
}

namespace detail {

/// \brief Finds the next contiguous range of RBs whose respective bit in provided RB bitmap is set to zero.
///
/// \param used_rb_bitmap Bitmap of RBs, where 1's represent used RBs and 0's empty RBs.
/// \param search_limits Minimum and maximum RB indices where the search is carried out.
/// \return An interval of contiguous RBs where the respective bits are set to zero. If no interval was found, an empty
///         interval is returned.
/// Note: This function should not be used directly. Use \c find_empty_interval_of_length instead.
template <typename Tag>
interval<unsigned, false, Tag> find_next_empty_interval(const bounded_bitset<MAX_NOF_PRBS, false, Tag>& used_rb_bitmap,
                                                        interval<unsigned, false, Tag> search_limits = {0,
                                                                                                        MAX_NOF_PRBS})
{
  // Restrict the search to the bitmap dimensions.
  const interval<unsigned, false, Tag> bitset_limits = {0, used_rb_bitmap.size()};
  search_limits.intersect(bitset_limits);

  int rb_start = used_rb_bitmap.find_lowest(search_limits.start(), search_limits.stop(), false);
  if (rb_start >= 0) {
    int rb_end = used_rb_bitmap.find_lowest(rb_start + 1, search_limits.stop(), true);
    return {static_cast<unsigned>(rb_start), rb_end >= 0 ? static_cast<unsigned>(rb_end) : search_limits.stop()};
  }
  return {};
}

} // namespace detail

/// \brief Finds a range of contiguous RBs, whose value in the provided RB bitmap is set to zero. The returned range
/// length should be at most "nof_rbs" RBs. If no range with length "nof_rbs" is found, the longest valid range of
/// RBs set to zero in "used_rb_bitmap" is returned.
///
/// \param used_rb_bitmap Bitmap of RBs, where 1's represent used RBs and 0's empty RBs.
/// \param nof_rbs Maximum range of RBs.
/// \param search_limits Minimum and maximum RB indices where the search is carried out.
/// \return An interval of RBs with maximum length equal to "nof_rbs".
template <typename Tag>
interval<unsigned, false, Tag>
find_empty_interval_of_length(const bounded_bitset<MAX_NOF_PRBS, false, Tag>& used_rb_bitmap,
                              unsigned                                        nof_rbs,
                              interval<unsigned, false, Tag>                  search_limits = {0, MAX_NOF_PRBS})
{
  // Restrict the search to the bitmap dimensions.
  const interval<unsigned, false, Tag> bitset_limits = {0, used_rb_bitmap.size()};
  search_limits.intersect(bitset_limits);

  interval<unsigned, false, Tag> max_interv;
  do {
    interval<unsigned, false, Tag> interv = detail::find_next_empty_interval(used_rb_bitmap, search_limits);
    if (interv.empty()) {
      break;
    }
    if (interv.length() >= nof_rbs) {
      max_interv.set(interv.start(), interv.start() + nof_rbs);
      break;
    }
    if (interv.length() > max_interv.length()) {
      max_interv = interv;
    }
    search_limits.displace_to(interv.stop() + 1);
  } while (not search_limits.empty());
  return max_interv;
}

} // namespace srsran::rb_helper
