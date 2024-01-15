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

#include "srsran/scheduler/vrb_alloc.h"
#include "srsran/scheduler/resource_block_group.h"

using namespace srsran;

prb_bitmap srsran::convert_rbgs_to_prbs(const rbg_bitmap& rbgs, crb_interval bwp_rbs, nominal_rbg_size P)
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
