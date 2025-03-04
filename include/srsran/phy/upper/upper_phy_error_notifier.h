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

/// Upper physical layer error notifier.
class upper_phy_error_notifier
{
public:
  /// Default destructor.
  virtual ~upper_phy_error_notifier() = default;

  /// \brief Notifies a late downlink message.
  ///
  /// \param[in] dl_msg_slot Slot of the downlink message.
  virtual void on_late_downlink_message(slot_point dl_msg_slot) = 0;

  /// \brief Notifies a late uplink message.
  ///
  /// \param[in] ul_msg_slot Slot of the uplink message.
  virtual void on_late_uplink_message(slot_point ul_msg_slot) = 0;
};

} // namespace srsran
