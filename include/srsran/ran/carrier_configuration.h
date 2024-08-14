/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/nr_band.h"
#include <cstdint>

namespace srsran {

/// \brief Configuration of each transmission point associated to the corresponding cell(s). This includes
/// different physical antennas, different frequencies, bandwidths.
struct carrier_configuration {
  /// Width of this carrier in MHz. Values: 5, 10, 15, 20, 25, 30, 40, 50, 60, 70, 80, 90, 100, 200, 400.
  uint16_t carrier_bw_mhz;
  /// NR Absolute Radio Frequency Channel Number (NR-ARFCN) of "F_REF", which is the RF reference frequency, as per
  /// TS 38.104, Section 5.4.2.1 ("F_REF" maps to the central frequency of the band).
  uint32_t arfcn_f_ref;
  /// <em>NR operating band<\em>, as per Table 5.2-1 and 5.2-2. TS 38.104.
  nr_band band;
  /// Number of antennas. Values: (0..65355).
  uint16_t nof_ant;
};

} // namespace srsran
