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

struct dl_tti_request_message;
struct tx_data_request_message;
struct ul_dci_request_message;
struct ul_tti_request_message;

/// This interface represents the gateway for sending FAPI slot based messages to the underlying PHY.
class slot_message_gateway
{
public:
  virtual ~slot_message_gateway() = default;

  /// \brief Sends a downlink TTI request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void dl_tti_request(const dl_tti_request_message& msg) = 0;

  /// \brief Sends an uplink TTI request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void ul_tti_request(const ul_tti_request_message& msg) = 0;

  /// \brief Sends an uplink DCI request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void ul_dci_request(const ul_dci_request_message& msg) = 0;

  /// \brief Sends a TX data request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void tx_data_request(const tx_data_request_message& msg) = 0;
};

} // namespace fapi
} // namespace srsran
