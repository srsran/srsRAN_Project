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
class slot_data_message_notifier;
class slot_error_message_notifier;
class slot_time_message_notifier;
} // namespace fapi

class mac_cell_control_information_handler;
class mac_cell_rach_handler;
class mac_cell_result_notifier;
class mac_cell_slot_handler;
class mac_pdu_handler;

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

  /// \brief Returns a reference to the slot time notifier used by the adaptor.
  virtual fapi::slot_time_message_notifier& get_slot_time_notifier() = 0;

  /// \brief Returns a reference to the slot error notifier used by the adaptor.
  virtual fapi::slot_error_message_notifier& get_slot_error_notifier() = 0;

  /// \brief Returns a reference to the slot data notifier used by the adaptor.
  virtual fapi::slot_data_message_notifier& get_slot_data_notifier() = 0;

  /// \brief Returns a reference to the MAC cell results notifier used by the adaptor.
  virtual mac_cell_result_notifier& get_cell_result_notifier() = 0;

  /// \brief Configures the MAC cell slot handler to the given one.
  virtual void set_cell_slot_handler(mac_cell_slot_handler& mac_slot_handler) = 0;

  /// \brief Configures the MAC cell RACH handler to the given one.
  virtual void set_cell_rach_handler(mac_cell_rach_handler& mac_rach_handler) = 0;

  /// \brief Configures the MAC cell PDU handler to the given one.
  virtual void set_cell_pdu_handler(mac_pdu_handler& handler) = 0;

  /// \brief Configures the MAC cell CRC handler to the given one.
  virtual void set_cell_crc_handler(mac_cell_control_information_handler& handler) = 0;
};

} // namespace fapi_adaptor
} // namespace srsran
