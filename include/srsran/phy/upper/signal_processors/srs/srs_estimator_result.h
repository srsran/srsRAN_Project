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
