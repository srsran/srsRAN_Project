
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

} // namespace srsgnb

#endif // SRSGNB_CONFIG_GENERATORS_H
