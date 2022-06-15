/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/ran/resource_allocation/resource_block_group.h"

using namespace srsgnb;

nominal_rbg_size srsgnb::get_nominal_rbg_size(unsigned bwp_nof_prb, bool config_1_or_2)
{
  srsran_assert(bwp_nof_prb > 0 and bwp_nof_prb <= 275, "Invalid BWP size");
  if (bwp_nof_prb <= 36) {
    return config_1_or_2 ? P2 : P4;
  }
  if (bwp_nof_prb <= 72) {
    return config_1_or_2 ? P4 : P8;
  }
  if (bwp_nof_prb <= 144) {
    return config_1_or_2 ? P8 : P16;
  }
  return P16;
}

unsigned srsgnb::get_nof_rbgs(crb_interval bwp_rb_dims, nominal_rbg_size P)
{
  return divide_ceil(bwp_rb_dims.length() + (bwp_rb_dims.start() % P), P);
}

unsigned srsgnb::get_nof_rbgs(crb_interval bwp_rbs, bool config1_or_2)
{
  nominal_rbg_size P = get_nominal_rbg_size(bwp_rbs.length(), config1_or_2);
  return get_nof_rbgs(bwp_rbs, P);
}

unsigned srsgnb::get_rbg_size(crb_interval bwp_rbs, nominal_rbg_size P, uint32_t rbg_idx)
{
  uint32_t nof_rbgs = get_nof_rbgs(bwp_rbs, P);
  srsran_sanity_check(rbg_idx < nof_rbgs, "RBG index out-of-bounds ({} >= {})", rbg_idx, nof_rbgs);
  if (rbg_idx == 0) {
    return P - (bwp_rbs.start() % P);
  }
  if (rbg_idx == nof_rbgs - 1) {
    uint32_t ret = (bwp_rbs.stop()) % P;
    return ret > 0 ? ret : P;
  }
  return P;
}
