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

namespace srsran {
namespace fapi {

class slot_time_message_notifier;
class slot_message_gateway;

/// Buffered decorator interface.
class buffered_decorator
{
public:
  virtual ~buffered_decorator() = default;

  /// Returns the slot message gateway of this decorator.
  virtual slot_message_gateway& get_slot_message_gateway() = 0;

  /// Returns the slot time message notifier of this decorator.
  virtual slot_time_message_notifier& get_slot_time_message_notifier() = 0;

  /// Configures the slot time notifier this buffered decorator will forward time message notifications.
  virtual void set_slot_time_notifier(slot_time_message_notifier& notifier) = 0;
};

} // namespace fapi
} // namespace srsran
