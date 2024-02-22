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

#include "gtpu_pdu.h"
#include "gtpu_tunnel_base_tx.h"
#include "srsran/gtpu/gtpu_echo_tx.h"
#include "srsran/gtpu/gtpu_tunnel_tx.h"
#include <arpa/inet.h>
#include <cstdint>
#include <netinet/in.h>

namespace srsran {

/// Class used for transmission of GTP-U path management messages.
class gtpu_echo_tx : public gtpu_tunnel_base_tx, public gtpu_echo_tx_interface
{
public:
  gtpu_echo_tx(dlt_pcap& gtpu_pcap_, gtpu_tunnel_tx_upper_layer_notifier& upper_dn_) :
    gtpu_tunnel_base_tx(gtpu_tunnel_log_prefix{{}, GTPU_PATH_MANAGEMENT_TEID, "UL"}, gtpu_pcap_, upper_dn_)
  {
  }
  ~gtpu_echo_tx() = default;

  void send_echo_request(const sockaddr_storage& dst_addr) override
  {
    byte_buffer buf;

    // TODO: Add optional IE for "private extension"

    gtpu_header hdr         = {};
    hdr.flags.version       = GTPU_FLAGS_VERSION_V1;
    hdr.flags.protocol_type = GTPU_FLAGS_GTP_PROTOCOL;
    hdr.flags.ext_hdr       = false;
    hdr.flags.seq_number    = true;
    hdr.message_type        = GTPU_MSG_ECHO_REQUEST;
    hdr.length              = 0; // length will be computed automatically
    hdr.teid                = GTPU_PATH_MANAGEMENT_TEID;
    hdr.seq_number          = sn_next;

    bool write_ok = gtpu_write_header(buf, hdr, logger);

    if (!write_ok) {
      logger.log_error("Discarded SDU. Cause: Error writing GTP-U header of echo request.");
      return;
    }

    sn_next++;
    logger.log_info(buf.begin(), buf.end(), "TX echo request. sn={} pdu_len={}", hdr.seq_number, buf.length());
    send_pdu(std::move(buf), dst_addr);
  }

  void send_echo_response(const sockaddr_storage& dst_addr, uint16_t sn) override
  {
    byte_buffer buf;

    // Add information element for "recovery" for backward compatibility. See TS 29.281 Sec. 8.2
    gtpu_ie_recovery ie_recovery = {};

    bool write_ok = gtpu_write_ie_recovery(buf, ie_recovery, logger);
    if (!write_ok) {
      logger.log_error("Dropped SDU, error writing IE recovery to echo response. sn={}", sn);
      return;
    }

    // TODO: Add optional IE for "private extension"

    // Add header
    gtpu_header hdr         = {};
    hdr.flags.version       = GTPU_FLAGS_VERSION_V1;
    hdr.flags.protocol_type = GTPU_FLAGS_GTP_PROTOCOL;
    hdr.flags.ext_hdr       = false;
    hdr.flags.seq_number    = true;
    hdr.message_type        = GTPU_MSG_ECHO_RESPONSE;
    hdr.length              = 0; // length will be computed automatically
    hdr.teid                = GTPU_PATH_MANAGEMENT_TEID;
    hdr.seq_number          = sn; // responses copy the SN of the request, TS 29.281 Sec. 4.3.1

    write_ok = gtpu_write_header(buf, hdr, logger);

    if (!write_ok) {
      logger.log_error("Discarded SDU. Cause: Error writing GTP-U header of echo response. sn={}", sn);
      return;
    }

    logger.log_info(buf.begin(), buf.end(), "TX echo response. sn={} pdu_len={}", sn, buf.length());
    send_pdu(std::move(buf), dst_addr);
  }

  void handle_echo_response(const sockaddr_storage& src_addr, uint16_t sn) override
  {
    logger.log_info("handle_echo_response was called. sn={}", sn);
    // TODO: disarm t3_response
  }

private:
  uint32_t sn_next = 0;
};
} // namespace srsran
