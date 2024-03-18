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
      logger.log_error("Discarded PDU. Cause: Invalid TEID for path management message. teid={} msg_type={:#x}",
                       pdu.hdr.teid,
                       pdu.hdr.message_type);
      return;
    }
    // SN sanity check
    if (!pdu.hdr.flags.seq_number) {
      logger.log_error("Discarded PDU. Cause: Missing sequence number in path management message. msg_type={:#x}",
                       pdu.hdr.message_type);
      return;
    }
    switch (pdu.hdr.message_type) {
      case GTPU_MSG_ECHO_REQUEST:
        logger.log_info("Rx echo request. sn={}", pdu.hdr.seq_number);
        tx.send_echo_response(src_addr, pdu.hdr.seq_number);
        break;
      case GTPU_MSG_ECHO_RESPONSE:
        logger.log_info("Rx echo response. sn={}", pdu.hdr.seq_number);
        tx.handle_echo_response(src_addr, pdu.hdr.seq_number);
        break;
      case GTPU_MSG_SUPPORTED_EXTENSION_HEADERS_NOTIFICATION:
        logger.log_warning("Discarded PDU. Cause: 'Supported extension headers notification' not supported. sn={}",
                           pdu.hdr.seq_number);
        // TS 29.281 Sec. 5.1:
        // For Supported Extension Headers Notification [...], the Sequence Number shall be ignored by the receiver,
        // even though the S flag is set to '1'.
        return;
      case GTPU_MSG_ERROR_INDICATION: {
        gtpu_msg_error_indication err_ind = {};
        byte_buffer               msg     = gtpu_extract_msg(std::move(pdu));
        bool                      msg_ok  = gtpu_read_msg_error_indication(err_ind, msg, logger.get_basic_logger());
        if (!msg_ok) {
          logger.log_error(pdu.buf.begin(), pdu.buf.end(), "Received malfomed error indication from peer");
          return;
        }
        logger.log_info("Received error indication from peer. {}", err_ind);
        // TODO: implement handling as specified in TS 23.527 Sec. 5.3.3.1:
        // Upon receipt of a GTP-U Error Indication, the 5G-AN shall proceed as follows:
        // - if the GTP-U Error Indication was received from an UPF over a NG-U tunnel that is not an indirect
        // forwarding tunnel, the 5G-AN shall initiate a PDU Session Resource Notify procedure and release immediately
        // the resources of the PDU session for which the Error Indication was received;
        // - if the GTP-U Error Indication was received from a peer5G-AN over a Xn-U direct forwarding tunnel or an UPF
        // over a NG-U indirect forwarding tunnel, the 5G-AN may ignore the error indication or delete the forwarding
        // tunnel context locally without deleting the corresponding PDU session and bearers.
        // NOTE: The 5G-AN behaviour for dual connectivity is not described in this specification.
        return;
      }
      case GTPU_MSG_END_MARKER:
        // TODO: unpack and print information elements; add handling
        logger.log_warning("Discarded PDU. Cause: 'End marker' not supported");
        return;
      default:
        logger.log_error(
            "Discarded PDU. Cause: Invalid message type for path or tunnel management message. teid={} msg_type={:#x}",
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
