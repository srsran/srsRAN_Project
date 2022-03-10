
#ifndef SRSGNB_CONFIG_GENERATORS_H
#define SRSGNB_CONFIG_GENERATORS_H

#include "srsgnb/mac/sched_interface.h"

namespace srsgnb {

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

  return msg;
}


rach_indication_message generate_rach_ind_msg(slot_point prach_slot_rx, rnti_t temp_crnti)
{
  rach_indication_message msg{};
  msg.cell_index      = 0;
  msg.crnti           = temp_crnti;
  msg.timing_info     = 0;
  msg.slot_rx         = prach_slot_rx;
  msg.symbol_index    = 0;
  msg.frequency_index = 0;
  msg.preamble_id     = 0;
  msg.timing_advance  = 0;
  return msg;
}

} // namespace srsgnb

#endif // SRSGNB_CONFIG_GENERATORS_H
