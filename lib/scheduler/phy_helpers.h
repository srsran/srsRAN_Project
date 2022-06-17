/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_HELPERS_H
#define SRSGNB_PHY_HELPERS_H

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/scheduler/bwp_configuration.h"
#include "srsgnb/scheduler/scheduler_dci.h"

namespace srsgnb {

constexpr size_t SEARCH_SPACE_MAX_NOF_CANDIDATES = 8;
constexpr size_t NOF_REGS_PER_CCE                = 6;

/// List of CCE positions for a PDCCH.
using pdcch_cce_candidate_list = static_vector<unsigned, NOF_REGS_PER_CCE * 16>;

/// Position of a PDCCH REG (1 symbol x 1 RB).
struct pdcch_reg_position {
  uint8_t prb;
  uint8_t symbol;
};

using pdcch_reg_position_list = static_vector<pdcch_reg_position, NOF_REGS_PER_CCE * 16>;

/// Calculates CCE positions given a SearchSpace and Coreset configuration.
/// \return List of CCE candidates.
pdcch_cce_candidate_list get_pdcch_locations_coreset(const coreset_configuration&      cs_cfg,
                                                     const search_space_configuration& ss_cfg,
                                                     rnti_t                            rnti,
                                                     aggregation_level                 aggr_lvl,
                                                     unsigned                          slot_index);

/// Calculates the REGs positions of a given PDCCH.
/// \return List of REG positions.
pdcch_reg_position_list get_pdcch_regs(const coreset_configuration&      cs_cfg,
                                       const search_space_configuration& ss_cfg,
                                       unsigned                          cce_position,
                                       aggregation_level                 aggr_lvl);

} // namespace srsgnb

#endif // SRSGNB_PHY_HELPERS_H
