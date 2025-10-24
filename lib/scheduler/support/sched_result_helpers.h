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

#pragma once

#include "../cell/resource_grid.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/ran/prach/prach_frequency_mapping.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/scheduler/result/pusch_info.h"
#include "srsran/scheduler/result/sched_result.h"

namespace srsran {
/// Determine the space left for a new UCI PDU of a gived C-RNTI in the scheduler result.
inline bool
has_space_for_uci_pdu(const sched_result& result, rnti_t crnti, const scheduler_ue_expert_config& expert_cfg)
{
  // Avoid slots where PUSCH has already been scheduled, as the PUSCH could be occupying the PUCCH resources for this
  // SR/CSI.
  // Note: This will implicitly avoid that there are PUSCHs and PUCCH for the same UE in the same slot.
  if (not result.ul.puschs.empty()) {
    return false;
  }

  // Check if we can fit the UCI in an existing PUCCH PDU for the same UE.
  bool has_pucch = std::any_of(result.ul.pucchs.begin(), result.ul.pucchs.end(), [crnti](const pucch_info& grant) {
    return grant.crnti == crnti;
  });
  if (has_pucch) {
    return true;
  }

  // New PUCCH PDU is required.
  // Check if max PUCCH limit is not reached.
  const unsigned max_pucchs = std::min(expert_cfg.max_pucchs_per_slot, (unsigned)result.ul.pucchs.capacity());
  unsigned       space      = max_pucchs - result.ul.pucchs.size();
  if (space == 0) {
    return false;
  }

  // Check if max UL grant limit (PUCCH + PUSCH) is not reached.
  unsigned ul_grants = result.ul.pucchs.size() + result.ul.puschs.size();
  return expert_cfg.max_ul_grants_per_slot > ul_grants;
}

/// Determine the space left for a new PUSCH PDU of a gived C-RNTI in the scheduler result.
inline unsigned
get_space_left_for_pusch_pdus(const sched_result& result, rnti_t crnti, const scheduler_ue_expert_config& expert_cfg)
{
  // Check if max PUSCH limit is not reached.
  const unsigned max_puschs = std::min(expert_cfg.max_puschs_per_slot, (unsigned)result.ul.puschs.capacity());
  unsigned       space      = max_puschs - result.ul.puschs.size();
  if (space == 0) {
    return 0;
  }

  // Check if max UL grant limit (PUCCH + PUSCH) is not reached.
  // Note: Consider that if an PUSCH grant is scheduled for this UE, its PUCCH PDUs are removed.
  unsigned nof_pucchs    = std::count_if(result.ul.pucchs.begin(),
                                      result.ul.pucchs.end(),
                                      [crnti](const pucch_info& pucch) { return pucch.crnti == crnti; });
  unsigned nof_ul_grants = result.ul.pucchs.size() + result.ul.puschs.size() - nof_pucchs;
  unsigned space2 =
      expert_cfg.max_ul_grants_per_slot >= nof_ul_grants ? expert_cfg.max_ul_grants_per_slot - nof_ul_grants : 0;

  return std::min(space, space2);
}

/// Extract PUSCH grant info of PUSCH allocation.
///
/// \param pucch PUSCH grant parameters.
/// \return Parameters of the grant.
inline grant_info get_pusch_grant_info(const ul_sched_info& pusch)
{
  const bwp_configuration& bwp_cfg = *pusch.pusch_cfg.bwp_cfg;
  prb_interval             prbs    = {pusch.pusch_cfg.rbs.type1().start(), pusch.pusch_cfg.rbs.type1().stop()};
  crb_interval             crbs    = prb_to_crb(bwp_cfg, prbs);
  return grant_info{pusch.pusch_cfg.bwp_cfg->scs, pusch.pusch_cfg.symbols, crbs};
}

/// Extract PRACH grant info of PRACH allocations.
///
/// \param cell_cfg Cell configuration.
/// \param prachs Vector of PRACH occasions information.
/// \return Parameters of the grants.
inline static_vector<grant_info, MAX_PRACH_OCCASIONS_PER_SLOT>
get_prach_grant_info(const cell_configuration&                                               cell_cfg,
                     const static_vector<prach_occasion_info, MAX_PRACH_OCCASIONS_PER_SLOT>& prachs)
{
  static_vector<grant_info, MAX_PRACH_OCCASIONS_PER_SLOT> grants;
  if (prachs.empty()) {
    return grants;
  }

  prach_configuration prach_cfg =
      prach_configuration_get(frequency_range::FR1,
                              cell_cfg.paired_spectrum ? duplex_mode::FDD : duplex_mode::TDD,
                              cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index);

  // Derive PRACH duration information.
  // The parameter \c is_last_prach_occasion is arbitrarily set to false, as it doesn't affect the PRACH number of
  // PRBs.
  constexpr bool                   is_last_prach_occasion = false;
  const prach_preamble_information info =
      is_long_preamble(prach_cfg.format)
          ? get_prach_preamble_long_info(prach_cfg.format)
          : get_prach_preamble_short_info(
                prach_cfg.format,
                to_ra_subcarrier_spacing(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs),
                is_last_prach_occasion);
  const unsigned prach_nof_prbs =
      prach_frequency_mapping_get(info.scs, cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs).nof_rb_ra;

  for (const prach_occasion_info& prach : prachs) {
    ofdm_symbol_range symbols{prach.start_symbol, static_cast<uint8_t>(prach.start_symbol + prach_cfg.duration)};
    unsigned     prb_start = cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_frequency_start;
    prb_interval prbs{prb_start, prb_start + prach_nof_prbs};
    crb_interval crbs = prb_to_crb(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params, prbs);
    grants.emplace_back(grant_info{cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs, symbols, crbs});
  }
  return grants;
}

/// Extract PUCCH grant info of PUCCH allocation.
///
/// \param pucch PUCCH grant parameters.
/// \return Parameters of the grant.
inline std::pair<grant_info, std::optional<grant_info>> get_pucch_grant_info(const pucch_info& pucch)
{
  const bwp_configuration& bwp_cfg = *pucch.bwp_cfg;
  if (not pucch.resources.second_hop_prbs.empty()) {
    // Intra-slot frequency hopping.
    ofdm_symbol_range first_hop_symbols{pucch.resources.symbols.start(),
                                        pucch.resources.symbols.start() + pucch.resources.symbols.length() / 2};
    ofdm_symbol_range second_hop_symbols{pucch.resources.symbols.start() + pucch.resources.symbols.length() / 2,
                                         pucch.resources.symbols.stop()};

    unsigned crb_first_hop  = prb_to_crb(bwp_cfg, pucch.resources.prbs.start());
    unsigned crb_second_hop = prb_to_crb(bwp_cfg, pucch.resources.second_hop_prbs.start());
    return {grant_info{bwp_cfg.scs, first_hop_symbols, crb_interval{crb_first_hop, crb_first_hop + 1}},
            grant_info{bwp_cfg.scs, second_hop_symbols, crb_interval{crb_second_hop, crb_second_hop + 1}}};
  }
  // No frequency hopping.
  unsigned crb_first_hop = prb_to_crb(bwp_cfg, pucch.resources.prbs.start());
  return {grant_info{bwp_cfg.scs, pucch.resources.symbols, crb_interval{crb_first_hop, crb_first_hop + 1}},
          std::nullopt};
}

} // namespace srsran
