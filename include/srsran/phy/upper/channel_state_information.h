/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/ran/phy_time_unit.h"

namespace srsran {

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

} // namespace srsran
