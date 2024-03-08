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

namespace srsran {

/// Collects the time alignment measurement metrics.
struct time_alignment_measurement {
  /// Actual time alignment measurement in seconds.
  double time_alignment;
  /// Time measurement resolution in seconds.
  double resolution;
  /// Minimum measurement.
  double min;
  /// Maximum measurement.
  double max;
};

} // namespace srsran