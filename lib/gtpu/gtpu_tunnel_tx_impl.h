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

#include "gtpu_pdu.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/gtpu/gtpu_config.h"
#include "srsgnb/gtpu/gtpu_tunnel_tx.h"
#include <arpa/inet.h>
#include <cstdint>
#include <netinet/in.h>

namespace srsgnb {

/// Class used for transmitting GTP-U bearers.
class gtpu_tunnel_tx : public gtpu_tunnel_tx_lower_layer_interface
{
public:
  gtpu_tunnel_tx(uint32_t ue_index, gtpu_config::gtpu_tx_config cfg_, gtpu_tunnel_tx_upper_layer_notifier& upper_dn_) :
    logger("GTPU", {ue_index, "peer", cfg_.peer_teid}), cfg(cfg_), upper_dn(upper_dn_)
  {
    // Validate configuration
    if (inet_pton(AF_INET, cfg.peer_addr.c_str(), &((::sockaddr_in*)&peer_sockaddr)->sin_addr) == 1) {
      ((::sockaddr_in*)&peer_sockaddr)->sin_family = AF_INET;
      ((::sockaddr_in*)&peer_sockaddr)->sin_port   = htons(cfg.peer_port);
    } else if (inet_pton(AF_INET6, cfg.peer_addr.c_str(), &((::sockaddr_in6*)&peer_sockaddr)->sin6_addr) == 1) {
      ((::sockaddr_in6*)&peer_sockaddr)->sin6_family = AF_INET6;
      ((::sockaddr_in6*)&peer_sockaddr)->sin6_port   = htons(cfg.peer_port);
    } else {
      logger.log_error("Could not get peer address. Configured address={}, errno={}", cfg.peer_addr, strerror(errno));
    }

    logger.log_info("GTPU TX entity configured. Configuration={}", cfg);
  }

  /*
   * SDU/PDU handlers
   */
  void handle_sdu(byte_buffer buf) final
  {
    gtpu_header hdr         = {};
    hdr.flags.version       = GTPU_FLAGS_VERSION_V1;
    hdr.flags.protocol_type = GTPU_FLAGS_GTP_PROTOCOL;
    hdr.message_type        = GTPU_MSG_DATA_PDU;
    hdr.length              = buf.length();
    hdr.teid                = cfg.peer_teid;
    bool write_ok           = gtpu_write_header(buf, hdr, logger);
    if (!write_ok) {
      logger.log_error("Error writing GTP-U header, discarding.");
      return;
    }
    logger.log_info(buf.begin(), buf.end(), "TX GTP-U PDU (len={} B, teid={})", buf.length(), hdr.teid);
    upper_dn.on_new_pdu(std::move(buf), peer_sockaddr);
  }

private:
  gtpu_tunnel_logger                   logger;
  const gtpu_config::gtpu_tx_config    cfg;
  gtpu_tunnel_tx_upper_layer_notifier& upper_dn;
  sockaddr_storage                     peer_sockaddr;
};
} // namespace srsgnb
