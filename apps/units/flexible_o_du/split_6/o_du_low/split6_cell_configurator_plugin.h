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

} // namespace fapi

/// Split 6 cell configurator plugin interface.
class split6_cell_configurator_plugin
{
public:
  virtual ~split6_cell_configurator_plugin() = default;

  /// Returns the FAPI configuration message notifier of this plugin.
  virtual fapi::config_message_notifier& get_config_message_notifier() = 0;

  /// Returns the FAPI slot error message notifier of this plugin.
  virtual fapi::slot_error_message_notifier& get_slot_error_message_notifier() = 0;
};

} // namespace srsran
