/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "sr_helper.h"

bool srsran::sr_helper::is_sr_opportunity_slot(const pucch_config& pucch_cfg, slot_point sl_tx)
{
  // NOTE: This function is only used within the scheduler, where we assume the UE has a \c ul_config, a \c pucch_cfg
  // and a \c sr_res_list (verified by the scheduler validator).
  const auto& sr_resource_cfg_list = pucch_cfg.sr_res_list;

  for (const auto& sr_res : sr_resource_cfg_list) {
    // Check if this slot corresponds to an SR opportunity for the UE.
    if ((sl_tx - sr_res.offset).to_uint() % sr_periodicity_to_slot(sr_res.period) == 0) {
      return true;
    }
  }

  return false;
}
