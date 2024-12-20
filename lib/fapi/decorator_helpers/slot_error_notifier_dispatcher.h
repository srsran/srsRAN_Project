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

#include "srsran/fapi//slot_error_message_notifier.h"

namespace srsran {
namespace fapi {

/// Slot error message notifier dispatcher that forwards wrror messages to the configured notifier.
class slot_error_message_notifier_dispatcher : public slot_error_message_notifier
{
  slot_error_message_notifier* notifier = nullptr;

public:
  slot_error_message_notifier_dispatcher();

  // See interface for documentation.
  void on_error_indication(const error_indication_message& msg) override;

  /// Sets the slot error message notifier to the given one.
  void set_slot_error_message_notifier(slot_error_message_notifier& error_notifier);
};

} // namespace fapi
} // namespace srsran