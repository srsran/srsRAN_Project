/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "logging_slot_error_notifier_decorator.h"
#include "decorator_helpers/slot_error_notifier_dummy.h"
#include "message_loggers.h"

using namespace srsran;
using namespace fapi;

static slot_error_message_notifier_dummy dummy_notifier;

logging_slot_error_notifier_decorator::logging_slot_error_notifier_decorator(unsigned              sector_id_,
                                                                             srslog::basic_logger& logger_) :
  sector_id(sector_id_), logger(logger_), notifier(&dummy_notifier)
{
}

void logging_slot_error_notifier_decorator::on_error_indication(const error_indication_message& msg)
{
  log_error_indication(msg, sector_id, logger);

  notifier->on_error_indication(msg);
}

void logging_slot_error_notifier_decorator::set_slot_error_message_notifier(slot_error_message_notifier& error_notifier)
{
  notifier = &error_notifier;
}
