/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

namespace srsran {
namespace fapi {

/// Advances the slot indication message by the value configured in construction.
class buffered_slot_time_notifier_decorator : public slot_time_message_notifier
{
public:
  buffered_slot_time_notifier_decorator(unsigned                    l2_nof_slots_ahead_,
                                        subcarrier_spacing          scs_,
                                        slot_time_message_notifier& notifier_,
                                        slot_time_message_notifier& buffered_notifier_) :
    l2_nof_slots_ahead(l2_nof_slots_ahead_), scs(scs_), notifier(notifier_), buffered_notifier(buffered_notifier_)
  {
  }

  // See interface for documentation.
  void on_slot_indication(const slot_indication_message& msg) override;

private:
  const unsigned              l2_nof_slots_ahead;
  const subcarrier_spacing    scs;
  slot_time_message_notifier& notifier;
  slot_time_message_notifier& buffered_notifier;
};

} // namespace fapi
} // namespace srsran
