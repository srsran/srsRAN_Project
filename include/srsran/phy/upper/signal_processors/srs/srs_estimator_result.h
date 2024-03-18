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

#include "srsran/ran/phy_time_unit.h"
#include "srsran/ran/srs/srs_channel_matrix.h"

namespace srsran {

/// Sounding Reference Signals propagation channel estimation results.
struct srs_estimator_result {
  /// Wideband estimated channel matrix.
  srs_channel_matrix channel_matrix;
  /// Wideband measured noise variance.
  float noise_variance;
  /// Measured time alignment.
  phy_time_unit time_align;
};

} // namespace srsran
