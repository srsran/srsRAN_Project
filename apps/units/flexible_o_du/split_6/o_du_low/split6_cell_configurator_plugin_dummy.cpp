/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split6_cell_configurator_plugin_dummy.h"
#include "srsran/fapi/config_message_notifier.h"
#include "srsran/fapi/slot_error_message_notifier.h"

using namespace srsran;

namespace {

/// FAPI slot error message notifier dummy implementation.
class slot_error_message_notifier_dummy : public fapi::slot_error_message_notifier
{
public:
  void on_error_indication(const fapi::error_indication_message& msg) override {}
};

/// FAPI config message notifier dummy implementation.
class config_message_notifier_dummy : public fapi::config_message_notifier
{
public:
  void on_param_response(const fapi::param_response& msg) override {}
  void on_config_response(const fapi::config_response& msg) override {}
  void on_stop_indication(const fapi::stop_indication& msg) override {}
};

} // namespace

static config_message_notifier_dummy     dummy_config_notifier;
static slot_error_message_notifier_dummy dummy_slot_error_notifier;

fapi::config_message_notifier& split6_cell_configurator_plugin_dummy::get_config_message_notifier()
{
  return dummy_config_notifier;
}

fapi::slot_error_message_notifier& split6_cell_configurator_plugin_dummy::get_slot_error_message_notifier()
{
  return dummy_slot_error_notifier;
}
