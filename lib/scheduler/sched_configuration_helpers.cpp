
#include "srsgnb/scheduler/sched_configuration_helpers.h"

using namespace srsgnb;

cell_configuration_request_message
srsgnb::make_sched_cell_configuration_request_message(const mac_cell_configuration& cell_cfg)
{
  cell_configuration_request_message msg{};

  // Copy general parameters
  msg.cell_index = cell_cfg.cell_index;
  msg.pci        = cell_cfg.pci;
  msg.dl_carrier = cell_cfg.dl_carrier;
  msg.ul_carrier = cell_cfg.ul_carrier;

  // TODO: Pass valid configuration to scheduler (The code added below for dl_cfg_common and ul_cfg_common was just for
  // ...passing a test).
  msg.nof_ant_ports                                      = 1;
  msg.nof_beams                                          = 1;
  msg.dl_cfg_common.init_dl_bwp.pdsch_cfg_common_present = true;
  msg.dl_cfg_common.init_dl_bwp.pdcch_cfg_common_present = true;
  msg.ul_cfg_common.init_ul_bwp.rach_cfg_common.set_setup().rach_cfg_generic.ra_resp_win.value =
      asn1::rrc_nr::rach_cfg_generic_s::ra_resp_win_opts::sl10;
  msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common_present = true;
  msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.set_setup().pusch_time_domain_alloc_list.resize(1);
  msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.setup().pusch_time_domain_alloc_list[0].k2_present = true;
  msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.setup().pusch_time_domain_alloc_list[0].k2         = 2;
  msg.ul_cfg_common.init_ul_bwp.generic_params.subcarrier_spacing.value = asn1::rrc_nr::subcarrier_spacing_opts::khz15;
  msg.dl_cfg_common.init_dl_bwp.pdcch_cfg_common.set_setup().ra_search_space_present = true;
  msg.dl_cfg_common.init_dl_bwp.pdcch_cfg_common.setup().ra_search_space             = 1;
  msg.ul_cfg_common.init_ul_bwp.rach_cfg_common_present                              = true;

  // Copy SSB parameters
  msg.ssb_config = cell_cfg.ssb_cfg;

  // TODO: Add remaining fields.
  return msg;
}
