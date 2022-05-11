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

#ifndef SRSGNB_SSB_CONFIGURATION_H
#define SRSGNB_SSB_CONFIGURATION_H

#include "srsgnb/ran/ssb_mapping.h"
#include "srsgnb/ran/subcarrier_spacing.h"

namespace srsgnb {

/// Maximum number of beams.
/// \remark See O-RAN WG8-v05, Section 9.2.1.1, Table 9-3 SSB Configuration.
const size_t NOF_BEAMS = 64;

/// SSB periodicity in msec.
/// \remark Refer to ssb-periodicityServingCell, TS 38.331
enum class ssb_periodicity { ms5, ms10, ms20, ms40, ms80, ms160 };

/// SSB Configuration.
/// \remark See O-RAN WG8, Section 9.2.1.1, Table 9-3.
struct ssb_configuration {
  /// ORAN defined parameters.
  /// SSB block power value: (-60dBm, ..., 50dBm) [Table 3-39, FAPI and ss-PBCH-BlockPower in TS 38.311].
  int8_t ssb_power;
  /// SSB subcarrier spacing [Table 4.2-1, TS 38.211].
  // TODO: verify if which SCS this refers to.
  subcarrier_spacing scs;
  /// Given in PRBs - Values [0, 2199] - [Table 3-79, FAPI].
  uint16_t ssb_offset_to_point_A;
  /// Values: [0: 5ms, 1: 10ms, 2: 20ms, 3: 40ms, 4: 80ms, 5: ms160].
  uint8_t ssb_period;
  /// SSB SubcarrierOffest or k_ssb - Values [0, 31].
  /// NOTE: According to TS 38.211, Section 7.4.3.1, values are [0, 23], but ORAN uses [0, 31].
  uint8_t ssb_subcarrier_offset;
  /// This ssb-PositionsInBurs in the TS 38.311, 0 = SSB beam not used, 1 = SSB beam used.
  uint64_t ssb_bitmap;
  /// The n-th element of the array indicates what Beam ID to use for the n-th SSB occasion in ssb_bitmap. Only relevant
  /// if n-th bit of ssb_bitmap is 1.
  std::array<uint8_t, NOF_BEAMS> beam_ids;
  /// Values: 0 = 0dB, 1 = 3dB.
  uint8_t beta_pss;
};

} // namespace srsgnb

#endif // SRSGNB_SSB_CONFIGURATION_H
