/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/static_vector.h"
#include "srsran/ran/csi_rs/csi_rs_constants.h"

namespace srsran {

/// CSI-RS types.
enum class csi_rs_type : uint8_t {
  /// Tracking Reference signals.
  TRS,
  /// Non-Zero-Power CSI Reference Signal.
  CSI_RS_NZP,
  /// Zero-Power CSI Reference Signal.
  CSI_RS_ZP
};

/// Code-Division Multiplexing (CDM) strategies for CSI-RS.
enum class csi_rs_cdm_type {
  /// No CDM, i.e. CDM group of size 1.
  no_CDM = 0,
  /// Frequency-domain CDM group of size 2.
  fd_CDM2,
  /// CDM group of size 4 (two frequency-domain locations and two time-domain locations).
  cdm4_FD2_TD2,
  /// CDM group of size 8 (two frequency-domain locations and four time-domain locations).
  cdm8_FD2_TD4
};

/// Strategies regarding Resource Element (RE) density in the frequency domain for CSI-RS signals.
enum class csi_rs_freq_density_type {
  /// One RE occupied in even-numbered PRB.
  dot5_even_RB = 0,
  /// One RE occupied in odd-numbered PRB.
  dot5_odd_RB,
  /// One RE occupied in every PRB.
  one,
  /// Three RE occupied in every PRB.
  three
};

/// Computes the CDM group size.
static constexpr unsigned get_cdm_group_size(csi_rs_cdm_type cdm)
{
  // Determine the CDM group size.
  unsigned cdm_group_size = 1;
  if (cdm == csi_rs_cdm_type::fd_CDM2) {
    cdm_group_size = 2;
  } else if (cdm == csi_rs_cdm_type::cdm4_FD2_TD2) {
    cdm_group_size = 4;
  } else if (cdm == csi_rs_cdm_type::cdm8_FD2_TD4) {
    cdm_group_size = 8;
  }
  return cdm_group_size;
}

} // namespace srsran