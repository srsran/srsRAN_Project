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

#include <memory>

namespace srsran {
namespace fapi {

class slot_data_message_notifier;
class error_message_notifier;
class slot_last_message_notifier;
class slot_message_gateway;
class slot_time_message_notifier;

/// FAPI decorator interface.
class fapi_decorator
{
public:
  explicit fapi_decorator(std::unique_ptr<fapi_decorator> next_decorator_) : next_decorator(std::move(next_decorator_))
  {
  }

  virtual ~fapi_decorator() = default;

  /// Returns the slot last message notifier of this FAPI decorator.
  virtual slot_last_message_notifier& get_slot_last_message_notifier() = 0;

  /// Returns the slot message gateway of this FAPI decorator.
  virtual slot_message_gateway& get_slot_message_gateway() = 0;

  /// Returns the slot data message notifier of this FAPI decorator.
  slot_data_message_notifier& get_slot_data_message_notifier()
  {
    return next_decorator ? next_decorator->get_slot_data_message_notifier()
                          : get_slot_data_message_notifier_from_this_decorator();
  }

  /// Returns the slot error message notifier of this FAPI decorator.
  error_message_notifier& get_error_message_notifier()
  {
    return next_decorator ? next_decorator->get_error_message_notifier()
                          : get_error_message_notifier_from_this_decorator();
  }

  /// Returns the slot time message notifier of this FAPI decorator.
  slot_time_message_notifier& get_slot_time_message_notifier()
  {
    return next_decorator ? next_decorator->get_slot_time_message_notifier()
                          : get_slot_time_message_notifier_from_this_decorator();
  }

  /// Sets the slot data message notifier of this FAPI decorator.
  virtual void set_slot_data_message_notifier(slot_data_message_notifier& notifier) = 0;

  /// Sets the error message notifier of this FAPI decorator.
  virtual void set_error_message_notifier(error_message_notifier& notifier) = 0;

  /// Sets the slot time message notifier of this FAPI decorator.
  virtual void set_slot_time_message_notifier(slot_time_message_notifier& notifier) = 0;

protected:
  /// Connects the next decorator notifiers to this FAPI decorator.
  void connect_notifiers()
  {
    next_decorator->set_slot_data_message_notifier(get_slot_data_message_notifier_from_this_decorator());
    next_decorator->set_error_message_notifier(get_error_message_notifier_from_this_decorator());
    next_decorator->set_slot_time_message_notifier(get_slot_time_message_notifier_from_this_decorator());
  }

private:
  /// Returns the slot data message notifier of this FAPI decorator.
  virtual slot_data_message_notifier& get_slot_data_message_notifier_from_this_decorator() = 0;

  /// Returns the error message notifier of this FAPI decorator.
  virtual error_message_notifier& get_error_message_notifier_from_this_decorator() = 0;

  /// Returns the slot time message notifier of this FAPI decorator.
  virtual slot_time_message_notifier& get_slot_time_message_notifier_from_this_decorator() = 0;

protected:
  std::unique_ptr<fapi_decorator> next_decorator = nullptr;
};

} // namespace fapi
} // namespace srsran
