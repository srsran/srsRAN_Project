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

#include "csi_rs_helpers.h"

bool srsran::csi_helper::is_csi_rs_slot(const cell_configuration& cell_cfg, slot_point sl_tx)
{
  if (not cell_cfg.is_dl_enabled(sl_tx)) {
    return false;
  }

  if (not cell_cfg.zp_csi_rs_list.empty() and cell_cfg.nzp_csi_rs_list.empty()) {
    return false;
  }

  //  for (unsigned i = 0; i != cell_cfg.zp_csi_rs_list.size(); ++i)
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
