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
