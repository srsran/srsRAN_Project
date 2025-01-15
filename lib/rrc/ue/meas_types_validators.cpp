/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/rrc/meas_types_validators.h"
#include "srsran/srslog/srslog.h"

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
