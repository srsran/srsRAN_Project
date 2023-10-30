/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_config_helpers.h"
#include "srsran/du/du_cell_config_helpers.h"

using namespace srsran;

/// Derives MAC Cell Configuration from DU Cell Configuration.
mac_cell_creation_request srsran::make_mac_cell_config(du_cell_index_t          cell_index,
                                                       const du_cell_config&    du_cfg,
                                                       std::vector<byte_buffer> bcch_dl_sch_payloads,
                                                       const sched_cell_configuration_request_message& sched_cell_cfg)
{
  mac_cell_creation_request mac_cfg{};
  mac_cfg.cell_index           = cell_index;
  mac_cfg.pci                  = du_cfg.pci;
  mac_cfg.scs_common           = du_cfg.scs_common;
  mac_cfg.ssb_cfg              = du_cfg.ssb_cfg;
  mac_cfg.dl_carrier           = du_cfg.dl_carrier;
  mac_cfg.ul_carrier           = du_cfg.ul_carrier;
  mac_cfg.cell_barred          = du_cfg.cell_barred;
  mac_cfg.intra_freq_resel     = du_cfg.intra_freq_resel;
  mac_cfg.bcch_dl_sch_payloads = std::move(bcch_dl_sch_payloads);
  mac_cfg.sched_req            = sched_cell_cfg;
  return mac_cfg;
}

mac_lc_config srsran::make_default_srb_mac_lc_config()
{
  mac_lc_config mac_cfg{};
  mac_cfg.priority            = 1;
  mac_cfg.lcg_id              = uint_to_lcg_id(0);
  mac_cfg.pbr                 = to_prioritized_bit_rate(65537);
  mac_cfg.bsd                 = to_bucket_size_duration(5);
  mac_cfg.lc_sr_mask          = false;
  mac_cfg.lc_sr_delay_applied = false;
  mac_cfg.sr_id               = uint_to_sched_req_id(0);
  return mac_cfg;
}

mac_lc_config srsran::make_default_drb_mac_lc_config()
{
  mac_lc_config mac_cfg = make_default_srb_mac_lc_config();
  mac_cfg.priority      = 2;
  mac_cfg.lcg_id        = uint_to_lcg_id(1);
  return mac_cfg;
}
