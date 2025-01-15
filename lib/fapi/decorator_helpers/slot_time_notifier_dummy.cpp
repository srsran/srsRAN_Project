/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "slot_time_notifier_dummy.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace fapi;

void slot_time_message_notifier_dummy::on_slot_indication(const slot_indication_message& msg)
{
  report_error("Dummy FAPI slot time message notifier cannot handle given slot indication");
}
