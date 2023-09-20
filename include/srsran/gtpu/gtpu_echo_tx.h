/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include <stdint.h>

struct sockaddr_storage;

namespace srsran {

/// This interface represents the data entry point of the transmitting side of a GTP-U echo.
/// The other-layers will use this interface to trigger echo requests or to reply with echo responses.
class gtpu_echo_tx_interface
{
public:
  gtpu_echo_tx_interface()                                          = default;
  virtual ~gtpu_echo_tx_interface()                                 = default;
  gtpu_echo_tx_interface(const gtpu_echo_tx_interface&)             = delete;
  gtpu_echo_tx_interface& operator=(const gtpu_echo_tx_interface&)  = delete;
  gtpu_echo_tx_interface(const gtpu_echo_tx_interface&&)            = delete;
  gtpu_echo_tx_interface& operator=(const gtpu_echo_tx_interface&&) = delete;

  /// Sends an echo request message to the \c dst_addr.
  /// \param dst_addr The destination address for the echo request message.
  virtual void send_echo_request(const sockaddr_storage& dst_addr) = 0;

  /// Sends an echo response message to the \c dst_addr using sequence number \c sn.
  /// The sequence number shall be copied from the echo request as specified in TS 29.281 Sec. 4.3.1.
  /// \param dst_addr The destination address for the echo response message.
  /// \param sn The sequence number of the initiating echo request message.
  virtual void send_echo_response(const sockaddr_storage& dst_addr, uint16_t sn) = 0;

  /// \brief Handles an echo response message, disables T3-RESPONSE for the request with sequence number \c sn.
  /// \param src_addr The source address of the echo response message.
  /// \param sn The sequence number of the echo response message and the initiating echo request message.
  virtual void handle_echo_response(const sockaddr_storage& src_addr, uint16_t sn) = 0;
};

} // namespace srsran
