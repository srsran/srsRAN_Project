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

#include "srsran/fapi/slot_last_message_notifier.h"

namespace srsran {
namespace fapi {

/// Slot last message notifier dispatcher that forwards last message notifications to the configured notifier.
class slot_last_message_notifier_dispatcher : public slot_last_message_notifier
{
  slot_last_message_notifier* notifier = nullptr;

public:
  slot_last_message_notifier_dispatcher();

  // See interface for documentation.
  void on_last_message(slot_point slot) override;

  /// Sets the slot last message notifier to the given one.
  void set_slot_last_message_notifier(slot_last_message_notifier& last_msg_notifier);
};

} // namespace fapi
} // namespace srsran
