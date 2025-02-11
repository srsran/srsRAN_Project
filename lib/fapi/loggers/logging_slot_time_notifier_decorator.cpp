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

#include "logging_slot_time_notifier_decorator.h"
#include "decorator_helpers/slot_time_notifier_dummy.h"
#include "message_loggers.h"

using namespace srsran;
using namespace fapi;

static slot_time_message_notifier_dummy dummy_notifier;

logging_slot_time_notifier_decorator::logging_slot_time_notifier_decorator(unsigned              sector_id_,
                                                                           srslog::basic_logger& logger_) :
  sector_id(sector_id_), logger(logger_), notifier(&dummy_notifier)
{
}

void logging_slot_time_notifier_decorator::on_slot_indication(const slot_indication_message& msg)
{
  log_slot_indication(msg, sector_id, logger);

  notifier->on_slot_indication(msg);
}

void logging_slot_time_notifier_decorator::set_slot_time_message_notifier(slot_time_message_notifier& time_notifier)
{
  notifier = &time_notifier;
}
