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

#include "srsran/ran/slot_point.h"

namespace srsran {
namespace fapi {

/// \brief Slot-based, last message notifier interface.
///
/// This interface notifies, for each slot, the transmission of the last FAPI message that has been sent through the
/// FAPI message gateway.
class slot_last_message_notifier
{
public:
  virtual ~slot_last_message_notifier() = default;

  /// \brief Notifies the transmission of the last message corresponding to the given slot.
  ///
  /// \param[in] slot Current slot.
  virtual void on_last_message(slot_point slot) = 0;
};

} // namespace fapi
} // namespace srsran
