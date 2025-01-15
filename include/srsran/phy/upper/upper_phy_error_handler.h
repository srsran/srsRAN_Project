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

#include "srsran/ran/slot_point.h"

namespace srsran {

/// Upper physical layer error handler.
class upper_phy_error_handler
{
public:
  /// Default destructor.
  virtual ~upper_phy_error_handler() = default;

  /// \brief Handles a late downlink message.
  ///
  /// \param[in] slot Slot of the downlink message.
  virtual void handle_late_downlink_message(slot_point slot) = 0;
};

} // namespace srsran
