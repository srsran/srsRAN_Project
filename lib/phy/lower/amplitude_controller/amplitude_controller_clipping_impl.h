/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Amplitude controller implementation using clipping.

#pragma once

#include "srsran/phy/lower/amplitude_controller/amplitude_controller.h"
#include "srsran/support/math_utils.h"

namespace srsran {

/// Clipping amplitude controller implementation.
class amplitude_controller_clipping_impl : public amplitude_controller
{
private:
  /// Enables or disables the clipping process.
  bool clipping_enabled;
  /// Gain factor applied to the input signal, before the clipping process.
  float amplitude_gain;
  /// Squared value of the amplitude corresponding to the radio device's full scale power.
  float full_scale_pwr;
  /// Maximum amplitude allowed by the clipping.
  float ceiling_lin;
  /// Runtime metrics reported by the amplitude controller.
  amplitude_controller_metrics metrics = {};

public:
  /// \brief Constructs an amplitude controller.
  /// \param[in] enable_clipping Enables or disables the clipping process.
  /// \param[in] input_gain_dB  Sets the gain factor in dB applied to the input signal.
  /// \param[in] full_scale_lin Sets the full scale amplitude reference used to determine the amplitude ceiling.
  /// \param[in] ceiling_dBFS Sets the clipping amplitude ceiling in dB, relative to the full scale amplitude.
  amplitude_controller_clipping_impl(bool  enable_clipping,
                                     float input_gain_dB,
                                     float full_scale_lin,
                                     float ceiling_dBFS) :
    clipping_enabled(enable_clipping),
    amplitude_gain(convert_dB_to_amplitude(input_gain_dB)),
    full_scale_pwr(full_scale_lin * full_scale_lin),
    ceiling_lin(full_scale_lin * convert_dB_to_amplitude(ceiling_dBFS))
  {
    // Do nothing.
  }

  /// See interface for documentation.
  amplitude_controller_metrics process(span<cf_t> output, span<const cf_t> input) override;
};

} // namespace srsran.
