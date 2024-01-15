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

struct crc_indication_message;
struct dl_tti_response_message;
struct rach_indication_message;
struct rx_data_indication_message;
struct srs_indication_message;
struct uci_indication_message;

/// \brief Slot-based, data-specific message notifier interface.
///
/// This interface notifies the reception of slot-based, data-specific FAPI messages from the underlying PHY.
class slot_data_message_notifier
{
public:
  virtual ~slot_data_message_notifier() = default;

  /// \brief Notifies the reception of a RX data indication message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_rx_data_indication(const rx_data_indication_message& msg) = 0;

  /// \brief Notifies the reception of a CRC indication message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_crc_indication(const crc_indication_message& msg) = 0;

  /// \brief Notifies the reception of a UCI indication message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_uci_indication(const uci_indication_message& msg) = 0;

  /// \brief Notifies the reception of a SRS indication message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_srs_indication(const srs_indication_message& msg) = 0;

  /// \brief Notifies the reception of a RACH indication message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_rach_indication(const rach_indication_message& msg) = 0;
};

} // namespace fapi
} // namespace srsran
