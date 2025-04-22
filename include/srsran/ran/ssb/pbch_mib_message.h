/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  /// System frame number (0..1023). Parameter \e systemFrameNumber.
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
