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

#pragma once

#include "srsran/fapi/slot_time_message_notifier.h"
#include "srsran/ran/subcarrier_spacing.h"
#include <chrono>
#include <functional>

namespace srsran {
namespace fapi {

class message_bufferer_slot_gateway_task_dispatcher;

/// Advances the slot indication message by the value configured in construction.
class message_bufferer_slot_time_notifier_decorator : public slot_time_message_notifier
{
public:
  message_bufferer_slot_time_notifier_decorator(
      unsigned                                       l2_nof_slots_ahead_,
      subcarrier_spacing                             scs_,
      message_bufferer_slot_gateway_task_dispatcher& gateway_task_dispatcher_);

  // See interface for documentation.
  void on_slot_indication(const slot_indication_message& msg) override;

  // Configures the message bufferer notifier to the given one.
  void set_slot_time_notifier(slot_time_message_notifier& time_notifier) { notifier = std::ref(time_notifier); }

private:
  const subcarrier_spacing                           scs;
  const unsigned                                     l2_nof_slots_ahead;
  const std::chrono::nanoseconds                     l2_nof_slots_ahead_ns;
  message_bufferer_slot_gateway_task_dispatcher&     gateway_task_dispatcher;
  std::reference_wrapper<slot_time_message_notifier> notifier;
};

} // namespace fapi
} // namespace srsran
