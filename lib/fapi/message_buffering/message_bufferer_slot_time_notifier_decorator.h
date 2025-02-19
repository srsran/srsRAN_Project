/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
