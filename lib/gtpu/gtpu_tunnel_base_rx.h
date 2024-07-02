/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "gtpu_pdu.h"
#include "gtpu_tunnel_logger.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/gtpu/gtpu_tunnel_common_rx.h"

namespace srsran {

/// Class used for receiving GTP-U bearers.
class gtpu_tunnel_base_rx : public gtpu_tunnel_common_rx_upper_layer_interface
{
public:
  gtpu_tunnel_base_rx(gtpu_tunnel_log_prefix log_prefix, bool test_mode_ = false) :
    logger("GTPU", std::move(log_prefix)), test_mode(test_mode_)
  {
  }
  ~gtpu_tunnel_base_rx() override = default;

  /*
   * SDU/PDU handlers
   */
  void handle_pdu(byte_buffer buf, const sockaddr_storage& src_addr) final
  {
    gtpu_dissected_pdu dissected_pdu;
    if (test_mode) {
      dissected_pdu.test_mode = true;
      dissected_pdu.buf       = std::move(buf);
    } else {
      bool read_ok = gtpu_dissect_pdu(dissected_pdu, std::move(buf), logger);
      if (!read_ok) {
        logger.log_error("Dropped PDU, error reading GTP-U header. pdu_len={}", dissected_pdu.buf.length());
        return;
      }
    }
    // continue processing in domain-specific subclass
    handle_pdu(std::move(dissected_pdu), src_addr);
  }

protected:
  virtual void handle_pdu(gtpu_dissected_pdu&& pdu, const sockaddr_storage& src_addr) = 0;

  gtpu_tunnel_logger logger;
  bool               test_mode;
};
} // namespace srsran
