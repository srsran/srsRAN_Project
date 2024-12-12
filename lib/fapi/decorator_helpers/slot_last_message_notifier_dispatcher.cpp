/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "slot_last_message_notifier_dispatcher.h"
#include "slot_last_message_notifier_dummy.h"

using namespace srsran;
using namespace fapi;

static slot_last_message_notifier_dummy dummy_notifier;

slot_last_message_notifier_dispatcher::slot_last_message_notifier_dispatcher() : notifier(&dummy_notifier) {}

void slot_last_message_notifier_dispatcher::on_last_message(slot_point slot)
{
  notifier->on_last_message(slot);
}

void slot_last_message_notifier_dispatcher::set_slot_last_message_notifier(
    slot_last_message_notifier& last_msg_notifier)
{
  notifier = &last_msg_notifier;
}
