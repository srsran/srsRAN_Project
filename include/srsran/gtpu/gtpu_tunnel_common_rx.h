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

#include "srsran/adt/byte_buffer.h"

struct sockaddr_storage;

/*
 * This file will hold the interfaces and notifiers for the GTP-U tunnel.
 * They follow the following nomenclature:
 *
 *   gtpu_tunnel[_DOMAIN]_{tx/rx}_{lower/upper}_layer_{interface/notifier}
 *
 * 1. TX/RX indicates whether the interface is intended for the
 *    TX or RX side of the tunnel. TX/RX terminology is used from the
 *    perspective of the GTP-U, i.e. are we receiving or sending packets to
 *    the socket gateway.
 * 2. Lower/Upper indicates whether the interface/notifier interacts
 *    with the upper or lower layers.
 * 3. Interface/Notifier: whether this is an interface the GTP-U tunnel will
 *    inherit or a notifier that the GTP-U will keep as a member.
 * 4. DOMAIN indicates the GTP-U specialization for a particular domain {NG-U (N3), NR-U (F1-U),...}
 *
 */

namespace srsran {

/****************************************
 * Interfaces/notifiers for the gateway
 ****************************************/
/// This interface represents the data entry point of the receiving side of a GTP-U entity.
/// The IO gateway will use this call to pass GTP-U PDUs into the RX entity.
class gtpu_tunnel_common_rx_upper_layer_interface
{
public:
  gtpu_tunnel_common_rx_upper_layer_interface()                                                               = default;
  virtual ~gtpu_tunnel_common_rx_upper_layer_interface()                                                      = default;
  gtpu_tunnel_common_rx_upper_layer_interface(const gtpu_tunnel_common_rx_upper_layer_interface&)             = delete;
  gtpu_tunnel_common_rx_upper_layer_interface& operator=(const gtpu_tunnel_common_rx_upper_layer_interface&)  = delete;
  gtpu_tunnel_common_rx_upper_layer_interface(const gtpu_tunnel_common_rx_upper_layer_interface&&)            = delete;
  gtpu_tunnel_common_rx_upper_layer_interface& operator=(const gtpu_tunnel_common_rx_upper_layer_interface&&) = delete;

  /// \brief Interface for the IO gateway to pass PDUs into the GTP-U
  /// \param pdu PDU to be handled
  /// \param src_addr Source address of this PDU
  virtual void handle_pdu(byte_buffer pdu, const sockaddr_storage& src_addr) = 0;
};

} // namespace srsran
