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

#include "logging_slot_gateway_decorator.h"
#include "message_loggers.h"

using namespace srsran;
using namespace fapi;

void logging_slot_gateway_decorator::dl_tti_request(const dl_tti_request_message& msg)
{
  log_dl_tti_request(msg, logger);

  gateway.dl_tti_request(msg);
}

void logging_slot_gateway_decorator::ul_tti_request(const ul_tti_request_message& msg)
{
  log_ul_tti_request(msg, logger);

  gateway.ul_tti_request(msg);
}

void logging_slot_gateway_decorator::ul_dci_request(const ul_dci_request_message& msg)
{
  log_ul_dci_request(msg, logger);

  gateway.ul_dci_request(msg);
}

void logging_slot_gateway_decorator::tx_data_request(const tx_data_request_message& msg)
{
  log_tx_data_request(msg, logger);

  gateway.tx_data_request(msg);
}
