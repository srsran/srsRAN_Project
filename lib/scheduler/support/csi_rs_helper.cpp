/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "csi_rs_helpers.h"

bool srsran::csi_helper::is_csi_rs_slot(const cell_configuration& cell_cfg, slot_point sl_tx)
{
  if (not cell_cfg.is_dl_enabled(sl_tx)) {
    return false;
  }

  if (not cell_cfg.zp_csi_rs_list.empty() and cell_cfg.nzp_csi_rs_list.empty()) {
    return false;
  }

  for (const auto& zp_csi : cell_cfg.zp_csi_rs_list) {
    if (zp_csi.offset.has_value() and zp_csi.period.has_value() and
        (sl_tx - *zp_csi.offset).to_uint() % (unsigned)*zp_csi.period == 0) {
      return true;
    }
  }

  for (const auto& nzp_csi : cell_cfg.nzp_csi_rs_list) {
    if (nzp_csi.csi_res_offset.has_value() and nzp_csi.csi_res_period.has_value() and
        (sl_tx - *nzp_csi.csi_res_offset).to_uint() % (unsigned)*nzp_csi.csi_res_period == 0) {
      return true;
    }
  }

  return false;
}
