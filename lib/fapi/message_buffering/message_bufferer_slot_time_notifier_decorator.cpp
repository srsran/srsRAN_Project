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

#include "message_bufferer_slot_time_notifier_decorator.h"
#include "message_bufferer_slot_gateway_task_dispatcher.h"
#include "srsran/fapi/message_builders.h"
#include "srsran/fapi/messages/slot_indication.h"
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
  scs(scs_),
  l2_nof_slots_ahead(l2_nof_slots_ahead_),
  l2_nof_slots_ahead_ns(l2_nof_slots_ahead * 1000000 / slot_point(scs, 0).nof_slots_per_subframe()),
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
  notifier.get().on_slot_indication(build_slot_indication_message(
      delayed_slot.sfn(), delayed_slot.slot_index(), msg.time_point + l2_nof_slots_ahead_ns));

  // Forward cached messages.
  gateway_task_dispatcher.forward_cached_messages(slot);
}
