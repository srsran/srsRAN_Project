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
class error_message_notifier;
class slot_time_message_notifier;
class operation_controller;

/// \brief FAPI slot messages adaptor interface.
///
/// The FAPI slot messages adaptor will adapt slot related messages to FAPI slot messages. It adapts slot request
/// messages from the adaptor format to FAPI slot request and for indications it will adapt FAPI slot indications to the
/// adaptor format.
class slot_messages_adaptor
{
public:
  virtual ~slot_messages_adaptor() = default;

  /// Returns the operation controller of this adaptor.
  virtual operation_controller& get_operation_controller() = 0;

  /// Returns the slot data message notifier of this adaptor.
  virtual slot_data_message_notifier& get_slot_data_message_notifier() = 0;

  /// Returns the slot time message notifier of this adaptor.
  virtual slot_time_message_notifier& get_slot_time_message_notifier() = 0;

  /// Returns the FAPI error message notifier of this adaptor.
  virtual error_message_notifier& get_error_message_notifier() = 0;
};

} // namespace fapi
} // namespace srsran
