/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "logging_slot_error_notifier_decorator.h"
#include "message_loggers.h"

using namespace srsran;
using namespace fapi;

void logging_slot_error_notifier_decorator::on_error_indication(const error_indication_message& msg)
{
  log_error_indication(msg, logger);

  notifier.on_error_indication(msg);
}
