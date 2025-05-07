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

#include "srsran/ran/dmrs.h"
#include "srsran/ran/ssb/ssb_properties.h"
#include "srsran/ran/subcarrier_spacing.h"
#include <cstdint>

namespace srsran {

/// \brief Collects the PBCH message contents for the Master Information Block (MIB) parameters.
///
/// The MIB is defined by Information Element \e MIB in TS38.331 Section 6.2.2.
struct pbch_mib_message {
  /// System Frame Number (0..1023). Parameter \e systemFrameNumber.
  uint32_t sfn;
  /// \c true if the SS/PBCH block transmission is in an odd half frame, \c false otherwise.
  bool hrf;
  /// Parameter \e subCarrierSpacingCommon.
  subcarrier_spacing scs_common;
  /// Parameter \e ssb-SubcarrierOffset.
  ssb_subcarrier_offset subcarrier_offset;
  /// Field \e dmrs-TypeA-Position.
  dmrs_typeA_position dmrs_typeA_pos;
  /// Field \e pdcch-ConfigSIB1.
  uint8_t pdcch_config_sib1;
  /// Field \e pdcch-ConfigSIB1.
  bool cell_barred;
  /// Field \e intraFreqReselection.
  bool intrafreq_reselection;
  /// SSB opportunity index in a 5ms burst.
  uint8_t ssb_block_index;
};

} // namespace srsran
