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

#include "restricted_set_config.h"
#include "srsran/adt/bounded_integer.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/subcarrier_spacing.h"

namespace srsran {

/// \remark See TS 38.331, RACH-ConfigGeneric.
struct rach_config_generic {
  /// Values: {0,...,255}.
  uint8_t prach_config_index;
  /// Msg2 RAR window length in #slots. Network configures a value < 10msec. Values: (1, 2, 4, 8, 10, 20, 40, 80).
  unsigned ra_resp_window;
  /// Number of PRACH occasions FDMed in one time instance as per TS38.211, clause 6.3.3.2.
  unsigned msg1_fdm;
  /// Offset of lowest PRACH transmission occasion in frequency domain respective to PRB 0,
  /// as per TS38.211, clause 6.3.3.2. Possible values: {0,...,MAX_NOF_PRB - 1}.
  unsigned msg1_frequency_start;
  /// Zero-correlation zone configuration number as per TS38.331 "zeroCorrelationZoneConfig", used to derive N_{CS}.
  uint16_t zero_correlation_zone_config;
  /// \brief \c preambleReceivedTargetPower, part of \c RACH-ConfigGeneric, TS 38.311.
  /// Target power level at the network receiver side, in dBm. Only values multiple of 2 are valid.
  bounded_integer<int, -202, -60> preamble_rx_target_pw;
  /// Max number of RA preamble transmissions performed before declaring a failure. Values {3, 4, 5, 6, 7, 8, 10, 20,
  /// 50, 100, 200}.
  uint8_t preamble_trans_max = 7;
  /// Power ramping steps for PRACH. Values {0, 2, 4, 6}.
  uint8_t power_ramping_step_db = 4;
};

/// Used to specify the cell-specific random-access parameters as per TS 38.331, "RACH-ConfigCommon".
struct rach_config_common {
  rach_config_generic rach_cfg_generic;
  /// Total number of prambles used for contention based and contention free RA. Values: (1..64).
  optional<unsigned> total_nof_ra_preambles;
  /// PRACH Root Sequence Index can be of 2 types, as per \c prach-RootSequenceIndex, \c RACH-ConfigCommon, TS 38.331.
  /// We use \c true for l839, while \c false for l139.
  bool is_prach_root_seq_index_l839;
  /// PRACH root sequence index. Values: (1..839).
  /// \remark See TS 38.211, clause 6.3.3.1.
  unsigned prach_root_seq_index;
  /// \brief Subcarrier spacing of PRACH as per TS38.331, "RACH-ConfigCommon". If invalid, the UE applies the SCS as
  /// derived from the prach-ConfigurationIndex in RACH-ConfigGeneric as per TS38.211 Tables 6.3.3.1-[1-3].
  subcarrier_spacing    msg1_scs;
  restricted_set_config restricted_set;
  /// Enables the transform precoder for Msg3 transmission according to clause 6.1.3 of TS 38.214.
  bool msg3_transform_precoder;
  /// Indicates the number of SSBs per RACH occasion (L1 parameter 'SSB-per-rach-occasion'). See TS 38.331, \c
  /// ssb-perRACH-OccasionAndCB-PreamblesPerSSB. Values {1/8, 1/4, 1/2, 1, 2, 4, 8, 16}.
  /// Value 1/8 corresponds to one SSB associated with 8 RACH occasions and so on so forth.
  float nof_ssb_per_ro = 1;
  /// Indicates the number of Contention Based preambles per SSB (L1 parameter 'CB-preambles-per-SSB'). See TS 38.331,
  /// \c ssb-perRACH-OccasionAndCB-PreamblesPerSSB.
  /// \remark Values of \c cb_preambles_per_ssb depends on value of \c ssb_per_ro.
  uint8_t nof_cb_preambles_per_ssb = 4;
};

} // namespace srsran