/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split6_slot_configurator_plugin_dummy.h"
#include "srsran/fapi/messages/crc_indication.h"
#include "srsran/fapi/messages/error_indication.h"
#include "srsran/fapi/messages/rach_indication.h"
#include "srsran/fapi/messages/rx_data_indication.h"
#include "srsran/fapi/messages/srs_indication.h"
#include "srsran/fapi/messages/uci_indication.h"
#include "srsran/fapi/slot_data_message_notifier.h"
#include "srsran/fapi/slot_error_message_notifier.h"
#include "srsran/fapi/slot_time_message_notifier.h"

using namespace srsran;

namespace {

/// FAPI slot error message notifier dummy implementation.
class slot_error_message_notifier_dummy : public fapi::slot_error_message_notifier
{
public:
  void on_error_indication(const fapi::error_indication_message& msg) override {}
};

/// FAPI slot data message notifier dummy implementation.
class slot_data_message_notifier_dummy : public fapi::slot_data_message_notifier
{
public:
  void on_rx_data_indication(const fapi::rx_data_indication_message& msg) override {}
  void on_crc_indication(const fapi::crc_indication_message& msg) override {}
  void on_uci_indication(const fapi::uci_indication_message& msg) override {}
  void on_srs_indication(const fapi::srs_indication_message& msg) override {}
  void on_rach_indication(const fapi::rach_indication_message& msg) override {}
};

/// FAPI slot time message notifier dummy implementation.
class slot_time_message_notifier_dummy : public fapi::slot_time_message_notifier
{
public:
  void on_slot_indication(const fapi::slot_indication_message& msg) override {}
};

} // namespace

static slot_data_message_notifier_dummy  dummy_data_notifier;
static slot_time_message_notifier_dummy  dummy_time_notifier;
static slot_error_message_notifier_dummy dummy_error_notifier;

fapi::slot_data_message_notifier& split6_slot_configurator_plugin_dummy::get_slot_data_message_notifier()
{
  return dummy_data_notifier;
}

fapi::slot_error_message_notifier& split6_slot_configurator_plugin_dummy::get_error_message_notifier()
{
  return dummy_error_notifier;
}

fapi::slot_time_message_notifier& split6_slot_configurator_plugin_dummy::get_slot_time_message_notifier()
{
  return dummy_time_notifier;
}
