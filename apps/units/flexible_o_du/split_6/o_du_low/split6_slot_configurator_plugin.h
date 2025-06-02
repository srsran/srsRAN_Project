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

namespace srsran {

namespace fapi {

class slot_data_message_notifier;
class slot_error_message_notifier;
class slot_time_message_notifier;

} // namespace fapi

/// Split 6 slot configurator plugin.
class split6_slot_configurator_plugin
{
public:
  virtual ~split6_slot_configurator_plugin() = default;

  /// Returns the slot data message notifier of this split 6 slot configurator.
  virtual fapi::slot_data_message_notifier& get_slot_data_message_notifier() = 0;

  /// Returns the slot error message notifier of this split 6 slot configurator.
  virtual fapi::slot_error_message_notifier& get_error_message_notifier() = 0;

  /// Returns the slot time message notifier of this split 6 slot configurator.
  virtual fapi::slot_time_message_notifier& get_slot_time_message_notifier() = 0;
};

} // namespace srsran
