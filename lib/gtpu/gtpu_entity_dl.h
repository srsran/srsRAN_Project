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
#include "srsgnb/gtpu/gtpu_dl.h"
#include <cstdint>

namespace srsgnb {

/// Class used for receiving GTP-U bearers.
class gtpu_entity_dl : public gtpu_dl_upper_layer_interface
{
public:
  gtpu_entity_dl(uint32_t ue_index, gtpu_config::gtpu_dl_config cfg_, gtpu_dl_lower_layer_notifier& dl_lower_) :
    logger(srslog::fetch_basic_logger("GTPU")), cfg(cfg_), lower_dn(dl_lower_)
  {
    // Validate configuration
    logger.info("GTPU DL entity configured.");
  }

  /*
   * SDU/PDU handlers
   */
  void handle_sdu(byte_buffer buf) final
  {
    gtpu_header hdr;
    bool        read_ok = gtpu_read_and_strip_header(hdr, buf, logger);
    if (!read_ok) {
      logger.error("Error reading GTP-U header, discarding.");
      return;
    }
    if (hdr.teid != cfg.src_teid) {
      logger.error("Mismatched TEID, discarding. Header TEID={}, config TEID={}", hdr.teid, cfg.src_teid);
      return;
    }
    lower_dn.on_new_pdu(std::move(buf));
  }

private:
  srslog::basic_logger&             logger;
  const gtpu_config::gtpu_dl_config cfg;
  gtpu_dl_lower_layer_notifier&     lower_dn;
};
} // namespace srsgnb
