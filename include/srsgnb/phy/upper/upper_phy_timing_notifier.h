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

#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Upper physical layer timing notifier.
class upper_phy_timing_notifier
{
public:
  virtual ~upper_phy_timing_notifier() = default;

  /// \brief Notifies a new TTI boundary event.
  ///
  /// \param[in] slot Notification context.
  virtual void on_tti_boundary(slot_point slot) = 0;
};

} // namespace srsgnb
