/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {
namespace fapi {

class slot_time_message_notifier;
class slot_message_gateway;

/// Buffered decorator interface.
class buffered_decorator
{
public:
  virtual ~buffered_decorator() = default;

  /// Returns the slot message gateway of this decorator.
  virtual slot_message_gateway& get_slot_message_gateway() = 0;

  /// Returns the slot time message notifier of this decorator.
  virtual slot_time_message_notifier& get_slot_time_message_notifier() = 0;

  /// Configures the slot time notifier this buffered decorator will forward time message notifications.
  virtual void set_slot_time_notifier(slot_time_message_notifier& notifier) = 0;
};

} // namespace fapi
} // namespace srsran
