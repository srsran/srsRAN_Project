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

namespace srsran {

/// Lower physical layer carrier center frequency controller interface.
class lower_phy_center_freq_controller
{
public:
  /// Default destructor.
  virtual ~lower_phy_center_freq_controller() = default;

  /// Sets the carrier center frequency to the given value. Returns true on success, otherwise false.
  virtual bool set_carrier_center_frequency(double carrier_center_frequency_Hz) = 0;
};

} // namespace srsran
