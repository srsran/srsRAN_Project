/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "du_pucch_resource_manager.h"

using namespace srsran;
using namespace srs_du;

static std::vector<pucch_resource> build_default_pucch_res_list(const pucch_builder_params& ue_def_pucch_params,
                                                                unsigned                    bwp_size)
{
  // Compute the cell PUCCH resource list, depending on which parameter that has been passed.
  static const unsigned nof_pucch_f2_res_f1 = 1U;
  return srs_du::generate_pucch_res_list_given_number(
      ue_def_pucch_params.nof_ue_pucch_f1_res_harq.to_uint() + ue_def_pucch_params.nof_sr_resources.to_uint(),
      ue_def_pucch_params.nof_ue_pucch_f2_res_harq.to_uint() + nof_pucch_f2_res_f1,
      ue_def_pucch_params.f1_params,
      ue_def_pucch_params.f2_params,
      bwp_size);
}

static pucch_config build_default_pucch_cfg(const pucch_config&                pucch_cfg,
                                            const pucch_builder_params&        user_params,
                                            const std::vector<pucch_resource>& res_list)
{
  // Compute the cell PUCCH resource list, depending on which parameter that has been passed.
  srsran_assert(not res_list.empty(), "Cell PUCCH resource list cannot be empty");

  pucch_config target_pucch_cfg = pucch_cfg;

  // Build the PUCCH resource list. This overwrites the default list.
  if (not ue_pucch_config_builder(target_pucch_cfg,
                                  res_list,
                                  user_params.nof_ue_pucch_f1_res_harq.to_uint(),
                                  user_params.nof_ue_pucch_f2_res_harq.to_uint(),
                                  user_params.nof_sr_resources.to_uint())) {
    srsran_assertion_failure(
        "The requested PUCCH resource list could not be built; the default config will be used instead");
    // TODO: check if we should return a false or to let the allocation continue with default config.
  }

  target_pucch_cfg.format_2_common_param.value().max_c_rate = user_params.f2_params.max_code_rate;

  return target_pucch_cfg;
}

du_pucch_resource_manager::du_pucch_resource_manager(span<const du_cell_config> cell_cfg_list_) :
  user_defined_pucch_cfg(cell_cfg_list_[0].pucch_cfg),
  default_pucch_res_list(
      build_default_pucch_res_list(cell_cfg_list_[0].pucch_cfg,
                                   cell_cfg_list_[0].ul_cfg_common.init_ul_bwp.generic_params.crbs.length())),
  default_pucch_cfg(
      build_default_pucch_cfg(cell_cfg_list_[0].ue_ded_serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg.value(),
                              user_defined_pucch_cfg,
                              default_pucch_res_list)),
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
  const unsigned sr_period = sr_periodicity_to_slot(default_pucch_cfg.sr_res_list[0].period);

  // Compute fundamental CSI report period.
  // TODO: Handle more than one CSI report period.
  unsigned csi_period = 0;
  if (default_csi_report_cfg.has_value()) {
    const auto& rep = srsran::variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
        default_csi_report_cfg->report_cfg_type);
    csi_period = (unsigned)rep.report_slot_period;
  }

  // Setup RAN resources per cell.
  for (auto& cell : cells) {
    for (unsigned offset = 0; offset != sr_period; ++offset) {
      if (cell_cfg_list_[0].tdd_ul_dl_cfg_common.has_value()) {
        const tdd_ul_dl_config_common& tdd_cfg = *cell_cfg_list_[0].tdd_ul_dl_cfg_common;
        const unsigned slot_index = offset % (NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(tdd_cfg.ref_scs));
        if (not has_active_tdd_ul_symbols(tdd_cfg, slot_index)) {
          // UL disabled for this slot.
          continue;
        }
      }
      // Set up the pucch_res_id for the resource used for SR.
      for (unsigned n_sr_res = 0; n_sr_res < user_defined_pucch_cfg.nof_sr_resources.to_uint(); ++n_sr_res) {
        const unsigned pucch_res_id_for_sr = user_defined_pucch_cfg.nof_ue_pucch_f1_res_harq.to_uint() +
                                             user_defined_pucch_cfg.nof_sr_resources.to_uint() - 1U - n_sr_res;
        cell.sr_offset_free_list.emplace_back(pucch_res_id_for_sr, offset);
      }
    }

    for (unsigned offset = 0; offset != csi_period; ++offset) {
      if (cell_cfg_list_[0].tdd_ul_dl_cfg_common.has_value()) {
        const tdd_ul_dl_config_common& tdd_cfg = *cell_cfg_list_[0].tdd_ul_dl_cfg_common;
        const unsigned slot_index = offset % (NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(tdd_cfg.ref_scs));
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
  pucch_config& target_pucch_cfg = cell_grp_cfg.cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg.value();

  auto&    sr_res_list  = target_pucch_cfg.sr_res_list;
  auto&    free_sr_list = cells[cell_grp_cfg.cells[0].serv_cell_cfg.cell_index].sr_offset_free_list;
  unsigned i            = 0;
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

    // Update the CSI report with the correct PUCCH_res_id.
    target_csi_cfg.pucch_csi_res_list.front().pucch_res_id = default_pucch_cfg.pucch_res_list.size() - 1U;

    auto& free_csi_list = cells[cell_grp_cfg.cells[0].serv_cell_cfg.cell_index].csi_offset_free_list;
    if (free_csi_list.empty()) {
      // Allocation failed.
      dealloc_resources(cell_grp_cfg);
      return false;
    }

    // Find CSI slot offset that does not coincide with any of SR slot offsets.
    auto rev_it = std::find_if(free_csi_list.rbegin(), free_csi_list.rend(), [&](unsigned offset) {
      for (const auto& sr : sr_res_list) {
        unsigned lowest_period = std::min(sr_periodicity_to_slot(sr.period),
                                          csi_report_periodicity_to_uint(target_csi_cfg.report_slot_period));
        if (sr.offset % lowest_period == offset % lowest_period) {
          return false;
        }
      }
      return true;
    });
    if (rev_it == free_csi_list.rend()) {
      // If no CSI slot offset that doesn't collide with SR was found, use same slot offset.
      rev_it = free_csi_list.rbegin();
    }
    target_csi_cfg.report_slot_offset = *rev_it;
    free_csi_list.erase(std::next(rev_it).base());
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
