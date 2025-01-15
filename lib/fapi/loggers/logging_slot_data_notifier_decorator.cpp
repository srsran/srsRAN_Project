/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "logging_slot_data_notifier_decorator.h"
#include "decorator_helpers/slot_data_message_notifier_dummy.h"
#include "message_loggers.h"

using namespace srsran;
using namespace fapi;

static slot_data_message_notifier_dummy dummy_notifier;

logging_slot_data_notifier_decorator::logging_slot_data_notifier_decorator(srslog::basic_logger& logger_) :
  logger(logger_), notifier(&dummy_notifier)
{
}

void logging_slot_data_notifier_decorator::on_rx_data_indication(const rx_data_indication_message& msg)
{
  log_rx_data_indication(msg, logger);

  notifier->on_rx_data_indication(msg);
}

void logging_slot_data_notifier_decorator::on_crc_indication(const crc_indication_message& msg)
{
  log_crc_indication(msg, logger);

  notifier->on_crc_indication(msg);
}

void logging_slot_data_notifier_decorator::on_uci_indication(const uci_indication_message& msg)
{
  log_uci_indication(msg, logger);

  notifier->on_uci_indication(msg);
}

void logging_slot_data_notifier_decorator::on_srs_indication(const srs_indication_message& msg)
{
  notifier->on_srs_indication(msg);
}

void logging_slot_data_notifier_decorator::on_rach_indication(const rach_indication_message& msg)
{
  log_rach_indication(msg, logger);

  notifier->on_rach_indication(msg);
}

void logging_slot_data_notifier_decorator::set_slot_data_message_notifier(slot_data_message_notifier& data_notifier)
{
  notifier = &data_notifier;
}
