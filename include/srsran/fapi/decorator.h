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

#include <memory>

namespace srsran {
namespace fapi {

class slot_data_message_notifier;
class slot_error_message_notifier;
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
  slot_error_message_notifier& get_slot_error_message_notifier()
  {
    return next_decorator ? next_decorator->get_slot_error_message_notifier()
                          : get_slot_error_message_notifier_from_this_decorator();
  }

  /// Returns the slot time message notifier of this FAPI decorator.
  slot_time_message_notifier& get_slot_time_message_notifier()
  {
    return next_decorator ? next_decorator->get_slot_time_message_notifier()
                          : get_slot_time_message_notifier_from_this_decorator();
  }

  /// Sets the slot data message notifier of this FAPI decorator.
  virtual void set_slot_data_message_notifier(slot_data_message_notifier& notifier) = 0;

  /// Sets the slot error message notifier of this FAPI decorator.
  virtual void set_slot_error_message_notifier(slot_error_message_notifier& notifier) = 0;

  /// Sets the slot time message notifier of this FAPI decorator.
  virtual void set_slot_time_message_notifier(slot_time_message_notifier& notifier) = 0;

protected:
  /// Connects the next decorator notifiers to this FAPI decorator.
  void connect_notifiers()
  {
    next_decorator->set_slot_data_message_notifier(get_slot_data_message_notifier_from_this_decorator());
    next_decorator->set_slot_error_message_notifier(get_slot_error_message_notifier_from_this_decorator());
    next_decorator->set_slot_time_message_notifier(get_slot_time_message_notifier_from_this_decorator());
  }

private:
  /// Returns the slot data message notifier of this FAPI decorator.
  virtual slot_data_message_notifier& get_slot_data_message_notifier_from_this_decorator() = 0;

  /// Returns the slot error message notifier of this FAPI decorator.
  virtual slot_error_message_notifier& get_slot_error_message_notifier_from_this_decorator() = 0;

  /// Returns the slot time message notifier of this FAPI decorator.
  virtual slot_time_message_notifier& get_slot_time_message_notifier_from_this_decorator() = 0;

protected:
  std::unique_ptr<fapi_decorator> next_decorator = nullptr;
};

} // namespace fapi
} // namespace srsran
