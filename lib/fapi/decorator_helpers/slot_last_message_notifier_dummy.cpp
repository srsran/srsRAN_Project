/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "slot_last_message_notifier_dummy.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace fapi;

void slot_last_message_notifier_dummy::on_last_message(slot_point slot)
{
  report_error("Dummy FAPI slot last message notifier cannot handle given last message notification");
}
