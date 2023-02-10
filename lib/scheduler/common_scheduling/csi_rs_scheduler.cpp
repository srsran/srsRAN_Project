/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "csi_rs_scheduler.h"

using namespace srsgnb;

static csi_rs_info build_csi_rs_info(const bwp_configuration& bwp_cfg, const nzp_csi_rs_resource& nzp_csi_rs_res)
{
  csi_rs_info csi_rs;

  csi_rs.bwp_cfg = &bwp_cfg;
  csi_rs.crbs    = {nzp_csi_rs_res.res_mapping.freq_band_start_rb,
                    nzp_csi_rs_res.res_mapping.freq_band_start_rb + nzp_csi_rs_res.res_mapping.freq_band_nof_rb};
  csi_rs.type    = srsgnb::csi_rs_type::CSI_RS_NZP;
  // TODO: Handle other rows
  auto& row1 = variant_get<csi_rs_resource_mapping::fd_alloc_row1>(nzp_csi_rs_res.res_mapping.fd_alloc);
  csi_rs.row = 1;
  csi_rs.freq_domain.resize(row1.size());
  for (unsigned i = 0; i != row1.size(); ++i) {
    csi_rs.freq_domain.set(i, row1.test(i));
  }
  csi_rs.symbol0                      = nzp_csi_rs_res.res_mapping.first_ofdm_symbol_in_td;
  csi_rs.symbol1                      = nzp_csi_rs_res.res_mapping.first_ofdm_symbol_in_td2.has_value()
                                            ? *nzp_csi_rs_res.res_mapping.first_ofdm_symbol_in_td2
                                            : 2;
  csi_rs.cdm_type                     = nzp_csi_rs_res.res_mapping.cdm;
  csi_rs.freq_density                 = nzp_csi_rs_res.res_mapping.freq_density;
  csi_rs.scrambling_id                = nzp_csi_rs_res.scrambling_id;
  csi_rs.power_ctrl_offset_profile_nr = nzp_csi_rs_res.pwr_ctrl_offset;
  csi_rs.power_ctrl_offset_ss_profile_nr =
      nzp_csi_rs_res.pwr_ctrl_offset_ss_db.has_value() ? *nzp_csi_rs_res.pwr_ctrl_offset_ss_db : 0;

  return csi_rs;
}

csi_rs_scheduler::csi_rs_scheduler(const cell_configuration& cell_cfg_) :
  cell_cfg(cell_cfg_),
  cached_csi_rs(cell_cfg.nzp_csi_res.has_value()
                    ? build_csi_rs_info(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params, *cell_cfg.nzp_csi_res)
                    : optional<const csi_rs_info>{})
{
}

void csi_rs_scheduler::run_slot(cell_slot_resource_allocator& res_grid)
{
  if (not cached_csi_rs.has_value()) {
    return;
  }

  slot_point sl_tx = res_grid.slot;

  if (((sl_tx.to_uint() - *cell_cfg.nzp_csi_res->csi_res_offset) % (unsigned)*cell_cfg.nzp_csi_res->csi_res_period) ==
      0) {
    res_grid.result.dl.csi_rs.emplace_back(*cached_csi_rs);
  }
}
