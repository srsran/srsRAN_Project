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

namespace srsran {
namespace fapi {

class slot_data_message_notifier;
class error_message_notifier;
class slot_time_message_notifier;

/// \brief FAPI slot messages adaptor interface.
///
/// The FAPI slot messages adaptor will adapt slot related messages to FAPI slot messages. It adapts slot request
/// messages from the adaptor format to FAPI slot request and for indications it will adapt FAPI slot indications to the
/// adaptor format.
class slot_messages_adaptor
{
public:
  virtual ~slot_messages_adaptor() = default;

  /// Returns the slot data message notifier of this adaptor.
  virtual slot_data_message_notifier& get_slot_data_message_notifier() = 0;

  /// Returns the slot time message notifier of this adaptor.
  virtual slot_time_message_notifier& get_slot_time_message_notifier() = 0;

  /// Returns the FAPI error message notifier of this adaptor.
  virtual error_message_notifier& get_error_message_notifier() = 0;
};

} // namespace fapi
} // namespace srsran
