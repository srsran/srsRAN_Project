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
class slot_last_message_notifier;
class slot_message_gateway;
class slot_time_message_notifier;
} // namespace fapi

namespace fapi_adaptor {

/// \brief PHY-FAPI bidirectional P7 sector adaptor interface.
///
/// This adaptor is a collection of interfaces to connect a P7 MAC-FAPI sector adaptor to a PHY-FAPI P7 sector adaptor.
class phy_fapi_p7_sector_adaptor
{
public:
  virtual ~phy_fapi_p7_sector_adaptor() = default;

  /// Returns a reference to the slot-based message gateway used by the adaptor.
  virtual fapi::slot_message_gateway& get_slot_message_gateway() = 0;

  /// Returns a reference to the slot-based last message notifier used by the adaptor.
  virtual fapi::slot_last_message_notifier& get_slot_last_message_notifier() = 0;

  /// Configures the slot-based, time-specific message notifier to the given one.
  virtual void set_slot_time_message_notifier(fapi::slot_time_message_notifier& fapi_time_notifier) = 0;

  /// Configures the error-specific message notifier to the given one.
  virtual void set_error_message_notifier(fapi::error_message_notifier& fapi_error_notifier) = 0;

  /// Configures the slot-based, data-specific message notifier to the given one.
  virtual void set_slot_data_message_notifier(fapi::slot_data_message_notifier& fapi_data_notifier) = 0;
};

} // namespace fapi_adaptor
} // namespace srsran
