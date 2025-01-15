/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "message_bufferer_slot_time_notifier_decorator.h"
#include "message_bufferer_slot_gateway_task_dispatcher.h"
#include "srsran/fapi/message_builders.h"
#include "srsran/fapi/messages.h"
#include "srsran/ran/slot_point.h"

using namespace srsran;
using namespace fapi;

namespace {

/// Dummy slot time message notifier.
class slot_time_message_notifier_dummy : public slot_time_message_notifier
{
public:
  // See interface for documentation.
  void on_slot_indication(const slot_indication_message& msg) override {}
};

} // namespace

static slot_time_message_notifier_dummy dummy_notifier;

message_bufferer_slot_time_notifier_decorator::message_bufferer_slot_time_notifier_decorator(
    unsigned                                       l2_nof_slots_ahead_,
    subcarrier_spacing                             scs_,
    message_bufferer_slot_gateway_task_dispatcher& gateway_task_dispatcher_) :
  l2_nof_slots_ahead(l2_nof_slots_ahead_),
  scs(scs_),
  gateway_task_dispatcher(gateway_task_dispatcher_),
  notifier(dummy_notifier)
{
}

void message_bufferer_slot_time_notifier_decorator::on_slot_indication(const slot_indication_message& msg)
{
  slot_point slot(scs, msg.sfn, msg.slot);

  // First update the current slot of the gateway task dispatcher.
  gateway_task_dispatcher.update_current_slot(slot);

  // Notify the upper layers.
  slot_point delayed_slot = slot + l2_nof_slots_ahead;
  notifier.get().on_slot_indication(build_slot_indication_message(delayed_slot.sfn(), delayed_slot.slot_index()));

  // Forward cached messages.
  gateway_task_dispatcher.forward_cached_messages(slot);
}
