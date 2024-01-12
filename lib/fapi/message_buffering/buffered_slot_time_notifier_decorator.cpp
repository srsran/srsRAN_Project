/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "buffered_slot_time_notifier_decorator.h"
#include "srsran/fapi/message_builders.h"
#include "srsran/fapi/messages.h"
#include "srsran/ran/slot_point.h"

using namespace srsran;
using namespace fapi;

void buffered_slot_time_notifier_decorator::on_slot_indication(const slot_indication_message& msg)
{
  slot_point slot(scs, msg.sfn, msg.slot);
  slot += l2_nof_slots_ahead;

  buffered_notifier.on_slot_indication(build_slot_indication_message(slot.sfn(), slot.slot_index()));

  notifier.on_slot_indication(msg);
}
