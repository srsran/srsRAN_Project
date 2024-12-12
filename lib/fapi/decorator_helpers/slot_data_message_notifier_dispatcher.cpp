/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "slot_data_message_notifier_dispatcher.h"
#include "slot_data_message_notifier_dummy.h"

using namespace srsran;
using namespace fapi;

static slot_data_message_notifier_dummy dummy_notifier;

slot_data_message_notifier_dispatcher::slot_data_message_notifier_dispatcher() : notifier(&dummy_notifier) {}

void slot_data_message_notifier_dispatcher::on_rx_data_indication(const rx_data_indication_message& msg)
{
  notifier->on_rx_data_indication(msg);
}

void slot_data_message_notifier_dispatcher::on_crc_indication(const crc_indication_message& msg)
{
  notifier->on_crc_indication(msg);
}

void slot_data_message_notifier_dispatcher::on_uci_indication(const uci_indication_message& msg)
{
  notifier->on_uci_indication(msg);
}

void slot_data_message_notifier_dispatcher::on_srs_indication(const srs_indication_message& msg)
{
  notifier->on_srs_indication(msg);
}

void slot_data_message_notifier_dispatcher::on_rach_indication(const rach_indication_message& msg)
{
  notifier->on_rach_indication(msg);
}

void slot_data_message_notifier_dispatcher::set_slot_data_message_notifier(slot_data_message_notifier& data_notifier)
{
  notifier = &data_notifier;
}
