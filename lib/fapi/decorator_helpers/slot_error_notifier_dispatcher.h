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

#include "srsran/fapi//slot_error_message_notifier.h"

namespace srsran {
namespace fapi {

/// Slot error message notifier dispatcher that forwards wrror messages to the configured notifier.
class slot_error_message_notifier_dispatcher : public slot_error_message_notifier
{
  slot_error_message_notifier* notifier = nullptr;

public:
  slot_error_message_notifier_dispatcher();

  // See interface for documentation.
  void on_error_indication(const error_indication_message& msg) override;

  /// Sets the slot error message notifier to the given one.
  void set_slot_error_message_notifier(slot_error_message_notifier& error_notifier);
};

} // namespace fapi
} // namespace srsran
