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

/// \file
/// \brief Amplitude controller generic interface.

#pragma once

#include "srsran/srsvec/types.h"

namespace srsran {

/// Power measurements and runtime metrics reported by the amplitude controller.
struct amplitude_controller_metrics {
  /// Average power after input gain and before clipping, normalized to full scale.
  float avg_power_fs;
  /// Peak power after input gain and before clipping, normalized to full scale.
  float peak_power_fs;
  /// PAPR of the input signal.
  float papr_lin;
  /// Gain factor applied to the signal by the amplitude control after the measurements.
  float gain_dB;
  /// Total number of samples processed by the clipping stage.
  uint64_t nof_processed_samples;
  /// Total number of samples clipped by the clipping stage.
  uint64_t nof_clipped_samples;
  /// Expected probability that an input sample is clipped, calculated as \c nof_clipped_samples /
  /// \c nof_processed_samples.
  long double clipping_probability;
  /// Indicates whether clipping is enabled in the amplitude controller.
  bool clipping_enabled;
};

/// \brief Describes an amplitude controller interface.
///
/// The amplitude controller adapts the amplitude of the signal to be injected to the radio device.
class amplitude_controller
{
public:
  /// Default destructor.
  virtual ~amplitude_controller() = default;

  /// \brief Performs amplitude control and provides signal power measurements.
  /// \param[out] output Output signal samples.
  /// \param[in] input Input signal samples.
  /// \return Amplitude controller metrics, including power measurements and clipping statistics.
  /// \note The output size and the input size must be equal.
  virtual amplitude_controller_metrics process(span<cf_t> output, span<const cf_t> input) = 0;
};

} // namespace srsran
