/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/fapi/slot_time_message_notifier.h"
#include "srsran/ran/subcarrier_spacing.h"

namespace srsran {
namespace fapi {

/// Advances the slot indication message by the value configured in construction.
class buffered_slot_time_notifier_decorator : public slot_time_message_notifier
{
public:
  buffered_slot_time_notifier_decorator(unsigned                    l2_nof_slots_ahead_,
                                        subcarrier_spacing          scs_,
                                        slot_time_message_notifier& notifier_);

  // See interface for documentation.
  void on_slot_indication(const slot_indication_message& msg) override;

  // Configures the buffered notifier to the given one.
  void set_slot_time_notifier(slot_time_message_notifier& new_notifier) { buffered_notifier = std::ref(new_notifier); }

private:
  const unsigned                                     l2_nof_slots_ahead;
  const subcarrier_spacing                           scs;
  slot_time_message_notifier&                        notifier;
  std::reference_wrapper<slot_time_message_notifier> buffered_notifier;
};

} // namespace fapi
} // namespace srsran
