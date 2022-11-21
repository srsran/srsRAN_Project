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
#include "srsgnb/gtpu/gtpu_rx.h"
#include <cstdint>

namespace srsgnb {

/// Class used for receiving GTP-U bearers.
class gtpu_entity_rx : public gtpu_rx_upper_layer_interface
{
public:
  gtpu_entity_rx(uint32_t ue_index, gtpu_config::gtpu_rx_config cfg_, gtpu_rx_lower_layer_notifier& rx_lower_) :
    logger(srslog::fetch_basic_logger("GTPU")), cfg(cfg_), lower_dn(rx_lower_)
  {
    // Validate configuration
    logger.info("GTPU DL entity configured.");
  }

  /*
   * SDU/PDU handlers
   */
  void handle_pdu(byte_buffer buf) final
  {
    gtpu_header hdr;
    bool        read_ok = gtpu_read_and_strip_header(hdr, buf, logger);
    if (!read_ok) {
      logger.error("Error reading GTP-U header, discarding.");
      return;
    }
    if (hdr.teid != cfg.local_teid) {
      logger.error("Mismatched TEID, discarding. Header TEID={}, local TEID={}", hdr.teid, cfg.local_teid);
      return;
    }
    lower_dn.on_new_sdu(std::move(buf));
  }

private:
  srslog::basic_logger&             logger;
  const gtpu_config::gtpu_rx_config cfg;
  gtpu_rx_lower_layer_notifier&     lower_dn;
};
} // namespace srsgnb
