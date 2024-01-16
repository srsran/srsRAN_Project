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
/// \brief Amplitude controller factories.

#pragma once

#include "srsran/phy/lower/amplitude_controller/amplitude_controller.h"

namespace srsran {

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
  virtual std::unique_ptr<amplitude_controller> create() = 0;
};

/// Creates a clipping amplitude controller factory.
std::shared_ptr<amplitude_controller_factory>
create_amplitude_controller_clipping_factory(const amplitude_controller_clipping_config& config);

/// Creates a scaling amplitude controller factory.
std::shared_ptr<amplitude_controller_factory> create_amplitude_controller_scaling_factory(float gain_dB_);

} // namespace srsran
