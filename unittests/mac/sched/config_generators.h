
#ifndef SRSGNB_CONFIG_GENERATORS_H
#define SRSGNB_CONFIG_GENERATORS_H

#include "srsgnb/mac/sched_interface.h"

namespace srsgnb {

asn1::rrc_nr::rach_cfg_common_s make_rach_cfg_common()
{
  asn1::rrc_nr::rach_cfg_common_s rach;
  rach.rach_cfg_generic.ra_resp_win.value = asn1::rrc_nr::rach_cfg_generic_s::ra_resp_win_opts::sl10;
  // TODO
  return rach;
}

asn1::rrc_nr::ul_cfg_common_sib_s make_ul_cfg_common()
{
  asn1::rrc_nr::ul_cfg_common_sib_s ul_cfg;
  ul_cfg.init_ul_bwp.rach_cfg_common_present     = true;
  ul_cfg.init_ul_bwp.rach_cfg_common.set_setup() = make_rach_cfg_common();
  return ul_cfg;
}

cell_configuration_request_message make_cell_cfg_req()
{
  cell_configuration_request_message msg{};
  msg.cell_index        = 0;
  msg.dl_cell_bandwidth = 10e6;
  msg.dl_cell_bandwidth = 10e6;
  msg.pci               = 1;
  msg.nof_ant_ports     = 1;
  msg.nof_beams         = 1;

  msg.dl_cfg_common.init_dl_bwp.pdsch_cfg_common_present = true;
  msg.dl_cfg_common.init_dl_bwp.pdcch_cfg_common_present = true;
  msg.dl_cfg_common.init_dl_bwp.pdcch_cfg_common.set_setup();

  auto& pdcch_cfg_common = msg.dl_cfg_common.init_dl_bwp.pdcch_cfg_common.setup();

  // RA search space
  pdcch_cfg_common.ra_search_space_present = true;
  pdcch_cfg_common.ra_search_space         = 1;

  msg.ul_cfg_common = make_ul_cfg_common();

  return msg;
}

rach_indication_message generate_rach_ind_msg(slot_point prach_slot_rx, rnti_t temp_crnti, unsigned rapid = 0)
{
  rach_indication_message msg{};
  msg.cell_index      = 0;
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
