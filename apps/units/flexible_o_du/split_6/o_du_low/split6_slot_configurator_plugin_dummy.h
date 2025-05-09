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

#include "split6_slot_configurator_plugin.h"

namespace srsran {

/// Split 6 slot configurator plugin dummy implementation.
class split6_slot_configurator_plugin_dummy : public split6_slot_configurator_plugin
{
public:
  // See interface for documentation.
  fapi::slot_data_message_notifier& get_slot_data_message_notifier() override;

  // See interface for documentation.
  fapi::slot_error_message_notifier& get_error_message_notifier() override;

  // See interface for documentation.
  fapi::slot_time_message_notifier& get_slot_time_message_notifier() override;
};

} // namespace srsran
