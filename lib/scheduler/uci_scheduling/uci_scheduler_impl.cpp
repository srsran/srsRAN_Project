/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

uci_scheduler_impl::~uci_scheduler_impl() = default;

void uci_scheduler_impl::run_slot(cell_resource_allocator& cell_alloc)
{
  // Initial allocation: we allocate opportunities all over the grid.
  schedule_updated_ues_ucis(cell_alloc);

  // Only allocate in the farthest slot in the grid, as the previous part of the allocation grid has been completed
  // at the first this function was called.
  schedule_slot_ucis(cell_alloc[cell_alloc.max_ul_slot_alloc_delay]);
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
    for (unsigned offset = sr_res.offset; offset < periodic_uci_slot_wheel.size(); offset += period_slots) {
      periodic_uci_slot_wheel[offset].push_back(periodic_uci_info{ue_cfg.crnti, true});
    }
  }

  if (ue_cfg.cfg_dedicated().csi_meas_cfg.has_value()) {
    // We assume we only use the first CSI report configuration.
    const unsigned csi_report_cfg_idx = 0;
    const auto&    csi_report_cfg = ue_cfg.cfg_dedicated().csi_meas_cfg.value().csi_report_cfg_list[csi_report_cfg_idx];
    const auto&    period_pucch =
        variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(csi_report_cfg.report_cfg_type);

    unsigned period_slots = csi_report_periodicity_to_uint(period_pucch.report_slot_period);
    for (unsigned offset = period_pucch.report_slot_offset; offset < periodic_uci_slot_wheel.size();
         offset += period_slots) {
      periodic_uci_slot_wheel[offset].push_back(periodic_uci_info{ue_cfg.crnti, false});
    }
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
    for (unsigned offset = sr_res.offset; offset < periodic_uci_slot_wheel.size(); offset += period_slots) {
      // Removing without keeping order as it is not relevant.
      auto rem_it = std::remove_if(periodic_uci_slot_wheel[offset].begin(),
                                   periodic_uci_slot_wheel[offset].end(),
                                   [&ue_cfg](const auto& e) { return e.rnti == ue_cfg.crnti; });
      periodic_uci_slot_wheel[offset].erase(rem_it, periodic_uci_slot_wheel[offset].end());
    }
  }

  if (ue_cfg.cfg_dedicated().csi_meas_cfg.has_value()) {
    // We assume we only use the first CSI report configuration.
    const unsigned csi_report_cfg_idx = 0;
    const auto&    csi_report_cfg = ue_cfg.cfg_dedicated().csi_meas_cfg.value().csi_report_cfg_list[csi_report_cfg_idx];
    const auto&    period_pucch =
        variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(csi_report_cfg.report_cfg_type);

    unsigned period_slots = csi_report_periodicity_to_uint(period_pucch.report_slot_period);
    for (unsigned offset = period_pucch.report_slot_offset; offset < periodic_uci_slot_wheel.size();
         offset += period_slots) {
      auto rem_it = std::remove_if(periodic_uci_slot_wheel[offset].begin(),
                                   periodic_uci_slot_wheel[offset].end(),
                                   [&ue_cfg](const auto& e) { return e.rnti == ue_cfg.crnti; });
      periodic_uci_slot_wheel[offset].erase(rem_it, periodic_uci_slot_wheel[offset].end());
    }
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
  // NOTE: Allocating the CSI after the SR helps the PUCCH allocation to compute the number of allocated UCI bits and
  // the corresponding number of PRBs for the PUCCH Format 2 over a PUCCH F2 grant is within PUCCH capacity.
  auto& slot_ucis = periodic_uci_slot_wheel[slot_alloc.slot.to_uint() % periodic_uci_slot_wheel.size()];
  for (auto it = slot_ucis.begin(); it != slot_ucis.end();) {
    const periodic_uci_info& uci_info = *it;
    if (uci_info.is_sr) {
      const ue_cell_configuration* ue_cfg = get_ue_cfg(uci_info.rnti);
      if (ue_cfg != nullptr) {
        uci_alloc.uci_allocate_sr_opportunity(slot_alloc, uci_info.rnti, *ue_cfg);
      } else {
        logger.error("UE with RNTI {} and cell={} not found.", uci_info.rnti, cell_cfg.cell_index);
        it = slot_ucis.erase(it);
      }
    }
    ++it;
  }
  for (auto it = slot_ucis.begin(); it != slot_ucis.end();) {
    const periodic_uci_info& uci_info = *it;
    if (not uci_info.is_sr) {
      const ue_cell_configuration* ue_cfg = get_ue_cfg(uci_info.rnti);
      if (ue_cfg != nullptr) {
        uci_alloc.uci_allocate_csi_opportunity(slot_alloc, uci_info.rnti, *ue_cfg);
      } else {
        logger.error("UE with RNTI {} and cell={} not found.", uci_info.rnti, cell_cfg.cell_index);
        it = slot_ucis.erase(it);
      }
    }
    ++it;
  }
}

void uci_scheduler_impl::schedule_updated_ues_ucis(cell_resource_allocator& cell_alloc)
{
  // For all UEs whose config has been recently updated, schedule their UCIs up until one slot before the farthest
  // slot in the resource grid.
  // TODO: Remove UCIs from the list of updated UEs when the UE is removed, if they have no HARQ-ACK.
  for (rnti_t rnti : updated_ues) {
    const ue_cell_configuration* ue_cfg = get_ue_cfg(rnti);
    if (ue_cfg == nullptr) {
      logger.error("UE with RNTI {} and cell={} not found.", rnti, cell_cfg.cell_index);
      continue;
    }

    // Schedule UCI up to the farthest slot.
    for (unsigned n = 0; n != cell_alloc.max_ul_slot_alloc_delay; ++n) {
      auto& slot_ucis = periodic_uci_slot_wheel[(cell_alloc.slot_tx() + n).to_uint() % periodic_uci_slot_wheel.size()];

      // Schedule SRs first
      // NOTE: Allocating the CSI after the SR helps the PUCCH allocation to compute the number of allocated UCI bits
      // and the corresponding number of PRBs for the PUCCH Format 2 over a PUCCH F2 grant is within PUCCH capacity.
      for (const periodic_uci_info& uci_info : slot_ucis) {
        if (uci_info.rnti == rnti) {
          if (uci_info.is_sr) {
            uci_alloc.uci_allocate_sr_opportunity(cell_alloc[n], rnti, *ue_cfg);
          }
        }
      }

      // Schedule CSI
      for (const periodic_uci_info& uci_info : slot_ucis) {
        if (uci_info.rnti == rnti) {
          if (not uci_info.is_sr) {
            uci_alloc.uci_allocate_csi_opportunity(cell_alloc[n], rnti, *ue_cfg);
          }
        }
      }
    }
  }

  // Clear the list of updated UEs.
  updated_ues.clear();
}
