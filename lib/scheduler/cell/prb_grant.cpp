/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/scheduler/prb_grant.h"
#include "srsgnb/scheduler/resource_block_group.h"

using namespace srsgnb;

prb_bitmap srsgnb::convert_rbgs_to_prbs(const rbg_bitmap& rbgs, crb_interval bwp_rbs, nominal_rbg_size P)
{
  int      idx            = 0;
  unsigned first_rbg_size = get_rbg_size(bwp_rbs, P, 0);

  prb_bitmap prbs(bwp_rbs.length());
  do {
    idx = rbgs.find_lowest(idx, rbgs.size(), true);
    if (idx < 0) {
      break;
    }
    uint32_t prb_idx = (idx - 1) * to_nominal_rbg_size_value(P) + first_rbg_size;
    uint32_t prb_end =
        std::min(prb_idx + ((idx == 0) ? first_rbg_size : to_nominal_rbg_size_value(P)), (uint32_t)prbs.size());
    prbs.fill(prb_idx, prb_end);
    idx++;
  } while (idx != (int)prbs.size());
  return prbs;
}
