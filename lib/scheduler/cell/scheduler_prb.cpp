/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "scheduler_prb.h"
#include "srsran/scheduler/resource_block_group.h"

using namespace srsran;

bwp_rb_bitmap::bwp_rb_bitmap(uint32_t bwp_nof_prbs, uint32_t bwp_prb_start_, bool config1_or_2) :
  prbs_(bwp_nof_prbs),
  rbgs_(get_nof_rbgs(crb_interval(bwp_prb_start_, bwp_prb_start_ + bwp_nof_prbs), config1_or_2)),
  P_(get_nominal_rbg_size(bwp_nof_prbs, config1_or_2)),
  Pnofbits(log2(to_nominal_rbg_size_value(P_))),
  first_rbg_size(get_rbg_size(crb_interval(bwp_prb_start_, bwp_prb_start_ + bwp_nof_prbs), P_, 0))
{
}

uint32_t bwp_rb_bitmap::prb_to_rbg_idx(uint32_t prb_idx) const
{
  return ((prb_idx + to_nominal_rbg_size_value(P_) - first_rbg_size) >> Pnofbits);
}

void bwp_rb_bitmap::add_prbs_to_rbgs(const prb_bitmap& grant)
{
  int idx = 0;
  do {
    idx = grant.find_lowest(idx, grant.size(), true);
    if (idx < 0) {
      return;
    }
    uint32_t rbg_idx = prb_to_rbg_idx(idx);
    rbgs_.set(rbg_idx, true);
    idx++;
  } while (idx != (int)prbs_.size());
}

void bwp_rb_bitmap::add_prbs_to_rbgs(const prb_interval& grant)
{
  uint32_t rbg_start = prb_to_rbg_idx(grant.start());
  uint32_t rbg_stop  = std::min(prb_to_rbg_idx(grant.stop() - 1) + 1u, (uint32_t)rbgs_.size());
  rbgs_.fill(rbg_start, rbg_stop);
}

void bwp_rb_bitmap::add_rbgs_to_prbs(const rbg_bitmap& grant)
{
  int idx = 0;
  do {
    idx = grant.find_lowest(idx, grant.size(), true);
    if (idx < 0) {
      return;
    }
    uint32_t prb_idx = (idx - 1) * to_nominal_rbg_size_value(P_) + first_rbg_size;
    uint32_t prb_end =
        std::min(prb_idx + ((idx == 0) ? first_rbg_size : to_nominal_rbg_size_value(P_)), (uint32_t)prbs_.size());
    prbs_.fill(prb_idx, prb_end);
    idx++;
  } while (idx != (int)prbs_.size());
}
