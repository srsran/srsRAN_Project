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

#include "gtpu_tunnel_logger.h"
#include "srsran/gtpu/gtpu_tunnel_tx.h"
#include "srsran/pcap/pcap.h"
#include <arpa/inet.h>
#include <cstdint>
#include <netinet/in.h>

namespace srsran {

/// Class used for transmitting GTP-U bearers.
class gtpu_tunnel_base_tx
{
public:
  gtpu_tunnel_base_tx(gtpu_tunnel_log_prefix               log_prefix,
                      dlt_pcap&                            gtpu_pcap_,
                      gtpu_tunnel_tx_upper_layer_notifier& upper_dn_) :
    logger("GTPU", log_prefix), gtpu_pcap(gtpu_pcap_), upper_dn(upper_dn_)
  {
  }

  bool to_sockaddr(sockaddr_storage& out_sockaddr, const char* addr, uint16_t port)
  {
    // Validate configuration
    if (inet_pton(AF_INET, addr, &((::sockaddr_in*)&out_sockaddr)->sin_addr) == 1) {
      ((::sockaddr_in*)&out_sockaddr)->sin_family = AF_INET;
      ((::sockaddr_in*)&out_sockaddr)->sin_port   = htons(port);
    } else if (inet_pton(AF_INET6, addr, &((::sockaddr_in6*)&out_sockaddr)->sin6_addr) == 1) {
      ((::sockaddr_in6*)&out_sockaddr)->sin6_family = AF_INET6;
      ((::sockaddr_in6*)&out_sockaddr)->sin6_port   = htons(port);
    } else {
      logger.log_error("Invalid address or port. addr={} port={} errno={}", addr, port, strerror(errno));
      return false;
    }
    return true;
  }

protected:
  gtpu_tunnel_logger logger;

  void send_pdu(byte_buffer buf, const ::sockaddr_storage& peer_sockaddr)
  {
    if (gtpu_pcap.is_write_enabled()) {
      gtpu_pcap.push_pdu(buf.deep_copy());
    }

    upper_dn.on_new_pdu(std::move(buf), peer_sockaddr);
  }

private:
  dlt_pcap&                            gtpu_pcap;
  gtpu_tunnel_tx_upper_layer_notifier& upper_dn;
};
} // namespace srsran
