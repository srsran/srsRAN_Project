/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "logging_slot_time_notifier_decorator.h"
#include "decorator_helpers/slot_time_notifier_dummy.h"
#include "message_loggers.h"

using namespace srsran;
using namespace fapi;

static slot_time_message_notifier_dummy dummy_notifier;

logging_slot_time_notifier_decorator::logging_slot_time_notifier_decorator(srslog::basic_logger& logger_) :
  logger(logger_), notifier(&dummy_notifier)
{
}

void logging_slot_time_notifier_decorator::on_slot_indication(const slot_indication_message& msg)
{
  log_slot_indication(msg, logger);

  notifier->on_slot_indication(msg);
}

void logging_slot_time_notifier_decorator::set_slot_time_message_notifier(slot_time_message_notifier& time_notifier)
{
  notifier = &time_notifier;
}
