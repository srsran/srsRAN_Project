/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_MAC_CELL_CONFIGURATION_HELPERS_H
#define SRSGNB_MAC_CELL_CONFIGURATION_HELPERS_H

#include "srsgnb/mac/cell_configuration.h"

// TODO: This file is temporary. Eventually we will receive cell configurations from the DU config file.

namespace srsgnb {

namespace test_helpers {

inline carrier_configuration make_default_carrier_configuration()
{
  carrier_configuration cfg;
  cfg.carrier_bw_mhz = 10;
  cfg.arfcn          = 365000;
  cfg.nof_ant        = 1;
  return cfg;
}

inline mac_cell_creation_request make_default_mac_cell_creation_request()
{
  mac_cell_creation_request msg{};

  msg.cell_index = to_du_cell_index(0);
  msg.pci        = 1;

  msg.scs_common = subcarrier_spacing::kHz15;
  msg.dl_carrier = make_default_carrier_configuration();
  msg.ul_carrier = make_default_carrier_configuration();

  // Configure initial DL BWP.
  msg.dl_cfg_common.init_dl_bwp.generic_params.scs  = subcarrier_spacing::kHz15;
  msg.dl_cfg_common.init_dl_bwp.generic_params.prbs = {0, 52};
  msg.dl_cfg_common.init_dl_bwp.pdcch_common.coresets.emplace(0);
  msg.dl_cfg_common.init_dl_bwp.pdcch_common.coresets[0].id = to_coreset_id(0);
  msg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces.emplace(0);
  msg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[0].id    = to_search_space_id(0);
  msg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[0].cs_id = to_coreset_id(0);
  msg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id     = to_search_space_id(1);

  // TODO: Remaining fields.

  return msg;
}

} // namespace test_helpers

} // namespace srsgnb

#endif // SRSGNB_MAC_CELL_CONFIGURATION_HELPERS_H
