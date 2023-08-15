/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "gtpu_pdu.h"
#include "gtpu_tunnel_base_tx.h"
#include "srsran/gtpu/gtpu_config.h"
#include "srsran/gtpu/gtpu_tunnel_tx.h"
#include <arpa/inet.h>
#include <cstdint>
#include <netinet/in.h>

namespace srsran {

/// Class used for transmission of GTP-U path management messages.
class gtpu_echo_tx : public gtpu_tunnel_base_tx
{
public:
  gtpu_echo_tx(gtpu_echo_tx_config cfg_, dlt_pcap& gtpu_pcap_, gtpu_tunnel_tx_upper_layer_notifier& upper_dn_) :
    gtpu_tunnel_base_tx(gtpu_tunnel_log_prefix{{}, GTPU_PATH_MANAGEMENT_TEID, "UL"}, gtpu_pcap_, upper_dn_), cfg(cfg_)
  {
    to_sockaddr(peer_sockaddr, cfg.peer_addr.c_str(), cfg.peer_port);
    logger.log_info("GTPU echo Tx configured. {}", cfg);
  }
  ~gtpu_echo_tx() = default;

  void send_echo_request()
  {
    gtpu_header hdr         = {};
    hdr.flags.version       = GTPU_FLAGS_VERSION_V1;
    hdr.flags.protocol_type = GTPU_FLAGS_GTP_PROTOCOL;
    hdr.flags.ext_hdr       = false;
    hdr.flags.seq_number    = true;
    hdr.message_type        = GTPU_MSG_ECHO_REQUEST;
    hdr.length              = 2; // Includes the SN because it is considered to be part of the payload
    hdr.teid                = GTPU_PATH_MANAGEMENT_TEID;
    hdr.seq_number          = sn_next;

    byte_buffer buf;
    bool        write_ok = gtpu_write_header(buf, hdr, logger);

    if (!write_ok) {
      logger.log_error("Dropped SDU, error writing GTP-U header of echo request. teid={}", hdr.teid);
      return;
    }

    sn_next++;
    logger.log_info(buf.begin(), buf.end(), "TX echo request. pdu_len={} teid={}", buf.length(), hdr.teid);
    send_pdu(std::move(buf), peer_sockaddr);
  }

  void send_echo_response(sockaddr_storage origin_sockaddr)
  {
    gtpu_header hdr         = {};
    hdr.flags.version       = GTPU_FLAGS_VERSION_V1;
    hdr.flags.protocol_type = GTPU_FLAGS_GTP_PROTOCOL;
    hdr.flags.ext_hdr       = false;
    hdr.flags.seq_number    = true;
    hdr.message_type        = GTPU_MSG_ECHO_RESPONSE;
    hdr.length              = 2; // Includes the SN because it is considered to be part of the payload
    hdr.teid                = GTPU_PATH_MANAGEMENT_TEID;
    hdr.seq_number          = sn_next;

    byte_buffer buf;
    bool        write_ok = gtpu_write_header(buf, hdr, logger);

    if (!write_ok) {
      logger.log_error("Dropped SDU, error writing GTP-U header of echo response. teid={}", hdr.teid);
      return;
    }

    sn_next++;
    logger.log_info(buf.begin(), buf.end(), "TX echo respnse. pdu_len={} teid={}", buf.length(), hdr.teid);
    send_pdu(std::move(buf), origin_sockaddr);
  }

private:
  const gtpu_echo_tx_config cfg;
  sockaddr_storage          peer_sockaddr;
  uint32_t                  sn_next = 0;
};
} // namespace srsran
