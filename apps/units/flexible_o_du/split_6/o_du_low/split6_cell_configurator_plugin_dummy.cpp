/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
