/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Amplitude controller implementation using scaling.

#pragma once

#include "srsran/phy/lower/amplitude_controller/amplitude_controller.h"
#include "srsran/support/math/math_utils.h"

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
