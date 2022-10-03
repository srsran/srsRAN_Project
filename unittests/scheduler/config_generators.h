/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../lib/du_manager/converters/mac_cell_configuration_helpers.h"
#include "srsgnb/mac/mac_configuration_helpers.h"
#include "srsgnb/scheduler/mac_scheduler.h"

namespace srsgnb {

inline sched_cell_configuration_request_message make_default_sched_cell_configuration_request()
{
  du_cell_config default_du_cell_cfg = config_helpers::make_default_du_cell_config();
  /// Random size of a SIB1 payload in bytes.
  const unsigned sib1_payload_size = srs_du::make_asn1_rrc_cell_sib1_buffer(default_du_cell_cfg).length();
  return make_sched_cell_config_req(to_du_cell_index(0), default_du_cell_cfg, sib1_payload_size);
}

rach_indication_message generate_rach_ind_msg(slot_point prach_slot_rx, rnti_t temp_crnti, unsigned rapid = 0)
{
  rach_indication_message msg{};
  msg.cell_index = to_du_cell_index(0);
  msg.slot_rx    = prach_slot_rx;
  msg.occasions.emplace_back();
  msg.occasions.back().frequency_index = 0;
  msg.occasions.back().start_symbol    = 0;
  msg.occasions.back().preambles.emplace_back();
  msg.occasions.back().preambles.back().preamble_id  = rapid;
  msg.occasions.back().preambles.back().tc_rnti      = temp_crnti;
  msg.occasions.back().preambles.back().time_advance = phy_time_unit::from_seconds(0);
  return msg;
}

} // namespace srsgnb
