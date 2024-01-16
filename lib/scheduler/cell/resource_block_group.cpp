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

#include "srsran/scheduler/resource_block_group.h"

using namespace srsran;

nominal_rbg_size srsran::get_nominal_rbg_size(unsigned bwp_nof_prb, bool is_config_1)
{
  srsran_assert(bwp_nof_prb > 0 and bwp_nof_prb <= 275, "Invalid BWP size");
  if (bwp_nof_prb <= 36) {
    return is_config_1 ? nominal_rbg_size::P2 : nominal_rbg_size::P4;
  }
  if (bwp_nof_prb <= 72) {
    return is_config_1 ? nominal_rbg_size::P4 : nominal_rbg_size::P8;
  }
  if (bwp_nof_prb <= 144) {
    return is_config_1 ? nominal_rbg_size::P8 : nominal_rbg_size::P16;
  }
  return nominal_rbg_size::P16;
}

unsigned srsran::get_nof_rbgs(crb_interval bwp_rb_dims, nominal_rbg_size P)
{
  return divide_ceil(bwp_rb_dims.length() + (bwp_rb_dims.start() % to_nominal_rbg_size_value(P)),
                     to_nominal_rbg_size_value(P));
}

unsigned srsran::get_nof_rbgs(crb_interval bwp_rbs, bool config1_or_2)
{
  nominal_rbg_size P = get_nominal_rbg_size(bwp_rbs.length(), config1_or_2);
  return get_nof_rbgs(bwp_rbs, P);
}

unsigned srsran::get_rbg_size(crb_interval bwp_rbs, nominal_rbg_size P, uint32_t rbg_idx)
{
  uint32_t nof_rbgs = get_nof_rbgs(bwp_rbs, P);
  srsran_sanity_check(rbg_idx < nof_rbgs, "RBG index out-of-bounds ({} >= {})", rbg_idx, nof_rbgs);
  if (rbg_idx == 0) {
    return to_nominal_rbg_size_value(P) - (bwp_rbs.start() % to_nominal_rbg_size_value(P));
  }
  if (rbg_idx == nof_rbgs - 1) {
    uint32_t ret = (bwp_rbs.stop()) % to_nominal_rbg_size_value(P);
    return ret > 0 ? ret : to_nominal_rbg_size_value(P);
  }
  return to_nominal_rbg_size_value(P);
}
