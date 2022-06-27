/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_DU_CELL_CONFIG_H
#define SRSGNB_DU_CELL_CONFIG_H

#include "srsgnb/ran/carrier_configuration.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/ssb_configuration.h"
#include "srsgnb/scheduler/bwp_configuration.h"

namespace srsgnb {

/// Initial, common Cell Configuration used in DU.
struct du_cell_config {
  pci_t pci;

  carrier_configuration dl_carrier;
  carrier_configuration ul_carrier;

  /// subcarrierSpacing for common, used for initial access and broadcast message.
  subcarrier_spacing scs_common;
  /// SSB subcarrier spacing.
  /// NOTE: Although this is according to O-RAN WG8, we need to verify if this is the correct SCS.
  subcarrier_spacing ssb_scs;
  ssb_configuration  ssb_cfg;

  coreset_id      coreset0_id;
  search_space_id searchspace0_id;

  /// Cell-specific DL and UL configuration used by common searchSpaces.
  dl_config_common dl_cfg_common;
  ul_config_common ul_cfg_common;
};

} // namespace srsgnb

#endif // SRSGNB_DU_CELL_CONFIG_H
