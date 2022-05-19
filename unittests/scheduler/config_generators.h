/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CONFIG_GENERATORS_H
#define SRSGNB_CONFIG_GENERATORS_H

#include "../../lib/du_manager/converters/mac_cell_configuration_helpers.h"
#include "srsgnb/mac/mac_configuration_helpers.h"
#include "srsgnb/scheduler/mac_scheduler.h"

namespace srsgnb {

sched_cell_configuration_request_message make_cell_cfg_req()
{
  sched_cell_configuration_request_message msg =
      make_scheduler_cell_configuration_request(test_helpers::make_default_mac_cell_creation_request());
  return msg;
}

rach_indication_message generate_rach_ind_msg(slot_point prach_slot_rx, rnti_t temp_crnti, unsigned rapid = 0)
{
  rach_indication_message msg{};
  msg.cell_index      = to_du_cell_index(0);
  msg.crnti           = temp_crnti;
  msg.timing_info     = 0;
  msg.slot_rx         = prach_slot_rx;
  msg.symbol_index    = 0;
  msg.frequency_index = 0;
  msg.preamble_id     = rapid;
  msg.timing_advance  = 0;
  return msg;
}

} // namespace srsgnb

#endif // SRSGNB_CONFIG_GENERATORS_H
