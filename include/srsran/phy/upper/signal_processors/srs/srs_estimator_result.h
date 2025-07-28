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

#include "srsran/phy/support/time_alignment_estimator/time_alignment_measurement.h"
#include "srsran/ran/srs/srs_channel_matrix.h"
#include <optional>

namespace srsran {

/// Sounding Reference Signals propagation channel estimation results.
struct srs_estimator_result {
  /// \brief Wideband estimated channel matrix.
  ///
  /// The channel matrix is normalized by the noise variance, so that its Frobenius norm square gives a rough estimate
  /// of the SNR for the sounding reference signals.
  /// \note The SNR inferred from the channel matrix is only meant as a qualitative indicator of the SRS reception and
  /// does not provide meaningful information about the achievable throughput.
  srs_channel_matrix channel_matrix;
  /// Wideband energy per resource element (EPRE), in decibel.
  std::optional<float> epre_dB;
  /// Wideband reference signal received power (RSRP), in decibel.
  std::optional<float> rsrp_dB;
  /// Wideband measured noise variance as a linear quantity.
  std::optional<float> noise_variance;
  /// Measured time alignment.
  time_alignment_measurement time_alignment;
};

} // namespace srsran
