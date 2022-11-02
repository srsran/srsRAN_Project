/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "logging_slot_gateway_decorator.h"
#include "srsgnb/fapi/message_loggers.h"

using namespace srsgnb;
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
