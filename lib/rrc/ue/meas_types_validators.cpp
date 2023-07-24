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

#include "srsran/rrc/meas_types_validators.h"

using namespace srsran;
using namespace srs_cu_cp;

#define LOG_CHAN ("RRC")

bool srsran::srs_cu_cp::validate_config(const rrc_meas_trigger_quant_offset& config)
{
  if (not config.rsrp.has_value() and not config.rsrq.has_value() and not config.sinr.has_value()) {
    srslog::fetch_basic_logger(LOG_CHAN).error("Either RSRP/RSRQ or SINR need to be configured.");
    return false;
  }
  return true;
}

bool srsran::srs_cu_cp::validate_config(const rrc_cond_event_a3& event)
{
  if (not validate_config(event.a3_offset)) {
    srslog::fetch_basic_logger(LOG_CHAN).error("A3 offset config not valid.");
    return false;
  }

  // TODO: add range checks for hysteresis and time to trigger

  return true;
}