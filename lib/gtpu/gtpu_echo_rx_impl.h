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

#include "gtpu_tunnel_base_rx.h"
#include "srsran/gtpu/gtpu_echo_tx.h"
#include "srsran/psup/psup_packing.h"

namespace srsran {

/// Class used for reception of GTP-U path management messages.
class gtpu_echo_rx : public gtpu_tunnel_base_rx
{
public:
  gtpu_echo_rx(gtpu_echo_tx_interface& tx_) :
    gtpu_tunnel_base_rx(gtpu_tunnel_log_prefix{{}, GTPU_PATH_MANAGEMENT_TEID, "DL"}),
    tx(tx_),
    psup_packer(logger.get_basic_logger())
  {
  }
  ~gtpu_echo_rx() = default;

protected:
  // domain-specific PDU handler
  void handle_pdu(gtpu_dissected_pdu&& pdu, const sockaddr_storage& src_addr) final
  {
    // TEID sanity check
    if (pdu.hdr.teid != GTPU_PATH_MANAGEMENT_TEID) {
      logger.log_error(
          "Invalid TEID for path management message. teid={}, msg_type={:#x}", pdu.hdr.teid, pdu.hdr.message_type);
      return;
    }
    // SN sanity check
    if (!pdu.hdr.flags.seq_number) {
      logger.log_error("Missing sequence number in path management message. teid={}, msg_type={:#x}",
                       pdu.hdr.teid,
                       pdu.hdr.message_type);
      return;
    }
    switch (pdu.hdr.message_type) {
      case GTPU_MSG_ECHO_REQUEST:
        logger.log_info("Rx echo request, teid={}, msg_type={:#x}", pdu.hdr.teid, pdu.hdr.message_type);
        tx.send_echo_response(src_addr);
        break;
      case GTPU_MSG_ECHO_RESPONSE:
        logger.log_warning(
            "Unhandled message: 'Echo Response', teid={}, msg_type={:#x}", pdu.hdr.teid, pdu.hdr.message_type);
        // TODO: disarm t3_response
        break;
      case GTPU_MSG_SUPPORTED_EXTENSION_HEADERS_NOTIFICATION:
        logger.log_warning("Unhandled message: 'Supported Extension Headers Notification', teid={}, msg_type={:#x}",
                           pdu.hdr.teid,
                           pdu.hdr.message_type);
        // TS 29.281 Sec. 5.1:
        // For Supported Extension Headers Notification [...], the Sequence Number shall be ignored by the receiver,
        // even though the S flag is set to '1'.
        return;
      default:
        logger.log_error("Invalid message type for path management message. teid={}, msg_type={:#x}",
                         pdu.hdr.teid,
                         pdu.hdr.message_type);
        return;
    }
  }

private:
  gtpu_echo_tx_interface& tx;
  psup_packing            psup_packer;
};
} // namespace srsran
