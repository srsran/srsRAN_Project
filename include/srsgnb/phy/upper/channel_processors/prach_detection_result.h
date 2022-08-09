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

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/phy_time_unit.h"

namespace srsgnb {

/// Describes a PRACH detection result.
struct prach_detection_result {
  /// Maximum number of preambles that can be detected in a slot.
  static constexpr unsigned MAX_NOF_PREAMBLES_PER_SLOT = 16;

  /// Describes the detection of a single preamble.
  struct preamble_indication {
    /// Index of the detected preamble. Possible values are {0, ..., 63}.
    unsigned preamble_index;
    /// Timing advance between the observed arrival time (for the considered UE) and the reference uplink time.
    phy_time_unit time_advance;
    /// Average RSRP value in dB.
    float power_dB;
    /// Average SNR value in dB.
    float snr_dB;
  };

  /// Average RSSI value in dB.
  float rssi_dB;
  /// \brief Detector time resolution.
  ///
  /// This is equal to the PRACH subcarrier spacing divided by the DFT size of the detector.
  phy_time_unit time_resolution;
  /// List of detected preambles.
  static_vector<preamble_indication, MAX_NOF_PREAMBLES_PER_SLOT> preambles;
};

} // namespace srsgnb
