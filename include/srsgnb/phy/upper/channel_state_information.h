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

#include "srsgnb/ran/phy_time_unit.h"

namespace srsgnb {

/// Channel State Information from received DM-RS.
struct channel_state_information {
  /// Time alignment measurement.
  phy_time_unit time_alignment;
  /// Average EPRE in decibels.
  float epre_dB;
  /// Average RSRP in decibels.
  float rsrp_dB;
  /// Average SINR in decibels.
  float sinr_dB;
};

} // namespace srsgnb
