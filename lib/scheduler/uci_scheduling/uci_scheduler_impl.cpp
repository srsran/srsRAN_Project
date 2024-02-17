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
}

uci_scheduler_impl::~uci_scheduler_impl() = default;

void uci_scheduler_impl::run_slot(cell_resource_allocator& cell_alloc, slot_point sl_tx)
{
  for (auto& user : ues) {
    // At this point, we assume the config validator ensures there is pCell.
    auto& ue_cell = user->get_pcell();

    if (not ue_cell.is_active() or not ue_cell.cfg().cfg_dedicated().ul_config.has_value()) {
      continue;
    }

    optional<std::pair<unsigned, unsigned>> csi_period_and_offset;
    if (ue_cell.cfg().cfg_dedicated().csi_meas_cfg.has_value()) {
      // We assume we only use the first CSI report configuration.
      const unsigned csi_report_cfg_idx = 0;
      const auto&    csi_report_cfg =
          ue_cell.cfg().cfg_dedicated().csi_meas_cfg.value().csi_report_cfg_list[csi_report_cfg_idx];

      csi_period_and_offset.emplace(std::pair<unsigned, unsigned>{
          csi_report_periodicity_to_uint(variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                                             csi_report_cfg.report_cfg_type)
                                             .report_slot_period),
          variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(csi_report_cfg.report_cfg_type)
              .report_slot_offset});
    }

    if (ue_cell.is_pucch_grid_inited()) {
      // Only allocate in the farthest slot in the grid, as the previous part of the allocation grid has been completed
      // at the first this function was called.
      schedule_uci(cell_alloc[cell_alloc.max_ul_slot_alloc_delay], user->crnti, ue_cell, csi_period_and_offset);
    }
    // Initial allocation: we allocate opportunities all over the grid.
    else {
      for (unsigned n = 0; n != cell_alloc.max_ul_slot_alloc_delay + 1; ++n) {
        schedule_uci(cell_alloc[n], user->crnti, ue_cell, csi_period_and_offset);
      }
      ue_cell.set_pucch_grid_inited();
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
    for (unsigned offset = sr_res.offset; offset < periodic_uci_slot_wheel.size(); offset += period_slots) {
      periodic_uci_slot_wheel[offset].push_back(periodic_uci_info{ue_cfg.crnti, true, i});
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
      periodic_uci_slot_wheel[offset].push_back(periodic_uci_info{ue_cfg.crnti, false, 0});
    }
  }
}

void uci_scheduler_impl::reconf_ue(const ue_cell_configuration& new_ue_cfg, const ue_cell_configuration& old_ue_cfg)
{
  // Detect differences.
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

void uci_scheduler_impl::schedule_uci(cell_slot_resource_allocator&           slot_alloc,
                                      rnti_t                                  crnti,
                                      const ue_cell&                          user,
                                      optional<std::pair<unsigned, unsigned>> csi_period_and_offset)
{
  if (not cell_cfg.is_fully_ul_enabled(slot_alloc.slot)) {
    return;
  }

  // At this point, we assume the UE has a \c ul_config, a \c pucch_cfg and a \c sr_res_list.
  const auto& sr_resource_cfg_list =
      user.cfg().cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value().sr_res_list;

  // NOTE: Allocating the CSI after the SR helps the PUCCH allocation to compute the number of allocated UCI bits and
  // the corresponding number of PRBs for the PUCCH Format 2 over a PUCCH F2 grant is within PUCCH capacity.

  for (const auto& sr_res : sr_resource_cfg_list) {
    srsran_assert(sr_res.period >= sr_periodicity::sl_1, "Minimum supported SR periodicity is 1 slot.");

    if ((slot_alloc.slot - sr_res.offset).to_uint() % sr_periodicity_to_slot(sr_res.period) == 0) {
      uci_alloc.uci_allocate_sr_opportunity(slot_alloc, crnti, user.cfg());
    }
  }

  if (csi_period_and_offset.has_value()) {
    if ((slot_alloc.slot - csi_period_and_offset->second).to_uint() % csi_period_and_offset->first == 0) {
      uci_alloc.uci_allocate_csi_opportunity(slot_alloc, crnti, user.cfg());
    }
  }
}
