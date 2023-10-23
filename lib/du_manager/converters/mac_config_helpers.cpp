/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
