/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/lower/amplitude_controller/amplitude_controller.h"
#include "srsgnb/phy/lower/amplitude_controller/amplitude_controller_factories.h"
#include "srsgnb/support/math_utils.h"

namespace srsgnb {

/// Generic amplitude controller implementation.
class amplitude_controller_impl : public amplitude_controller
{
private:
  /// Enables or disables the clipping process.
  bool clipping_enabled;
  /// Sample magnitude corresponding to the radio device's full scale value.
  float full_scale_lin;
  /// Maximum amplitude in dB relative to full scale.
  float ceiling_dBfs;
  /// Maximum amplitude in linear form.
  float ceiling_lin;
  /// Amplitude scaling factor used to process the signal.
  float gain_lin;
  /// Runtime metrics reported by the amplitude controller.
  amplitude_controller_metrics metrics = {};

public:
  /// Constructs an amplitude controller.
  /// \param[in] config Provides the amplitude controller configuration parameters.
  explicit amplitude_controller_impl(const amplitude_controller_configuration& config) :
    clipping_enabled(config.enable_clipping),
    full_scale_lin(config.full_scale_lin),
    ceiling_dBfs(config.ceiling_dBFS),
    ceiling_lin(full_scale_lin * convert_dB_to_amplitude(ceiling_dBfs)),
    gain_lin(convert_dB_to_amplitude(config.input_gain_dB))
  {
    // Do nothing.
  }

  /// See interface for documentation.
  amplitude_controller_metrics process(span<cf_t> output, span<const cf_t> input) override;

  /// See interface for documentation.
  const amplitude_controller_metrics get_metrics() const override { return metrics; }
};

} // namespace srsgnb.
