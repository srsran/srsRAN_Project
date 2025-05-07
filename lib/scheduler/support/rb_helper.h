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

#include "srsran/ran/pdcch/dci_format.h"
#include "srsran/scheduler/result/vrb_alloc.h"

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

/// \brief Finds the next contiguous range of CRBs whose respective bit in provided CRB bitmap is set to zero.
///
/// \param used_crb_bitmap Bitmap of RBs, where 1's represent used RBs and 0's empty RBs.
/// \param search_limits Minimum and maximum RB indices where the search is carried out.
/// \return An interval of contiguous CRBs where the respective bits are set to zero. If no interval was found, an empty
///         interval is returned.
crb_interval find_next_empty_interval(const crb_bitmap& used_crb_bitmap,
                                      crb_interval      search_limits = {0, MAX_NOF_PRBS});

/// \brief Finds a range of contiguous CRBs, whose value in the provided CRB bitmap is set to zero. The returned range
/// length should be at most "nof_crbs" CRBs. If no range with length "nof_crbs" is found, the longest valid range of
/// CRBs set to zero in "used_crb_bitmap" is returned.
///
/// \param used_crb_bitmap Bitmap of RBs, where 1's represent used RBs and 0's empty RBs.
/// \param nof_crbs Maximum range of RBs.
/// \param search_limits Minimum and maximum RB indices where the search is carried out.
/// \return An interval of CRBs with maximum length equal to "nof_crbs".
crb_interval find_empty_interval_of_length(const crb_bitmap& used_crb_bitmap,
                                           unsigned          nof_crbs,
                                           crb_interval      search_limits = {0, MAX_NOF_PRBS});

/// \brief Finds the next contiguous range of PRBs whose respective bit in provided PRB bitmap is set to zero.
///
/// \param used_prb_bitmap Bitmap of RBs, where 1's represent used RBs and 0's empty RBs.
/// \param search_limits Minimum and maximum RB indices where the search is carried out.
/// \return An interval of contiguous PRBs where the respective bits are set to zero. If no interval was found, an empty
///         interval is returned.
prb_interval find_next_empty_interval(const prb_bitmap& used_prb_bitmap,
                                      prb_interval      search_limits = {0, MAX_NOF_PRBS});

/// \brief Finds a range of contiguous PRBs, whose value in the provided PRB bitmap is set to zero. The returned range
/// length should be at most "nof_prbs" PRBs. If no range with length "nof_prbs" is found, the longest valid range of
/// PRBs set to zero in "used_prb_bitmap" is returned.
///
/// \param used_prb_bitmap Bitmap of RBs, where 1's represent used RBs and 0's empty RBs.
/// \param nof_prbs Maximum range of RBs.
/// \param search_limits Minimum and maximum RB indices where the search is carried out.
/// \return An interval of PRBs with maximum length equal to "nof_prbs".
prb_interval find_empty_interval_of_length(const prb_bitmap& used_prb_bitmap,
                                           unsigned          nof_prbs,
                                           prb_interval      search_limits = {0, MAX_NOF_PRBS});

/// \brief Finds the next contiguous range of VRBs whose respective bit in provided VRB bitmap is set to zero.
///
/// \param used_vrb_bitmap Bitmap of RBs, where 1's represent used RBs and 0's empty RBs.
/// \param search_limits Minimum and maximum RB indices where the search is carried out.
/// \return An interval of contiguous VRBs where the respective bits are set to zero. If no interval was found, an empty
///         interval is returned.
vrb_interval find_next_empty_interval(const vrb_bitmap& used_vrb_bitmap,
                                      vrb_interval      search_limits = {0, MAX_NOF_PRBS});

/// \brief Finds a range of contiguous VRBs, whose value in the provided PRB bitmap is set to zero. The returned range
/// length should be at most "nof_vrbs" VRBs. If no range with length "nof_vrbs" is found, the longest valid range of
/// VRBs set to zero in "used_vrb_bitmap" is returned.
///
/// \param used_vrb_bitmap Bitmap of RBs, where 1's represent used RBs and 0's empty RBs.
/// \param nof_vrbs Maximum range of RBs.
/// \param search_limits Minimum and maximum RB indices where the search is carried out.
/// \return An interval of VRBs with maximum length equal to "nof_vrbs".
vrb_interval find_empty_interval_of_length(const vrb_bitmap& used_vrb_bitmap,
                                           unsigned          nof_vrbs,
                                           vrb_interval      search_limits = {0, MAX_NOF_PRBS});
} // namespace srsran::rb_helper
