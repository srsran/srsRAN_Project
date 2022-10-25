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

#include "srsgnb/ran/band_helper.h"
#include "srsgnb/ran/bs_channel_bandwidth.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/ssb_properties.h"
#include "srsgnb/ran/tdd_ul_dl_config.h"

namespace srsgnb {

/// \brief Main cell parameters from which other cell parameters (e.g. coreset0, BWP RBs) will be derived.
/// \remark Only fields that may affect many different fields in du_cell_config (e.g. number of PRBs) should be added
/// in this struct.
struct du_cell_config_master_params {
  /// Physical Cell Identity.
  pci_t pci = 1;
  /// subCarrierSpacingCommon, as per \c MIB, TS 38.311.
  subcarrier_spacing scs_common = subcarrier_spacing::kHz15;
  /// BS Channel Bandwidth, as per TS 38.104, Section 5.3.1.
  bs_channel_bandwidth_fr1 channel_bw_mhz = bs_channel_bandwidth_fr1::MHz10;
  /// The number of Common Resource Blocks (nof_crbs) for the main carrier is determined according to TS 38.104,
  /// Table 5.3.2-1.
  unsigned nof_crbs = band_helper::get_n_rbs_from_bw(channel_bw_mhz, scs_common, frequency_range::FR1);
  /// This ARFCN represents "f_ref" for DL, as per TS 38.211, Section 5.4.2.1.
  unsigned dl_arfcn = 365000;
  /// <em>NR operating band<\em>, as per Table 5.2-1 and 5.2-2, TS 38.104.
  nr_band band = nr_band::n3;
  /// offsetToPointA, as per TS 38.211, Section 4.4.4.2; \ref ssb_offset_to_pointA.
  ssb_offset_to_pointA offset_to_point_a{12};
  /// This is \c controlResourceSetZero, as per TS38.213, Section 13.
  unsigned coreset0_index = 6;
};

} // namespace srsgnb