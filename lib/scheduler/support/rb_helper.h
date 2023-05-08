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

#include "srsran/ran/pdcch/search_space.h"
#include "srsran/ran/rnti.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/vrb_alloc.h"
#include "srsran/support/error_handling.h"

namespace srsran {
namespace rb_helper {

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

/// \brief Get CRB boundaries where a RB allocation can be made given a chosen BWP, Coreset and SearchSpace.
inline constexpr crb_interval get_dl_alloc_crb_limits(const bwp_downlink_common&        init_bwp_dl,
                                                      const bwp_configuration&          active_bwp_cfg,
                                                      const coreset_configuration&      cs_cfg,
                                                      const search_space_configuration& ss_cfg)
{
  // Allocations are limited by active BWP CRB boundaries.
  crb_interval crbs = active_bwp_cfg.crbs;

  // See TS38.211, clause 7.3.1.6 - Mapping from virtual to physical resource blocks.
  if (ss_cfg.type == search_space_configuration::type_t::ue_dedicated or not ss_cfg.common.f0_0_and_f1_0) {
    return crbs;
  }
  crbs = {cs_cfg.get_coreset_start_crb(), crbs.stop()};

  // See TS38.212, clause 7.3.1.2.1. For all types of RNTI, if CORESET#0 is enabled, the allocation is limited by
  // CORESET#0 size.
  if (init_bwp_dl.pdcch_common.coreset0.has_value()) {
    crb_interval cs0_crbs = init_bwp_dl.pdcch_common.coreset0->coreset0_crbs();
    crbs                  = {crbs.start(), cs0_crbs.start() + cs0_crbs.length()};
  }
  return crbs;
}

} // namespace rb_helper
} // namespace srsran