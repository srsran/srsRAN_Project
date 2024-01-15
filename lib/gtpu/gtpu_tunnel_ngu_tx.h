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
#include "srsran/adt/byte_buffer.h"
#include "srsran/gtpu/gtpu_config.h"
#include "srsran/gtpu/gtpu_tunnel_tx.h"
#include "srsran/support/bit_encoding.h"
#include <arpa/inet.h>
#include <cstdint>
#include <netinet/in.h>

namespace srsran {

/// Class used for transmitting GTP-U bearers.
class gtpu_tunnel_ngu_tx : public gtpu_tunnel_base_tx, public gtpu_tunnel_tx_lower_layer_interface
{
public:
  gtpu_tunnel_ngu_tx(srs_cu_up::ue_index_t                ue_index,
                     gtpu_config::gtpu_tx_config          cfg_,
                     dlt_pcap&                            gtpu_pcap_,
                     gtpu_tunnel_tx_upper_layer_notifier& upper_dn_) :
    gtpu_tunnel_base_tx(gtpu_tunnel_log_prefix{ue_index, cfg_.peer_teid, "UL"}, gtpu_pcap_, upper_dn_), cfg(cfg_)
  {
    to_sockaddr(peer_sockaddr, cfg.peer_addr.c_str(), cfg.peer_port);
    logger.log_info("GTPU NGU Tx configured. {}", cfg);
  }

  /*
   * SDU/PDU handlers
   */

  void handle_sdu(byte_buffer buf, qos_flow_id_t qfi) final
  {
    gtpu_header hdr         = {};
    hdr.flags.version       = GTPU_FLAGS_VERSION_V1;
    hdr.flags.protocol_type = GTPU_FLAGS_GTP_PROTOCOL;
    hdr.flags.ext_hdr       = true;
    hdr.message_type        = GTPU_MSG_DATA_PDU;
    hdr.length              = buf.length() + 4 + 4;
    hdr.teid                = cfg.peer_teid;
    hdr.next_ext_hdr_type   = gtpu_extension_header_type::pdu_session_container;

    byte_buffer ext_buf;
    bit_encoder encoder{ext_buf};
    encoder.pack(1, 4);                        // PDU type
    encoder.pack(0, 4);                        // unused options
    encoder.pack(0, 1);                        // spare
    encoder.pack(qos_flow_id_to_uint(qfi), 7); // QFI

    gtpu_extension_header ext;
    ext.extension_header_type = gtpu_extension_header_type::pdu_session_container;
    ext.container             = ext_buf;

    hdr.ext_list.push_back(ext);

    bool write_ok = gtpu_write_header(buf, hdr, logger);

    if (!write_ok) {
      logger.log_error("Dropped SDU, error writing GTP-U header. teid={}", hdr.teid);
      return;
    }
    logger.log_info(buf.begin(), buf.end(), "TX PDU. pdu_len={} teid={} qfi={}", buf.length(), hdr.teid, qfi);
    send_pdu(std::move(buf), peer_sockaddr);
  }

private:
  const gtpu_config::gtpu_tx_config cfg;
  sockaddr_storage                  peer_sockaddr;
};
} // namespace srsran
