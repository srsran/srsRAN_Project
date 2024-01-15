/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#pragma once

#include "srsran/ran/band_helper.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/ssb_properties.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"

namespace srsran {

/// \brief Main cell parameters from which other cell parameters (e.g. coreset0, BWP RBs) will be derived.
/// \remark Only fields that may affect many different fields in du_cell_config (e.g. number of PRBs) should be added
/// in this struct.
struct cell_config_builder_params {
  /// Physical Cell Identity.
  pci_t pci = 1;
  /// subCarrierSpacingCommon, as per \c MIB, TS 38.331.
  subcarrier_spacing scs_common = subcarrier_spacing::kHz15;
  /// BS Channel Bandwidth, as per TS 38.104, Section 5.3.1.
  bs_channel_bandwidth_fr1 channel_bw_mhz = bs_channel_bandwidth_fr1::MHz10;
  /// This ARFCN represents "f_ref" for DL, as per TS 38.211, Section 5.4.2.1.
  unsigned dl_arfcn = 365000;
  /// <em>NR operating band<\em>, as per Table 5.2-1 and 5.2-2, TS 38.104. If not specified, a valid band for the
  /// provided DL ARFCN is automatically derived.
  optional<nr_band> band;
  /// offsetToPointA, as per TS 38.211, Section 4.4.4.2; \ref ssb_offset_to_pointA. If not specified, a valid offset
  /// is derived.
  optional<ssb_offset_to_pointA> offset_to_point_a;
  /// This is \c controlResourceSetZero, as per TS38.213, Section 13. If not specified, a valid coreset0 is derived.
  optional<unsigned> coreset0_index;
  /// Maximum CORESET#0 duration in OFDM symbols to consider when deriving CORESET#0 index.
  uint8_t max_coreset0_duration = 2;
  /// This is \c searchSpaceZero, as per TS38.213, Section 13.
  unsigned search_space0_index = 0;
  /// \brief k_ssb or SSB SubcarrierOffest, as per TS38.211 Section 7.4.3.1. Possible values: {0, ..., 23}. If not
  /// specified, a valid k_ssb is derived.
  optional<ssb_subcarrier_offset> k_ssb;
  /// Whether to enable CSI-RS in the cell.
  bool csi_rs_enabled = true;
  /// Number of DL ports for the cell.
  unsigned nof_dl_ports = 1;
  /// \brief Minimum k1 value used in the generation of the UE "dl-DataToUl-Ack", as per TS38.213, 9.1.2.1.
  /// Possible values: {1, ..., 15}.
  uint8_t min_k1 = 4;
  /// \brief Minimum k2 value used in the generation of the UE PUSCH time-domain resources. The value of min_k2
  /// should be equal or lower than min_k1. Otherwise, the scheduler is forced to pick higher k1 values, as it cannot
  /// allocate PUCCHs in slots where there is an PUSCH with an already assigned DAI.
  /// Possible values: {1, ..., 32}.
  uint8_t min_k2 = 4;
  /// Defines the TDD DL-UL pattern and periodicity. If no value is set, the cell is in FDD mode.
  optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common;
};

} // namespace srsran
