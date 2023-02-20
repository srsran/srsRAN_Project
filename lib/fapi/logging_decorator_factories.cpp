/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi/logging_decorator_factories.h"
#include "logging_slot_data_notifier_decorator.h"
#include "logging_slot_gateway_decorator.h"
#include "logging_slot_time_noitifier_decorator.h"
#include "srsgnb/srslog/srslog.h"

using namespace srsran;
using namespace fapi;

std::unique_ptr<slot_message_gateway> logging_slot_gateway_decorator_factory::create(slot_message_gateway& gateway)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("FAPI", true);
  logger.set_level(srslog::basic_levels::debug);

  return std::make_unique<logging_slot_gateway_decorator>(logger, gateway);
}

std::unique_ptr<slot_data_message_notifier>
logging_slot_data_notifier_decorator_factory::create(slot_data_message_notifier& notifier)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("FAPI", true);
  logger.set_level(srslog::basic_levels::debug);

  return std::make_unique<logging_slot_data_notifier_decorator>(logger, notifier);
}

std::unique_ptr<slot_time_message_notifier>
logging_slot_time_notifier_decorator_factory::create(slot_time_message_notifier& notifier)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("FAPI", true);
  logger.set_level(srslog::basic_levels::debug);

  return std::make_unique<logging_slot_time_notifier_decorator>(logger, notifier);
}
