/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/ran/csi_rs/csi_rs_types.h"
#include "srsran/scheduler/config/bwp_configuration.h"

namespace srsran {

/// Stores the information associated with a CSI-RS signalling.
struct csi_rs_info {
  const bwp_configuration* bwp_cfg;
  /// Range of RBs where this CSI resource starts and ends, with relation to CRB#0. Only multiples of 4 are allowed.
  crb_interval crbs;
  csi_rs_type  type;
  /// \brief Row entry into the CSI Resource location table, as per 3GP TS 38.211, sec 7.4.1.5.3 and table 7.4.1.5.3-1.
  /// Values: {1,...,18}.
  uint8_t row;
  /// \brief Bitmap defining the frequencyDomainAllocation as per 3GPP TS 38.211, sec 7.4.1.5.3 and 3GPP TS 38.331
  /// "CSIResource Mapping".
  bounded_bitset<12, false> freq_domain;
  /// \brief The time domain location l0 and firstOFDMSymbolInTimeDomain as per 3GPP TS 38.211, sec 7.4.1.5.3.
  /// Values: {0,...,13}.
  uint8_t symbol0;
  /// \brief The time domain location l1 and firstOFDMSymbolInTimeDomain2 as per 3GPP TS 38.211, sec 7.4.1.5.3.
  uint8_t                  symbol1;
  csi_rs_cdm_type          cdm_type;
  csi_rs_freq_density_type freq_density;
  /// \brief ScramblingID of the CSI-RS as per 3GPP TS 38.214, sec 5.2.2.3.1. Values: {0,...,1023}.
  uint16_t scrambling_id;
  /// Ratio of PDSCH EPRE to NZP CSI-RS EPRE as per 3GPP TS 38.214, clause 5.2.2.3.1. Values: {-8,...,15}.
  int8_t power_ctrl_offset;
  /// Ratio of NZP CSI-RS EPRE to SSB/PBCH block EPRE. Values: {-3,0,3,6}.
  int8_t power_ctrl_offset_ss;
};

} // namespace srsran
