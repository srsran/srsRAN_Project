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

asn1::rrc_nr::rach_cfg_common_s make_rach_cfg_common()
{
  asn1::rrc_nr::rach_cfg_common_s rach;
  rach.rach_cfg_generic.ra_resp_win.value = asn1::rrc_nr::rach_cfg_generic_s::ra_resp_win_opts::sl10;
  // TODO
  return rach;
}

asn1::rrc_nr::ul_cfg_common_sib_s make_ul_cfg_common(uint8_t k2 = 2)
{
  asn1::rrc_nr::ul_cfg_common_sib_s ul_cfg;
  ul_cfg.init_ul_bwp.rach_cfg_common_present     = true;
  ul_cfg.init_ul_bwp.rach_cfg_common.set_setup() = make_rach_cfg_common();

  // Configure PUSCH
  ul_cfg.init_ul_bwp.pusch_cfg_common_present = true;
  ul_cfg.init_ul_bwp.pusch_cfg_common.set_setup();
  auto& pusch_cfg_common = ul_cfg.init_ul_bwp.pusch_cfg_common.setup();
  pusch_cfg_common.pusch_time_domain_alloc_list.resize(1);
  pusch_cfg_common.pusch_time_domain_alloc_list[0].k2_present           = true;
  pusch_cfg_common.pusch_time_domain_alloc_list[0].k2                   = k2;
  pusch_cfg_common.pusch_time_domain_alloc_list[0].start_symbol_and_len = 50;
  pusch_cfg_common.pusch_time_domain_alloc_list[0].map_type =
      asn1::rrc_nr::pusch_time_domain_res_alloc_s::map_type_opts::type_a;

  ul_cfg.init_ul_bwp.generic_params.subcarrier_spacing = asn1::rrc_nr::subcarrier_spacing_opts::khz30;

  return ul_cfg;
}

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
