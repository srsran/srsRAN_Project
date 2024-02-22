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

#include "logging_slot_data_notifier_decorator.h"
#include "message_loggers.h"

using namespace srsran;
using namespace fapi;

void logging_slot_data_notifier_decorator::on_rx_data_indication(const rx_data_indication_message& msg)
{
  log_rx_data_indication(msg, logger);

  notifier.on_rx_data_indication(msg);
}

void logging_slot_data_notifier_decorator::on_crc_indication(const crc_indication_message& msg)
{
  log_crc_indication(msg, logger);

  notifier.on_crc_indication(msg);
}

void logging_slot_data_notifier_decorator::on_uci_indication(const uci_indication_message& msg)
{
  log_uci_indication(msg, logger);

  notifier.on_uci_indication(msg);
}

void logging_slot_data_notifier_decorator::on_srs_indication(const srs_indication_message& msg)
{
  notifier.on_srs_indication(msg);
}

void logging_slot_data_notifier_decorator::on_rach_indication(const rach_indication_message& msg)
{
  log_rach_indication(msg, logger);

  notifier.on_rach_indication(msg);
}
