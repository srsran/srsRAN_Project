/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsgnb {
namespace fapi {

struct slot_indication_message;

/// \brief Slot-based, time-specific message notifier interface.
///
/// This interface notifies the reception of slot-based, time-specific FAPI messages from the underlying PHY.
class slot_time_message_notifier
{
public:
  virtual ~slot_time_message_notifier() = default;

  /// \brief Notifies the reception of a slot indication message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_slot_indication(const slot_indication_message& msg) = 0;
};

} // namespace fapi
} // namespace srsgnb
