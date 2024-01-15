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
#include "gtpu_tunnel_logger.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/gtpu/gtpu_tunnel_rx.h"
#include <cstdint>

namespace srsran {

/// Class used for receiving GTP-U bearers.
class gtpu_tunnel_base_rx : public gtpu_tunnel_rx_upper_layer_interface
{
public:
  gtpu_tunnel_base_rx(gtpu_tunnel_log_prefix log_prefix) : logger("GTPU", log_prefix) {}
  ~gtpu_tunnel_base_rx() = default;

  /*
   * SDU/PDU handlers
   */
  void handle_pdu(byte_buffer buf, const sockaddr_storage& src_addr) final
  {
    gtpu_dissected_pdu dissected_pdu;
    bool               read_ok = gtpu_dissect_pdu(dissected_pdu, std::move(buf), logger);
    if (!read_ok) {
      logger.log_error("Dropped PDU, error reading GTP-U header. pdu_len={}", dissected_pdu.buf.length());
      return;
    }

    // continue processing in domain-specific subclass
    handle_pdu(std::move(dissected_pdu), src_addr);
  }

protected:
  virtual void handle_pdu(gtpu_dissected_pdu&& pdu, const sockaddr_storage& src_addr) = 0;

  gtpu_tunnel_logger logger;
};
} // namespace srsran
