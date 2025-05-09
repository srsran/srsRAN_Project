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

namespace fapi {

class slot_data_message_notifier;
class slot_error_message_notifier;
class slot_time_message_notifier;

} // namespace fapi

/// Split 6 slot configurator plugin.
class split6_slot_configurator_plugin
{
public:
  virtual ~split6_slot_configurator_plugin() = default;

  /// Returns the slot data message notifier of this split 6 slot configurator.
  virtual fapi::slot_data_message_notifier& get_slot_data_message_notifier() = 0;

  /// Returns the slot error message notifier of this split 6 slot configurator.
  virtual fapi::slot_error_message_notifier& get_error_message_notifier() = 0;

  /// Returns the slot time message notifier of this split 6 slot configurator.
  virtual fapi::slot_time_message_notifier& get_slot_time_message_notifier() = 0;
};

} // namespace srsran
