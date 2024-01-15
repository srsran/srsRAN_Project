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

#include "csi_report_helpers.h"

bool srsran::csi_helper::is_csi_reporting_slot(const serving_cell_config& ue_cfg, slot_point sl_tx)
{
  if (ue_cfg.csi_meas_cfg.has_value()) {
    // We assume we only use the first CSI report configuration.
    const unsigned csi_report_cfg_idx = 0;
    const auto&    csi_report_cfg     = ue_cfg.csi_meas_cfg.value().csi_report_cfg_list[csi_report_cfg_idx];

    // > Scheduler CSI grants.
    unsigned csi_offset =
        variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(csi_report_cfg.report_cfg_type)
            .report_slot_offset;
    unsigned csi_period = csi_report_periodicity_to_uint(
        variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(csi_report_cfg.report_cfg_type)
            .report_slot_period);

    if ((sl_tx - csi_offset).to_uint() % csi_period == 0) {
      return true;
    }
  }

  return false;
}
