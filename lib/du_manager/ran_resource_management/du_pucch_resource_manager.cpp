/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_pucch_resource_manager.h"

using namespace srsran;
using namespace srs_du;

du_pucch_resource_manager::du_pucch_resource_manager(span<const du_cell_config> cell_cfg_list_) :
  default_pucch_cfg(cell_cfg_list_[0].ue_ded_serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg.value()),
  default_csi_report_cfg([&cell_cfg_list_]() -> optional<csi_report_config> {
    const auto& csi_meas = cell_cfg_list_[0].ue_ded_serv_cell_cfg.csi_meas_cfg;
    if (csi_meas.has_value() and not csi_meas->csi_report_cfg_list.empty()) {
      return csi_meas->csi_report_cfg_list[0];
    }
    return nullopt;
  }()),
  cells(cell_cfg_list_.size())
{
  srsran_assert(not default_pucch_cfg.sr_res_list.empty(), "There must be at least one SR Resource");

  // Compute fundamental SR period.
  // TODO: Handle more than one SR period.
  unsigned sr_period = sr_periodicity_to_slot(default_pucch_cfg.sr_res_list[0].period);

  // Compute fundamental CSI report period.
  // TODO: Handle more than one CSI report period.
  unsigned csi_period = 0;
  if (default_csi_report_cfg.has_value()) {
    auto& rep = srsran::variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
        default_csi_report_cfg->report_cfg_type);
    csi_period = (unsigned)rep.report_slot_period;
  }

  // Setup RAN resources per cell.
  for (auto& cell : cells) {
    for (unsigned offset = 0; offset != sr_period; ++offset) {
      if (cell_cfg_list_[0].tdd_ul_dl_cfg_common.has_value()) {
        const tdd_ul_dl_config_common& tdd_cfg = *cell_cfg_list_[0].tdd_ul_dl_cfg_common;
        unsigned slot_index = offset % (NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(tdd_cfg.ref_scs));
        if (not has_active_tdd_ul_symbols(tdd_cfg, slot_index)) {
          // UL disabled for this slot.
          continue;
        }
      }
      cell.sr_offset_free_list.emplace_back(default_pucch_cfg.sr_res_list[0].pucch_res_id - 1, offset);
      cell.sr_offset_free_list.emplace_back(default_pucch_cfg.sr_res_list[0].pucch_res_id, offset);
    }

    for (unsigned offset = 0; offset != csi_period; ++offset) {
      if (cell_cfg_list_[0].tdd_ul_dl_cfg_common.has_value()) {
        const tdd_ul_dl_config_common& tdd_cfg = *cell_cfg_list_[0].tdd_ul_dl_cfg_common;
        unsigned slot_index = offset % (NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(tdd_cfg.ref_scs));
        if (not has_active_tdd_ul_symbols(tdd_cfg, slot_index)) {
          // UL disabled for this slot.
          continue;
        }
      }
      cell.csi_offset_free_list.push_back(offset);
    }
  }
}

bool du_pucch_resource_manager::alloc_resources(cell_group_config& cell_grp_cfg)
{
  // Allocation of SR PUCCH offset.
  cell_grp_cfg.cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg = default_pucch_cfg;
  auto&    target_pucch_cfg = *cell_grp_cfg.cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg;
  auto&    sr_res_list      = target_pucch_cfg.sr_res_list;
  auto&    free_sr_list     = cells[cell_grp_cfg.cells[0].serv_cell_cfg.cell_index].sr_offset_free_list;
  unsigned i                = 0;
  for (; i != sr_res_list.size(); ++i) {
    if (free_sr_list.empty()) {
      break;
    }
    sr_res_list[i].pucch_res_id = free_sr_list.back().first;
    sr_res_list[i].offset       = free_sr_list.back().second;
    free_sr_list.pop_back();
  }
  if (i != sr_res_list.size()) {
    // Allocation failed. Return resources back to the pool.
    dealloc_resources(cell_grp_cfg);
    return false;
  }

  // Allocation of CSI PUCCH offset.
  if (default_csi_report_cfg.has_value()) {
    cell_grp_cfg.cells[0].serv_cell_cfg.csi_meas_cfg->csi_report_cfg_list = {*default_csi_report_cfg};
    auto& target_csi_cfg = srsran::variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
        cell_grp_cfg.cells[0].serv_cell_cfg.csi_meas_cfg->csi_report_cfg_list[0].report_cfg_type);
    auto& free_csi_list = cells[cell_grp_cfg.cells[0].serv_cell_cfg.cell_index].csi_offset_free_list;
    if (free_csi_list.empty()) {
      // Allocation failed.
      dealloc_resources(cell_grp_cfg);
      return false;
    }
    target_csi_cfg.report_slot_offset = free_csi_list.back();
    free_csi_list.pop_back();
  }

  return true;
}

void du_pucch_resource_manager::dealloc_resources(cell_group_config& cell_grp_cfg)
{
  for (auto& sr : cell_grp_cfg.cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list) {
    cells[cell_grp_cfg.cells[0].serv_cell_cfg.cell_index].sr_offset_free_list.emplace_back(sr.pucch_res_id, sr.offset);
  }
  if (cell_grp_cfg.cells[0].serv_cell_cfg.csi_meas_cfg.has_value()) {
    auto& target_csi_cfg = srsran::variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
        cell_grp_cfg.cells[0].serv_cell_cfg.csi_meas_cfg->csi_report_cfg_list[0].report_cfg_type);
    cells[cell_grp_cfg.cells[0].serv_cell_cfg.cell_index].csi_offset_free_list.push_back(
        target_csi_cfg.report_slot_offset);
  }
}
