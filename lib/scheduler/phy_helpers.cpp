/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "phy_helpers.h"

using namespace srsgnb;

pdcch_cce_candidate_list srsgnb::get_pdcch_locations_coreset(const coreset_configuration&      cs_cfg,
                                                             const search_space_configuration& ss_cfg,
                                                             rnti_t                            rnti,
                                                             aggregation_level                 aggr_lvl,
                                                             unsigned                          slot_index)
{
  // TODO: PHY helper.
  pdcch_cce_candidate_list cces;
  cces.push_back(0);
  return cces;
}

pdcch_reg_position_list srsgnb::get_pdcch_regs(const coreset_configuration&      cs_cfg,
                                               const search_space_configuration& ss_cfg,
                                               unsigned                          cce_position,
                                               aggregation_level                 aggr_lvl)
{
  // TODO: PHY helper.
  pdcch_reg_position_list regs;
  regs.push_back(pdcch_reg_position{0, 0});
  return regs;
}
