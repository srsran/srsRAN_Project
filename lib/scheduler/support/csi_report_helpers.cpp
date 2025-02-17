/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "csi_report_helpers.h"

bool srsran::csi_helper::is_csi_reporting_slot(const csi_meas_config& csi_meas, slot_point sl_tx)
{
  // We assume we only use the first CSI report configuration.
  const unsigned csi_report_cfg_idx = 0;
  const auto&    csi_report_cfg     = csi_meas.csi_report_cfg_list[csi_report_cfg_idx];
  const auto&    csi_report_cfg_type =
      std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(csi_report_cfg.report_cfg_type);

  // > Scheduler CSI grants.
  unsigned csi_offset = csi_report_cfg_type.report_slot_offset;
  unsigned csi_period = csi_report_periodicity_to_uint(csi_report_cfg_type.report_slot_period);

  return (sl_tx - csi_offset).to_uint() % csi_period == 0;
}
