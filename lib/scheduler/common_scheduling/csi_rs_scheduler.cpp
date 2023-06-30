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

#include "csi_rs_scheduler.h"
#include "srsran/ran/csi_rs/csi_rs_config_helpers.h"

using namespace srsran;

static void fill_csi_rs_info_res_map(csi_rs_info& csi_rs, const csi_rs_resource_mapping& res_map)
{
  csi_rs.crbs        = res_map.freq_band_rbs;
  csi_rs.freq_domain = res_map.fd_alloc;
  csi_rs.row         = csi_rs::get_csi_rs_resource_mapping_row_number(
      res_map.nof_ports, res_map.freq_density, res_map.cdm, res_map.fd_alloc);
  srsran_assert(csi_rs.row > 0, "The CSI-RS configuration resulted in an invalid row of Table 7.4.1.5.3-1, TS 38.211");

  csi_rs.symbol0      = res_map.first_ofdm_symbol_in_td;
  csi_rs.symbol1      = res_map.first_ofdm_symbol_in_td2.has_value() ? *res_map.first_ofdm_symbol_in_td2 : 2;
  csi_rs.cdm_type     = res_map.cdm;
  csi_rs.freq_density = res_map.freq_density;
}

static csi_rs_info build_csi_rs_info(const bwp_configuration& bwp_cfg, const zp_csi_rs_resource& zp_csi_rs_res)
{
  csi_rs_info csi_rs;

  csi_rs.bwp_cfg = &bwp_cfg;
  csi_rs.type    = csi_rs_type::CSI_RS_ZP;
  fill_csi_rs_info_res_map(csi_rs, zp_csi_rs_res.res_mapping);

  return csi_rs;
}

static csi_rs_info build_csi_rs_info(const bwp_configuration& bwp_cfg, const nzp_csi_rs_resource& nzp_csi_rs_res)
{
  csi_rs_info csi_rs;

  csi_rs.bwp_cfg = &bwp_cfg;
  csi_rs.type    = srsran::csi_rs_type::CSI_RS_NZP;

  fill_csi_rs_info_res_map(csi_rs, nzp_csi_rs_res.res_mapping);

  csi_rs.scrambling_id                = nzp_csi_rs_res.scrambling_id;
  csi_rs.power_ctrl_offset_profile_nr = nzp_csi_rs_res.pwr_ctrl_offset;
  csi_rs.power_ctrl_offset_ss_profile_nr =
      nzp_csi_rs_res.pwr_ctrl_offset_ss_db.has_value() ? *nzp_csi_rs_res.pwr_ctrl_offset_ss_db : 0;

  return csi_rs;
}

csi_rs_scheduler::csi_rs_scheduler(const cell_configuration& cell_cfg_) : cell_cfg(cell_cfg_)
{
  for (const auto& res : cell_cfg.zp_csi_rs_list) {
    cached_csi_rs.push_back(build_csi_rs_info(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params, res));
  }

  for (const auto& nzp_csi : cell_cfg.nzp_csi_rs_list) {
    cached_csi_rs.push_back(build_csi_rs_info(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params, nzp_csi));
  }
}

void csi_rs_scheduler::run_slot(cell_slot_resource_allocator& res_grid)
{
  if (cached_csi_rs.empty()) {
    return;
  }
  if (not cell_cfg.is_fully_dl_enabled(res_grid.slot)) {
    return;
  }

  // Schedule zp-CSI-RS PDUs.
  for (unsigned i = 0; i != cell_cfg.zp_csi_rs_list.size(); ++i) {
    const zp_csi_rs_resource& zp_csi = cell_cfg.zp_csi_rs_list[i];
    if ((res_grid.slot - *zp_csi.offset).to_uint() % (unsigned)*zp_csi.period == 0) {
      res_grid.result.dl.csi_rs.push_back(cached_csi_rs[i]);
    }
  }

  // Schedule nzp-CSI-RS PDUs.
  for (unsigned i = 0; i != cell_cfg.nzp_csi_rs_list.size(); ++i) {
    const nzp_csi_rs_resource& nzp_csi    = cell_cfg.nzp_csi_rs_list[i];
    const auto&                csi_rs_pdu = cached_csi_rs[cell_cfg.zp_csi_rs_list.size() + i];
    if ((res_grid.slot - *nzp_csi.csi_res_offset).to_uint() % (unsigned)*nzp_csi.csi_res_period == 0) {
      res_grid.result.dl.csi_rs.push_back(csi_rs_pdu);
    }
  }
}
