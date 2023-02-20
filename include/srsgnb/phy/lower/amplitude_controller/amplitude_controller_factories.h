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
/// \brief Amplitude controller factories.

#pragma once

#include "srsgnb/phy/lower/amplitude_controller/amplitude_controller.h"

namespace srsgnb {

/// Describes an amplitude controller configuration for the clipping implementation.
struct amplitude_controller_clipping_config {
  /// Enables or disables the clipping process.
  bool enable_clipping;
  /// Gain to be applied before the clipping process.
  float input_gain_dB;
  /// Signal amplitude that is mapped to the full scale output of the radio device.
  float full_scale_lin;
  /// Maximum signal amplitude in dB relative to full scale allowed at the output of the amplitude controller.
  float ceiling_dBFS;
};

/// Describes an amplitude controller factory.
class amplitude_controller_factory
{
public:
  /// Default destructor.
  virtual ~amplitude_controller_factory() = default;

  /// \brief Creates an amplitude controller.
  /// \return A unique pointer to an amplitude controller instance.
  virtual std::unique_ptr<amplitude_controller> create_amplitude_controller() = 0;
};

/// Creates a clipping amplitude controller factory.
std::shared_ptr<amplitude_controller_factory>
create_amplitude_controller_clipping_factory(const amplitude_controller_clipping_config& config);

} // namespace srsgnb
