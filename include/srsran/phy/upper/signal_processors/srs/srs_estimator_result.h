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
  /// Wideband estimated channel matrix.
  srs_channel_matrix channel_matrix;
  /// Wideband energy per resource element (EPRE), in decibel.
  std::optional<float> epre_dB;
  /// Wideband measured noise variance as a linear quantity.
  std::optional<float> noise_variance;
  /// Measured time alignment.
  time_alignment_measurement time_alignment;
};

} // namespace srsran
