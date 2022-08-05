/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsgnb {

namespace fapi {
class slot_data_message_notifier;
class slot_time_message_notifier;
} // namespace fapi

class mac_cell_result_notifier;
class mac_cell_slot_handler;

namespace fapi_adaptor {

/// \brief MAC&ndash;FAPI bidirectional adaptor interface.
///
/// This adaptor is a collection of interfaces to translate FAPI messages into their MAC layer counterpart and vice
/// versa.
///
/// \note All implementations of this public interface must hold the ownership of all its internal components.
class mac_fapi_adaptor
{
public:
  virtual ~mac_fapi_adaptor() = default;

  /// \brief Returns a reference to the adaptor slot data notifier.
  virtual fapi::slot_data_message_notifier& get_slot_data_notifier() = 0;

  /// \brief Returns a reference to the adaptor slot time notifier.
  virtual fapi::slot_time_message_notifier& get_slot_time_notifier() = 0;

  /// \brief Returns a reference to the adaptor MAC cell result notifier.
  virtual mac_cell_result_notifier& get_cell_result_notifier() = 0;

  /// \brief Configures the MAC cell slot handler to the given one.
  ///
  /// \param[in] mac_slot_handler MAC slot handler.
  virtual void set_cell_slot_handler(mac_cell_slot_handler& mac_slot_handler) = 0;

  // :TODO: add the rest of the MAC handlers.

  // :TODO: should provide access to the cell interface.
};

} // namespace fapi_adaptor
} // namespace srsgnb
