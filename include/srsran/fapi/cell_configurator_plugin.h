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

class config_message_notifier;
class slot_error_message_notifier;

/// Operation controller interface that allows to start/stop the cell configurator plugin.
class operation_controller
{
public:
  virtual ~operation_controller() = default;

  /// Starts the cell configurator plugin.
  virtual void start() = 0;

  /// Stops the cell configurator plugin.
  virtual void stop() = 0;
};

/// Cell configurator plugin interface.
class cell_configurator_plugin
{
public:
  virtual ~cell_configurator_plugin() = default;

  /// Returns the FAPI configuration message notifier of this plugin.
  virtual config_message_notifier& get_config_message_notifier() = 0;

  /// Returns the FAPI slot error message notifier of this plugin.
  virtual slot_error_message_notifier& get_slot_error_message_notifier() = 0;

  /// Returns the operation controller of this cell configurator plugin.
  virtual operation_controller& get_operation_controller() = 0;
};

} // namespace fapi
} // namespace srsran
