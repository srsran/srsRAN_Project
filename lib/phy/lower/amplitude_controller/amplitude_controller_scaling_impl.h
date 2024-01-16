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
/// \brief Amplitude controller implementation using scaling.

#pragma once

#include "srsran/phy/lower/amplitude_controller/amplitude_controller.h"
#include "srsran/support/math_utils.h"

namespace srsran {

/// Scaling amplitude controller implementation.
class amplitude_controller_scaling_impl : public amplitude_controller
{
private:
  /// Gain factor applied to the input signal.
  float amplitude_gain;

public:
  /// \brief Constructs an amplitude controller.
  /// \param[in] gain_dB  Sets the gain factor in dB applied to the input signal.
  amplitude_controller_scaling_impl(float gain_dB) : amplitude_gain(convert_dB_to_amplitude(gain_dB))
  {
    // Do nothing.
  }

  // See interface for documentation.
  amplitude_controller_metrics process(span<cf_t> output, span<const cf_t> input) override;
};

} // namespace srsran.
