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

#include "pucch_guardbands.h"
#include "pucch_default_resource.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/ran/resource_allocation/rb_interval.h"

using namespace srsran;

crb_bitmap srsran::compute_pucch_crbs(const cell_configuration& cell_cfg)
{
  // Get the parameter N_bwp_size, which is the Initial UL BWP size in PRBs, as per TS 38.213, Section 9.2.1.
  const unsigned size_ul_bwp = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();

  crb_bitmap pucch_crbs(size_ul_bwp);

  // Get PUCCH common resource config from Table 9.2.1-1, TS 38.213.
  pucch_default_resource common_default_res = get_pucch_default_resource(
      cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common, size_ul_bwp);

  // Fill the CRB bitmap with the PRBs used by the common PUCCH resources.
  for (unsigned r_pucch = 0; r_pucch != pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES; ++r_pucch) {
    auto prbs = get_pucch_default_prb_index(
        r_pucch, common_default_res.rb_bwp_offset, common_default_res.cs_indexes.size(), size_ul_bwp);

    pucch_crbs.fill(prbs.first, prbs.first + pucch_constants::FORMAT0_1_4_MAX_NPRB);
    pucch_crbs.fill(prbs.second, prbs.second + pucch_constants::FORMAT0_1_4_MAX_NPRB);
  }

  // Fill the CRB bitmap with the PRBs used by the dedicated PUCCH resources.
  for (const auto& res : cell_cfg.ded_pucch_resources) {
    unsigned nof_rbs = 1;
    if (const auto* params = std::get_if<pucch_format_2_3_cfg>(&res.format_params)) {
      nof_rbs = params->nof_prbs;
    }

    const prb_interval prbs1 = prb_interval::start_and_len(res.starting_prb, nof_rbs);
    const crb_interval crbs1 = prb_to_crb(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs, prbs1);
    pucch_crbs.fill(crbs1.start(), crbs1.stop());

    if (res.second_hop_prb.has_value()) {
      const prb_interval prbs2 = prb_interval::start_and_len(res.second_hop_prb.value(), nof_rbs);
      const crb_interval crbs2 = prb_to_crb(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs, prbs2);
      pucch_crbs.fill(crbs2.start(), crbs2.stop());
    }
  }

  return pucch_crbs;
}
