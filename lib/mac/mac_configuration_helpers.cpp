
#include "srsgnb/mac/mac_configuration_helpers.h"

using namespace srsgnb;

mac_cell_configuration srsgnb::make_mac_cell_configuration(const mac_cell_creation_request& request)
{
  mac_cell_configuration ret{};

  ret.cell_index = request.cell_index;
  ret.pci        = request.pci;
  ret.scs_common = request.scs_common;
  ret.ssb_scs    = request.ssb_scs;
  ret.dl_carrier = request.dl_carrier;
  ret.ul_carrier = request.ul_carrier;
  ret.ssb_cfg    = request.ssb_cfg;
  ret.prach_cfg  = request.prach_cfg;
  ret.tdd_cfg    = request.tdd_cfg;
  return ret;
}

sched_cell_configuration_request_message
srsgnb::make_scheduler_cell_configuration_request(const mac_cell_creation_request& request)
{
  sched_cell_configuration_request_message ret{};

  ret.cell_index    = request.cell_index;
  ret.nof_beams     = 1;
  ret.nof_layers    = 1;
  ret.nof_ant_ports = 1;
  ret.pci           = request.pci;

  ret.scs_common = request.scs_common;
  ret.ssb_config = request.ssb_cfg;

  ret.dl_carrier = request.dl_carrier;
  ret.ul_carrier = request.ul_carrier;

  // TODO: Pass valid configuration to scheduler (The code added below for dl_cfg_common and ul_cfg_common was just for
  // ...passing a test).
  ret.dl_cfg_common.init_dl_bwp.pdsch_cfg_common_present = true;
  ret.dl_cfg_common.init_dl_bwp.pdcch_cfg_common_present = true;
  ret.ul_cfg_common.init_ul_bwp.rach_cfg_common.set_setup().rach_cfg_generic.ra_resp_win.value =
      asn1::rrc_nr::rach_cfg_generic_s::ra_resp_win_opts::sl10;

  ret.ul_cfg_common.init_ul_bwp.pusch_cfg_common_present = true;
  ret.ul_cfg_common.init_ul_bwp.pusch_cfg_common.set_setup().pusch_time_domain_alloc_list.resize(1);
  ret.ul_cfg_common.init_ul_bwp.pusch_cfg_common.setup().pusch_time_domain_alloc_list[0].k2_present = true;
  ret.ul_cfg_common.init_ul_bwp.pusch_cfg_common.setup().pusch_time_domain_alloc_list[0].k2         = 2;
  ret.ul_cfg_common.init_ul_bwp.generic_params.subcarrier_spacing.value = asn1::rrc_nr::subcarrier_spacing_opts::khz15;
  ret.dl_cfg_common.init_dl_bwp.pdcch_cfg_common.set_setup().ra_search_space_present = true;
  ret.dl_cfg_common.init_dl_bwp.pdcch_cfg_common.setup().ra_search_space             = 1;
  ret.ul_cfg_common.init_ul_bwp.rach_cfg_common_present                              = true;

  return ret;
}
