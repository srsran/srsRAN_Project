/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "slot_error_notifier_dispatcher.h"
#include "slot_error_notifier_dummy.h"

using namespace srsran;
using namespace fapi;

static slot_error_message_notifier_dummy dummy_notifier;

slot_error_message_notifier_dispatcher::slot_error_message_notifier_dispatcher() : notifier(&dummy_notifier) {}

void slot_error_message_notifier_dispatcher::on_error_indication(const error_indication_message& msg)
{
  notifier->on_error_indication(msg);
}

void slot_error_message_notifier_dispatcher::set_slot_error_message_notifier(
    slot_error_message_notifier& error_notifier)
{
  notifier = &error_notifier;
}
