/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "logging_slot_data_notifier_decorator.h"
#include "srsgnb/fapi/message_loggers.h"

using namespace srsgnb;
using namespace fapi;

void logging_slot_data_notifier_decorator::on_dl_tti_response(const dl_tti_response_message& msg)
{
  notifier.on_dl_tti_response(msg);
}

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
