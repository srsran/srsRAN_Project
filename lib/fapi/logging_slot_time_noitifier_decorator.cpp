/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "logging_slot_time_noitifier_decorator.h"
#include "message_loggers.h"

using namespace srsran;
using namespace fapi;

void logging_slot_time_notifier_decorator::on_slot_indication(const slot_indication_message& msg)
{
  log_slot_indication(msg, logger);

  notifier.on_slot_indication(msg);
}
