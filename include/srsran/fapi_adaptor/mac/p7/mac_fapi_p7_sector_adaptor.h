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
} // namespace fapi

namespace fapi_adaptor {

/// \brief MAC-FAPI bidirectional P7 adaptor sector interface.
///
/// This adaptor is a collection of interfaces to get the FAPI message notifiers from the MAC-FAPI sector adaptor
/// to connect them to the PHY-FAPI sector adaptor, so this sector adaptor receives FAPI notifications.
class mac_fapi_p7_sector_adaptor
{
public:
  virtual ~mac_fapi_p7_sector_adaptor() = default;

  /// Returns a reference to the slot time notifier used by the adaptor.
  virtual fapi::slot_time_message_notifier& get_slot_time_message_notifier() = 0;

  /// Returns a reference to the error notifier used by the adaptor.
  virtual fapi::error_message_notifier& get_error_message_notifier() = 0;

  /// Returns a reference to the slot data notifier used by the adaptor.
  virtual fapi::slot_data_message_notifier& get_slot_data_message_notifier() = 0;
};

} // namespace fapi_adaptor
} // namespace srsran
