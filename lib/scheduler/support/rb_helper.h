/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/ran/pdcch/search_space.h"
#include "srsran/ran/rnti.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/scheduler_dci.h"
#include "srsran/scheduler/vrb_alloc.h"
#include "srsran/support/error_handling.h"

namespace srsran {
namespace rb_helper {

/// \brief Conversion of CRBs to VRBs as per TS38.211, clause 7.3.1.6.
/// \param crbs CRB interval to be converted to VRB interval.
/// \param bwp_crb_start Start of the BWP CRBs.
/// \param coreset_crb_start Start of the coreset CRBs.
/// \param dci_fmt DCI DL format.
/// \param ss_type type of Search Space.
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

/// \brief Finds the next contiguous range of PRBs whose respective bit in provided RB bitmap is set to zero.
///
/// \param used_rb_bitmap Bitmap of RBs, where 1's represent used RBs and 0's empty RBs.
/// \param start_crb_idx Minimum RB index from where the search is initiated.
/// \param last_crb_idx Maximum RB index (excluding) that limits the range of RBs where the search is carried out.
/// \return An interval of contiguous RBs where the respective bits are set to zero. If no interval was found, an empty
///         interval is returned.
crb_interval find_next_empty_interval(const prb_bitmap& used_rb_bitmap,
                                      size_t            start_crb_idx = 0,
                                      size_t            last_crb_idx  = MAX_NOF_PRBS);

/// \brief Finds a range of contiguous RBs, whose value in the provided RB bitmap is set to zero. The returned range
/// length should be at most "nof_rbs" RBs. If no range with length "nof_rbs" is found, the longest valid range of
/// RBs set to zero in "used_rb_bitmap" is returned.
///
/// \param used_rb_bitmap Bitmap of RBs, where 1's represent used RBs and 0's empty RBs.
/// \param nof_rbs Maximum range of RBs
/// \param start_crb_idx Minimum RB index from where the search is initiated.
/// \return An interval of RBs with maximum length equal to "nof_rbs".
crb_interval
find_empty_interval_of_length(const prb_bitmap& used_rb_bitmap, size_t nof_rbs, uint32_t start_crb_idx = 0);

} // namespace rb_helper
} // namespace srsran
