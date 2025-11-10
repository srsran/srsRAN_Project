
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
#include "../config/ue_configuration.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/scheduler/result/pucch_info.h"
#include "srsran/scheduler/result/sched_result.h"

namespace srsran {

/// Retrieves the correct N_{ID}^0 parameter for PUCCH scrambling from the UE configuration.
unsigned get_n_id0_scrambling(const ue_cell_configuration& ue_cell_cfg, unsigned cell_pci);

/// Checks for collisions between a set of UL PUCCH grants and the existing UL allocations in the scheduler result.
bool check_ul_collisions(span<const grant_info>    grants,
                         const ul_sched_result&    result,
                         const cell_configuration& cell_cfg,
                         bool                      is_common);

/// Marks a pair of PUCCH grants in the resource grid.
void mark_pucch_in_resource_grid(cell_slot_resource_allocator&    pucch_slot_alloc,
                                 const grant_info&                first_hop_grant,
                                 const std::optional<grant_info>& second_hop_grant,
                                 const crb_interval&              ul_bwp_crbs,
                                 const scheduler_expert_config&   expert_cfg);

/// Converts a PUCCH resource configuration to grant_info structures for the first and second hop (if applicable).
std::pair<grant_info, std::optional<grant_info>> pucch_resource_to_grant_info(const bwp_configuration& init_ul_bwp,
                                                                              const pucch_resource&    pucch_res);

/// Marks a PUCCH resource in the resource grid.
void mark_pucch_in_resource_grid(cell_slot_resource_allocator& pucch_slot_alloc,
                                 const pucch_resource&         pucch_res,
                                 const ue_cell_configuration&  ue_cell_cfg);

/// Contains the existing PUCCH grants currently allocated to a given UE.
class pucch_existing_pdus_handler
{
public:
  pucch_existing_pdus_handler(rnti_t crnti, span<pucch_info> pucchs, const pucch_config& pucch_cfg);

  [[nodiscard]] bool     is_empty() const { return pdus_cnt == 0; }
  [[nodiscard]] unsigned get_nof_unallocated_pdu() const { return pdus_cnt; }
  pucch_info*            get_next_pdu(static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>& pucchs);
  void remove_unused_pdus(static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>& pucchs, rnti_t rnti) const;
  void update_sr_pdu_bits(sr_nof_bits sr_bits, unsigned harq_ack_bits);
  void update_csi_pdu_bits(unsigned csi_part1_bits, sr_nof_bits sr_bits);
  void update_harq_pdu_bits(unsigned                                       harq_ack_bits,
                            sr_nof_bits                                    sr_bits,
                            unsigned                                       csi_part1_bits,
                            const pucch_resource&                          pucch_res_cfg,
                            const std::optional<pucch_common_all_formats>& common_params);

  pucch_info* sr_pdu{nullptr};
  pucch_info* harq_pdu{nullptr};
  pucch_info* csi_pdu{nullptr};

private:
  unsigned pdus_cnt = 0;
  unsigned pdu_id   = 0;
};

} // namespace srsran
