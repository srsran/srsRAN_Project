/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/fapi/logging_decorator_factories.h"
#include "logging_slot_data_notifier_decorator.h"
#include "logging_slot_error_notifier_decorator.h"
#include "logging_slot_gateway_decorator.h"
#include "logging_slot_time_notifier_decorator.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace fapi;

std::unique_ptr<slot_message_gateway> srsran::fapi::create_logging_slot_gateway(slot_message_gateway& gateway)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("FAPI", true);
  logger.set_level(srslog::basic_levels::debug);

  return std::make_unique<logging_slot_gateway_decorator>(logger, gateway);
}

std::unique_ptr<slot_time_message_notifier>
srsran::fapi::create_logging_slot_time_notifier(slot_time_message_notifier& notifier)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("FAPI", true);
  logger.set_level(srslog::basic_levels::debug);

  return std::make_unique<logging_slot_time_notifier_decorator>(logger, notifier);
}

std::unique_ptr<slot_error_message_notifier>
srsran::fapi::create_logging_slot_error_notifier(slot_error_message_notifier& notifier)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("FAPI", true);
  logger.set_level(srslog::basic_levels::debug);

  return std::make_unique<logging_slot_error_notifier_decorator>(logger, notifier);
}

std::unique_ptr<slot_data_message_notifier>
srsran::fapi::create_logging_slot_data_notifier(slot_data_message_notifier& notifier)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("FAPI", true);
  logger.set_level(srslog::basic_levels::debug);

  return std::make_unique<logging_slot_data_notifier_decorator>(logger, notifier);
}
