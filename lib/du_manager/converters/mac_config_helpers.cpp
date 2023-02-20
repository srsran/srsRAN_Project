/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_config_helpers.h"
#include "srsgnb/du/du_cell_config_helpers.h"

using namespace srsran;

/// Derives MAC Cell Configuration from DU Cell Configuration.
mac_cell_creation_request srsran::make_mac_cell_config(du_cell_index_t       cell_index,
                                                       const du_cell_config& du_cfg,
                                                       byte_buffer           bcch_dl_sch_payload,
                                                       const sched_cell_configuration_request_message& sched_cell_cfg)
{
  mac_cell_creation_request mac_cfg{};
  mac_cfg.cell_index          = cell_index;
  mac_cfg.pci                 = du_cfg.pci;
  mac_cfg.scs_common          = du_cfg.scs_common;
  mac_cfg.ssb_cfg             = du_cfg.ssb_cfg;
  mac_cfg.dl_carrier          = du_cfg.dl_carrier;
  mac_cfg.ul_carrier          = du_cfg.ul_carrier;
  mac_cfg.cell_barred         = du_cfg.cell_barred;
  mac_cfg.intra_freq_resel    = du_cfg.intra_freq_resel;
  mac_cfg.bcch_dl_sch_payload = std::move(bcch_dl_sch_payload);
  mac_cfg.sched_req           = sched_cell_cfg;
  return mac_cfg;
}
