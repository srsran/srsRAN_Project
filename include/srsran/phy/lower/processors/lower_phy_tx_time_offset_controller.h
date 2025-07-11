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

#include "srsran/ran/phy_time_unit.h"
#include <chrono>

namespace srsran {

/// Lower physical layer transmit time offset controller interface.
class lower_phy_tx_time_offset_controller
{
public:
  /// Default destructor.
  virtual ~lower_phy_tx_time_offset_controller() = default;

  /// \brief Sets a transmit time offset.
  virtual void set_tx_time_offset(phy_time_unit tx_time_offset) = 0;
};

} // namespace srsran
