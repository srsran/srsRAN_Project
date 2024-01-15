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
