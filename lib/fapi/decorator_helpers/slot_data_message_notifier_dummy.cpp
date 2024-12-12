/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "slot_data_message_notifier_dummy.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace fapi;

void slot_data_message_notifier_dummy::on_rx_data_indication(const rx_data_indication_message& msg)
{
  report_error("Dummy FAPI slot data message notifier cannot handle given rx data indication");
}

void slot_data_message_notifier_dummy::on_crc_indication(const crc_indication_message& msg)
{
  report_error("Dummy FAPI slot data message notifier cannot handle given CRC indication");
}

void slot_data_message_notifier_dummy::on_uci_indication(const uci_indication_message& msg)
{
  report_error("Dummy FAPI slot data message notifier cannot handle given UCI indication");
}

void slot_data_message_notifier_dummy::on_srs_indication(const srs_indication_message& msg)
{
  report_error("Dummy FAPI slot data message notifier cannot handle given SRS indication");
}

void slot_data_message_notifier_dummy::on_rach_indication(const rach_indication_message& msg)
{
  report_error("Dummy FAPI slot data message notifier cannot handle given RACH indication");
}
