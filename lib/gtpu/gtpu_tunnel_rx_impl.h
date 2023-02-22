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

#include "gtpu_pdu.h"
#include "gtpu_tunnel_logger.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/gtpu/gtpu_config.h"
#include "srsran/gtpu/gtpu_tunnel_rx.h"
#include <cstdint>

namespace srsran {

/// Class used for receiving GTP-U bearers.
class gtpu_tunnel_rx : public gtpu_tunnel_rx_upper_layer_interface
{
public:
  gtpu_tunnel_rx(uint32_t ue_index, gtpu_config::gtpu_rx_config cfg_, gtpu_tunnel_rx_lower_layer_notifier& rx_lower_) :
    logger("GTPU", {ue_index, cfg_.local_teid, "DL"}), cfg(cfg_), lower_dn(rx_lower_)
  {
    // Validate configuration
    logger.log_info("GTPU configured. {}", cfg);
  }

  /*
   * SDU/PDU handlers
   */
  void handle_pdu(byte_buffer buf) final
  {
    gtpu_header hdr;
    bool        read_ok = gtpu_read_and_strip_header(hdr, buf, logger);
    if (!read_ok) {
      logger.log_error("Dropped PDU, error reading GTP-U header. sdu_len={}", buf.length());
      return;
    }
    if (hdr.teid != cfg.local_teid) {
      logger.log_error("Dropped PDU, mismatched TEID. sdu_len={} teid={:#x}", buf.length(), hdr.teid);
      return;
    }
    logger.log_info(buf.begin(), buf.end(), "RX SDU. sdu_len={} teid={:#x}", buf.length(), hdr.teid);
    lower_dn.on_new_sdu(std::move(buf));
  }

private:
  gtpu_tunnel_logger                   logger;
  const gtpu_config::gtpu_rx_config    cfg;
  gtpu_tunnel_rx_lower_layer_notifier& lower_dn;
};
} // namespace srsran
