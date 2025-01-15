/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "slot_error_notifier_dummy.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

void fapi::slot_error_message_notifier_dummy::on_error_indication(const error_indication_message& msg)
{
  report_error("Dummy FAPI slot error message notifier cannot handle given error indication");
}
