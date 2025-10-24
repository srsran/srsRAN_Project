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

#include "mac_rach_handler.h"
#include "../rnti_manager.h"
#include "srsran/scheduler/scheduler_configurator.h"

using namespace srsran;

mac_cell_rach_handler_impl::mac_cell_rach_handler_impl(mac_rach_handler&                               parent_,
                                                       const sched_cell_configuration_request_message& sched_cfg) :
  parent(parent_),
  cell_index(sched_cfg.cell_index),
  nof_cb_preambles(sched_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->nof_cb_preambles_per_ssb),
  // For now, we assume all the preambles not used by CBRA are used for CFRA.
  preambles(sched_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->total_nof_ra_preambles - nof_cb_preambles)
{
  for (auto& preamble : preambles) {
    preamble.store(rnti_t::INVALID_RNTI, std::memory_order_relaxed);
  }
}

bool mac_cell_rach_handler_impl::is_cfra_preamble(unsigned ra_preamble_id) const
{
  // For now, we use all the preambles not used by CBRA.
  return ra_preamble_id < (preambles.size() + nof_cb_preambles) and ra_preamble_id >= nof_cb_preambles;
}

unsigned mac_cell_rach_handler_impl::get_cfra_index(unsigned ra_preamble_id) const
{
  srsran_assert(is_cfra_preamble(ra_preamble_id), "Invalid CFRA preamble");
  return ra_preamble_id - nof_cb_preambles;
}

void mac_cell_rach_handler_impl::handle_rach_indication(const mac_rach_indication& rach_ind)
{
  // Create Scheduler RACH indication message. Allocate TC-RNTIs in the process.
  rach_indication_message sched_rach{};
  sched_rach.cell_index = cell_index;
  sched_rach.slot_rx    = rach_ind.slot_rx;
  for (const auto& occasion : rach_ind.occasions) {
    auto& sched_occasion           = sched_rach.occasions.emplace_back();
    sched_occasion.start_symbol    = occasion.start_symbol;
    sched_occasion.frequency_index = occasion.frequency_index;
    for (const auto& preamble : occasion.preambles) {
      rnti_t selected_rnti = rnti_t::INVALID_RNTI;
      if (is_cfra_preamble(preamble.index)) {
        // Fetch C-RNTI if it is Contention-free RACH preamble.
        selected_rnti = preambles[get_cfra_index(preamble.index)].load(std::memory_order_acquire);
        if (selected_rnti == rnti_t::INVALID_RNTI) {
          parent.logger.info("cell={} preamble id={}: Ignoring detected contention-free PRACH preamble. Cause: No "
                             "C-RNTI was allocated this preamble.",
                             fmt::underlying(cell_index),
                             preamble.index);
          continue;
        }
      } else {
        // It is a Contention-based RACH preamble. Allocate TC-RNTI for the UE.
        selected_rnti = parent.rnti_mng.allocate();
        if (selected_rnti == rnti_t::INVALID_RNTI) {
          parent.logger.warning("cell={} preamble id={}: Ignoring PRACH. Cause: Failed to allocate TC-RNTI.",
                                fmt::underlying(cell_index),
                                preamble.index);
          continue;
        }
      }
      auto& sched_preamble        = sched_occasion.preambles.emplace_back();
      sched_preamble.preamble_id  = preamble.index;
      sched_preamble.tc_rnti      = selected_rnti;
      sched_preamble.time_advance = preamble.time_advance;
    }
    if (sched_occasion.preambles.empty()) {
      // No preamble was added. Remove occasion.
      sched_rach.occasions.pop_back();
    }
  }

  // Forward RACH indication to scheduler.
  if (not sched_rach.occasions.empty()) {
    parent.sched.handle_rach_indication(sched_rach);
  }
}

bool mac_cell_rach_handler_impl::handle_cfra_allocation(uint8_t preamble_id, du_ue_index_t ue_idx, rnti_t crnti)
{
  srsran_assert(is_cfra_preamble(preamble_id), "Invalid preamble_id={}", preamble_id);
  if (parent.ue_map[ue_idx].cell_idx != INVALID_DU_CELL_INDEX or
      parent.ue_map[ue_idx].preamble_id != MAX_NOF_RA_PREAMBLES_PER_OCCASION) {
    return false;
  }
  const unsigned idx           = get_cfra_index(preamble_id);
  rnti_t         expected_rnti = rnti_t::INVALID_RNTI;
  if (preambles[idx].compare_exchange_strong(expected_rnti, crnti, std::memory_order_acq_rel)) {
    parent.ue_map[ue_idx].cell_idx    = cell_index;
    parent.ue_map[ue_idx].preamble_id = preamble_id;
    return true;
  }
  return false;
}

void mac_cell_rach_handler_impl::handle_cfra_deallocation(du_ue_index_t ue_idx)
{
  auto&         ue_entry    = parent.ue_map[ue_idx];
  const uint8_t preamble_id = ue_entry.preamble_id;
  if (preamble_id != MAX_NOF_RA_PREAMBLES_PER_OCCASION) {
    ue_entry.cell_idx    = INVALID_DU_CELL_INDEX;
    ue_entry.preamble_id = MAX_NOF_RA_PREAMBLES_PER_OCCASION;
    preambles[get_cfra_index(preamble_id)].store(rnti_t::INVALID_RNTI, std::memory_order_release);
  }
}

mac_rach_handler::mac_rach_handler(scheduler_configurator& sched_,
                                   rnti_manager&           rnti_mng_,
                                   srslog::basic_logger&   logger_) :
  sched(sched_),
  rnti_mng(rnti_mng_),
  logger(logger_),
  ue_map(MAX_NOF_DU_UES, cfra_ue_context{INVALID_DU_CELL_INDEX, MAX_NOF_RA_PREAMBLES_PER_OCCASION})
{
}

mac_cell_rach_handler_impl& mac_rach_handler::add_cell(const sched_cell_configuration_request_message& sched_cfg)
{
  srsran_assert(not cell_map.contains(sched_cfg.cell_index), "Cell already exists");
  cell_map.emplace(sched_cfg.cell_index, std::make_unique<mac_cell_rach_handler_impl>(*this, sched_cfg));
  return *cell_map[sched_cfg.cell_index];
}

void mac_rach_handler::rem_cell(du_cell_index_t cell_index)
{
  srsran_assert(cell_map.contains(cell_index), "Cell does not exist");
  cell_map.erase(cell_index);
}
