/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "error_notifier_dispatcher.h"
#include "error_notifier_dummy.h"

using namespace srsran;
using namespace fapi;

static error_message_notifier_dummy dummy_notifier;

error_message_notifier_dispatcher::error_message_notifier_dispatcher() : notifier(&dummy_notifier) {}

void error_message_notifier_dispatcher::on_error_indication(const error_indication_message& msg)
{
  notifier->on_error_indication(msg);
}

void error_message_notifier_dispatcher::set_error_message_notifier(error_message_notifier& error_notifier)
{
  notifier = &error_notifier;
}
