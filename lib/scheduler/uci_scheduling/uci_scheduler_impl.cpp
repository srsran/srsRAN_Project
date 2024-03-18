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

#include "uci_scheduler_impl.h"
#include "../cell/resource_grid.h"
#include "uci_allocator_impl.h"

using namespace srsran;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uci_scheduler_impl::uci_scheduler_impl(const cell_configuration& cell_cfg_,
                                       uci_allocator&            uci_alloc_,
                                       ue_repository&            ues_) :
  cell_cfg(cell_cfg_), uci_alloc(uci_alloc_), ues(ues_), logger(srslog::fetch_basic_logger("SCHED"))
{
  // Max size of the UCI resource slot wheel, dimensioned based on the UCI periods.
  periodic_uci_slot_wheel.resize(std::max(MAX_SR_PERIOD, MAX_CSI_REPORT_PERIOD));

  // Pre-reserve space for the UEs that will be added.
  updated_ues.reserve(MAX_NOF_DU_UES);
}

uci_scheduler_impl::~uci_scheduler_impl() {}

void uci_scheduler_impl::run_slot(cell_resource_allocator& cell_alloc)
{
  // Initial allocation: we allocate opportunities all over the grid.
  schedule_updated_ues_ucis(cell_alloc);

  // Only allocate in the farthest slot in the grid, as the previous part of the allocation grid has been completed
  // at the first this function was called.
  schedule_slot_ucis(cell_alloc[cell_alloc.max_ul_slot_alloc_delay]);
}

void uci_scheduler_impl::add_resource(rnti_t crnti, unsigned res_offset, unsigned res_period, bool is_sr)
{
  // For each offset in the periodic UCI slot wheel.
  for (unsigned wheel_offset = res_offset; wheel_offset < periodic_uci_slot_wheel.size(); wheel_offset += res_period) {
    auto& slot_wheel = periodic_uci_slot_wheel[wheel_offset];

    // Check if the UE is already in the slot wheel.
    auto it = std::find_if(slot_wheel.begin(), slot_wheel.end(), [crnti](const auto& r) { return r.rnti == crnti; });

    if (it == slot_wheel.end()) {
      // New UE. Create a new element in the list with either SR or CSI resource.
      slot_wheel.push_back(periodic_uci_info{crnti, is_sr ? 1U : 0U, is_sr ? 0U : 1U});
    } else {
      // Resource for UE already exists. Increment the resource counter.
      if (is_sr) {
        it->sr_counter++;
      } else {
        it->csi_counter++;
      }
    }
  }
}

void uci_scheduler_impl::rem_resource(rnti_t crnti, unsigned res_offset, unsigned res_period, bool is_sr)
{
  auto log_error = [&]() {
    logger.error("cell={} c-rnti={}: Unable to remove {} PUCCH resource for period={} offset={}",
                 cell_cfg.cell_index,
                 crnti,
                 is_sr ? "SR" : "CSI",
                 res_period,
                 res_offset);
  };

  for (unsigned wheel_offset = res_offset; wheel_offset < periodic_uci_slot_wheel.size(); wheel_offset += res_period) {
    auto& slot_wheel = periodic_uci_slot_wheel[wheel_offset];

    auto it = std::find_if(slot_wheel.begin(), slot_wheel.end(), [crnti](const auto& r) { return r.rnti == crnti; });
    if (it != slot_wheel.end()) {
      if (is_sr) {
        if (it->sr_counter == 0) {
          log_error();
          continue;
        }
        it->sr_counter--;
      } else {
        if (it->csi_counter == 0) {
          log_error();
          continue;
        }
        it->csi_counter--;
      }

      if (it->sr_counter == 0 and it->csi_counter == 0) {
        // Move resource to last position and delete it to avoid O(N) removal.
        if (it != slot_wheel.end() - 1) {
          auto last_it = slot_wheel.end() - 1;
          std::swap(*it, *last_it);
        }
        slot_wheel.pop_back();
      }
    } else {
      log_error();
    }
  }
}

void uci_scheduler_impl::add_ue(const ue_cell_configuration& ue_cfg)
{
  if (not ue_cfg.cfg_dedicated().ul_config.has_value() or
      not ue_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.has_value()) {
    return;
  }

  // Save SR resources in the slot wheel.
  const auto& sr_resource_cfg_list = ue_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value().sr_res_list;
  for (unsigned i = 0; i != sr_resource_cfg_list.size(); ++i) {
    const auto& sr_res = sr_resource_cfg_list[i];
    srsran_assert(sr_res.period >= sr_periodicity::sl_1, "Minimum supported SR periodicity is 1 slot.");

    unsigned period_slots = sr_periodicity_to_slot(sr_res.period);
    add_resource(ue_cfg.crnti, sr_res.offset, period_slots, true);
  }

  if (ue_cfg.cfg_dedicated().csi_meas_cfg.has_value()) {
    // We assume we only use the first CSI report configuration.
    const unsigned csi_report_cfg_idx = 0;
    const auto&    csi_report_cfg = ue_cfg.cfg_dedicated().csi_meas_cfg.value().csi_report_cfg_list[csi_report_cfg_idx];
    const auto&    period_pucch =
        variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(csi_report_cfg.report_cfg_type);

    unsigned period_slots = csi_report_periodicity_to_uint(period_pucch.report_slot_period);
    add_resource(ue_cfg.crnti, period_pucch.report_slot_offset, period_slots, false);
  }

  // Register the UE in the list of recently configured UEs.
  updated_ues.push_back(ue_cfg.crnti);
}

void uci_scheduler_impl::reconf_ue(const ue_cell_configuration& new_ue_cfg, const ue_cell_configuration& old_ue_cfg)
{
  // Detect whether there are any differences in the old and new UE cell config.
  if (new_ue_cfg.cfg_dedicated().ul_config.has_value() and old_ue_cfg.cfg_dedicated().ul_config.has_value() and
      new_ue_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.has_value() and
      old_ue_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.has_value()) {
    // Both old and new UE config have PUCCH config.
    const auto& new_pucch = new_ue_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value();
    const auto& old_pucch = old_ue_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value();

    if (new_pucch.sr_res_list == old_pucch.sr_res_list and
        new_ue_cfg.cfg_dedicated().csi_meas_cfg == old_ue_cfg.cfg_dedicated().csi_meas_cfg) {
      // Nothing changed.
      return;
    }
  }

  rem_ue(old_ue_cfg);
  add_ue(new_ue_cfg);
}

void uci_scheduler_impl::rem_ue(const ue_cell_configuration& ue_cfg)
{
  if (not ue_cfg.cfg_dedicated().ul_config.has_value() or
      not ue_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.has_value()) {
    return;
  }

  const auto& sr_resource_cfg_list = ue_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value().sr_res_list;
  for (unsigned i = 0; i != sr_resource_cfg_list.size(); ++i) {
    const auto& sr_res = sr_resource_cfg_list[i];

    unsigned period_slots = sr_periodicity_to_slot(sr_res.period);
    rem_resource(ue_cfg.crnti, sr_res.offset, period_slots, true);
  }

  if (ue_cfg.cfg_dedicated().csi_meas_cfg.has_value()) {
    // We assume we only use the first CSI report configuration.
    const unsigned csi_report_cfg_idx = 0;
    const auto&    csi_report_cfg = ue_cfg.cfg_dedicated().csi_meas_cfg.value().csi_report_cfg_list[csi_report_cfg_idx];
    const auto&    period_pucch =
        variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(csi_report_cfg.report_cfg_type);

    unsigned period_slots = csi_report_periodicity_to_uint(period_pucch.report_slot_period);
    rem_resource(ue_cfg.crnti, period_pucch.report_slot_offset, period_slots, false);
  }
}

const ue_cell_configuration* uci_scheduler_impl::get_ue_cfg(rnti_t rnti) const
{
  auto* u = ues.find_by_rnti(rnti);
  if (u != nullptr) {
    auto* ue_cc = u->find_cell(cell_cfg.cell_index);
    if (ue_cc != nullptr) {
      return &ue_cc->cfg();
    }
  }
  return nullptr;
}

void uci_scheduler_impl::schedule_slot_ucis(cell_slot_resource_allocator& slot_alloc)
{
  // For the provided slot, check if there are any pending UCI resources to allocate, and allocate them.
  auto& slot_ucis = periodic_uci_slot_wheel[slot_alloc.slot.to_uint() % periodic_uci_slot_wheel.size()];
  for (auto it = slot_ucis.begin(); it != slot_ucis.end();) {
    const periodic_uci_info&     uci_info = *it;
    const ue_cell_configuration* ue_cfg   = get_ue_cfg(uci_info.rnti);

    if (ue_cfg == nullptr) {
      logger.error("cell={} c-rnti={}: UE for which {} is being scheduled was not found (slot={})",
                   cell_cfg.cell_index,
                   uci_info.rnti,
                   it->sr_counter > 0 ? "SR" : (it->csi_counter > 0 ? "CSI" : "invalid UCI"),
                   slot_alloc.slot);
      it = slot_ucis.erase(it);
      continue;
    }

    // Schedule SR PUCCH first.
    // NOTE: Allocating the CSI after the SR helps the PUCCH allocation to compute the number of allocated UCI bits and
    // the corresponding number of PRBs for the PUCCH Format 2 over a PUCCH F2 grant is within PUCCH capacity.
    if (uci_info.sr_counter > 0) {
      uci_alloc.uci_allocate_sr_opportunity(slot_alloc, uci_info.rnti, *ue_cfg);
    }

    // Schedule CSI PUCCH.
    if (uci_info.csi_counter > 0) {
      uci_alloc.uci_allocate_csi_opportunity(slot_alloc, uci_info.rnti, *ue_cfg);
    }

    ++it;
  }
}

void uci_scheduler_impl::schedule_updated_ues_ucis(cell_resource_allocator& cell_alloc)
{
  // For all UEs whose config has been recently updated, schedule their UCIs up until one slot before the farthest
  // slot in the resource grid.
  for (rnti_t rnti : updated_ues) {
    const ue_cell_configuration* ue_cfg = get_ue_cfg(rnti);
    if (ue_cfg == nullptr) {
      logger.error("cell={} c-rnti={}: UE for which UCI is being scheduled was not found.", cell_cfg.cell_index, rnti);
      continue;
    }

    // Schedule UCI up to the farthest slot.
    for (unsigned n = 0; n != cell_alloc.max_ul_slot_alloc_delay; ++n) {
      auto& slot_ucis = periodic_uci_slot_wheel[(cell_alloc.slot_tx() + n).to_uint() % periodic_uci_slot_wheel.size()];

      for (const periodic_uci_info& uci_info : slot_ucis) {
        if (uci_info.rnti == rnti) {
          // Schedule SR PUCCHs first.
          // NOTE: Allocating the CSI after the SR helps the PUCCH allocation to compute the number of allocated UCI
          // bits and the corresponding number of PRBs for the PUCCH Format 2 over a PUCCH F2 grant is within PUCCH
          // capacity.
          if (uci_info.sr_counter > 0) {
            uci_alloc.uci_allocate_sr_opportunity(cell_alloc[n], rnti, *ue_cfg);
          }

          // Schedule CSI
          if (uci_info.csi_counter > 0) {
            uci_alloc.uci_allocate_csi_opportunity(cell_alloc[n], rnti, *ue_cfg);
          }
        }
      }
    }
  }

  // Clear the list of updated UEs.
  updated_ues.clear();
}
