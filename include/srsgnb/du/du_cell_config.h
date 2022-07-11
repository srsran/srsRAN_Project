/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/ran/carrier_configuration.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/ssb_configuration.h"
#include "srsgnb/scheduler/bwp_configuration.h"

namespace srsgnb {

/// Initial, common Cell Configuration used in DU.
struct du_cell_config {
  pci_t       pci;
  std::string plmn;
  uint32_t    tac;
  uint32_t    cell_id;

  carrier_configuration dl_carrier;
  carrier_configuration ul_carrier;

  /// subcarrierSpacing for common, used for initial access and broadcast message.
  subcarrier_spacing scs_common;
  ssb_configuration  ssb_cfg;

  dmrs_typeA_position dmrs_typeA_pos;

  /// CORESET#0 index of Table 13-{1, ..., 10}, TS 38.213.
  unsigned coreset0_idx;

  /// SearcSpace#0 index of Table 13-{11, ..., 15}, TS 38.213.
  unsigned searchspace0_idx;

  /// Parameters used to pack MIB.
  /// "cellBarred" as per MIB, TS 38.311. true = barred; false = notBarred.
  bool cell_barred;
  /// "intraFreqReselection" as per MIB, TS 38.311. true = allowed; false = notAllowed.
  bool intra_freq_resel;

  /// Cell-specific DL and UL configuration used by common searchSpaces.
  dl_config_common dl_cfg_common;
  ul_config_common ul_cfg_common;
};

} // namespace srsgnb
